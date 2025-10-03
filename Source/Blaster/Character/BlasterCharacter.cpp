// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlasterCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlasterAnimInstance.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABlasterCharacter

ABlasterCharacter::ABlasterCharacter()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	  FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	  JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			 // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;		// The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;								// Camera does not rotate relative to arm

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComp->SetIsReplicated(true); // 设置为复制组件

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// 处理太空舱组件 和 mesh组件 与相机碰撞的问题
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// 方向旋转（orient rotation）到运动设置的旋转速率（rotation rate）
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	IOnlineSubsystem *OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		OnlineSessionInterface = OnlineSub->GetSessionInterface();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Found Online Subsystem: %s"), *OnlineSub->GetSubsystemName().ToString()));
		}
	}
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
}

void ABlasterCharacter::OpenLobby()
{
	UWorld *World = GetWorld();
	if (!World)
		return;

	// Game会替代Content所在目录,相对目录 eg: E:/Github_Project/MultiplayerFPSGame/Content/ThirdPerson/Maps/Lobby.umap
	World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
}

void ABlasterCharacter::CallOpenLevel(const FString &Address)
{
	UGameplayStatics::OpenLevel(this, FName(*Address));
}

void ABlasterCharacter::CallClientTravel(const FString &Address)
{
	APlayerController *PlayerContro = GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerContro)
	{
		PlayerContro->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 注册OverlappingWeapon变量用来replicated，只有满足COND_OwnerOnly条件的client才能接收到这个变量
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

// SetOverlappingWeapon这里只在server中被调用，解决OnRep_OverlappingWeapon不会被server调用的问题
void ABlasterCharacter::SetOverlappingWeapon(AWeapon *Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return CombatComp && CombatComp->EquippedWeapon;
}

bool ABlasterCharacter::IsAiming()
{
	return (CombatComp && CombatComp->bAiming);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComp)
	{
		CombatComp->Character = this;
	}
}

AWeapon *ABlasterCharacter::GetEquippedWeapon()
{
	if (CombatComp == nullptr)
		return nullptr;
	return CombatComp->EquippedWeapon;
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr)
		return;

	UAnimInstance *animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && FireWeaponMontage)
	{
		animInstance->Montage_Play(FireWeaponMontage);
		// 选择正确的section名字
		FName sectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		animInstance->Montage_JumpToSection(sectionName, FireWeaponMontage);
	}
}

FVector ABlasterCharacter::GetHitTarget() const
{
    if (CombatComp == nullptr) return FVector();
	return CombatComp->HitTarget;
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon *LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	// LastWeapon是发生复制复制之前的旧值
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if (CombatComp)
	{
		if (HasAuthority())
		{
			// server
			CombatComp->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			// client   --- 远程过程调用 是 可以在一台机器（client）上调用的函数，并在另一台机器（server）上执行 ---
			// client按 E就会发送RPC请求到server，server会调用ServerEquipButtonPressed_Implementation函数
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if (CombatComp)
	{
		CombatComp->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (CombatComp)
	{
		CombatComp->SetAiming(false);
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (CombatComp && CombatComp->EquippedWeapon == nullptr)
		return;

	FVector velocity = GetVelocity();
	velocity.Z = 0;
	float speed = velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (speed == 0 && !bIsInAir)
	{																				// 站着不动，不跳
		FRotator currentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f); // 鼠标左右移动的值
		FRotator deltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, StartingAimRotation);
		AO_Yaw = deltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (speed > 0.f || bIsInAir) // running, or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f; // 移动过程中 yaw保持0
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch; // 鼠标上下移动的值
	UE_LOG(LogTemp, Warning, TEXT("AO_Pitch: %f"), AO_Pitch);
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		// 这里因为网络传输的原因，数据被压缩了被换成了（0， 360），需要转换回来[-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		/*
			数据在序列化、压缩后通过网络传输发生变化，并解压到（0，360）范围内（因为unsigned格式），需要转换回来[-90, 0)
		*/
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

// 此函数只在Player机器上本地调用；这里播放的蒙太奇动画的action是不复制的
void ABlasterCharacter::FireButtonPressed()
{
	if (CombatComp)
	{
		CombatComp->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (CombatComp)
	{
		CombatComp->FireButtonPressed(false);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("TurnInPlace::AO_Yaw: %f"), AO_Yaw);
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	/*
		_Implementation 必须跟在函数声明后面,约定（所有的server RPC都必须以这个方式命名）
	*/
	if (CombatComp)
	{
		CombatComp->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ABlasterCharacter::CreateGameSession()
{
	UE_LOG(LogTemp, Warning, TEXT("1. CreateGameSession"))
	// called when pressing the 1 key.
	if (!OnlineSessionInterface.IsValid())
		return;

	auto existingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);

	if (existingSession != nullptr)
	{
		// 销毁旧的Session

		UE_LOG(LogTemp, Warning, TEXT("2. existingSession: %s."), *existingSession->SessionName.ToString())
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	/*
		将我们的委托CreateSessionCompleteDelegate对象添加到Session Interface委托列表中,
		一旦创建一个会话完成时，委托会被调用
	*/

	UE_LOG(LogTemp, Warning, TEXT("3. Creating session..."))
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> SessionSetting = MakeShareable(new FOnlineSessionSettings());
	SessionSetting->bIsLANMatch = false;
	SessionSetting->NumPublicConnections = 4;
	SessionSetting->bAllowJoinInProgress = true;
	SessionSetting->bAllowJoinViaPresence = true;
	SessionSetting->bShouldAdvertise = true;
	SessionSetting->bUsesPresence = true;
	SessionSetting->bUseLobbiesIfAvailable = true;
	SessionSetting->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	const ULocalPlayer *localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSetting);
}

void ABlasterCharacter::JoinGameSession()
{
	// Find game session.
	if (!OnlineSessionInterface.IsValid())
		return;
	UE_LOG(LogTemp, Warning, TEXT("4. JoinGameSession"))
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 1000;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void ABlasterCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Green,
				FString::Printf(TEXT("Created session: %s"), *SessionName.ToString()));
		}

		UWorld *World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString::Printf(TEXT("Failed to create session!")));
		}
	}
}

void ABlasterCharacter::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid())
		return;
	UE_LOG(LogTemp, Warning, TEXT("5. OnFindSessionsComplete: %s, SearchResults:%d"), bWasSuccessful ? TEXT("Success") : TEXT("Failed"), SessionSearch->SearchResults.Num());
	for (auto result : SessionSearch->SearchResults)
	{
		FString id = result.GetSessionIdStr();
		FString user = result.Session.OwningUserName;

		FString matchTypeSettingsValue;
		result.Session.SessionSettings.Get(FName("MatchType"), matchTypeSettingsValue);
		UE_LOG(LogTemp, Warning, TEXT("6. EveryResult ID: %s, User:%s, matchTypeSettingsValue: %s"), *id, *user, *matchTypeSettingsValue);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Result ID: %s, User:%s, matchTypeSettingsValue: %s"), *id, *user, *matchTypeSettingsValue));
		}

		// 查找正确的Session匹配类型
		if (matchTypeSettingsValue == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Joining Match Type: %s"), *matchTypeSettingsValue));
			}

			// 连接到游戏会话
			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
			const ULocalPlayer *localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, result);
		}
	}
}

void ABlasterCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
		return;

	// 拿到ip地址
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Connect string: %s"), *Address));
		}
	}

	APlayerController *PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, TRAVEL_Absolute);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABlasterCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);

		/*
			EquipAction
			当按下装备按钮时调用，不管在server还是client都会被调用
		*/
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::EquipButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABlasterCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABlasterCharacter::AimButtonReleased);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABlasterCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterCharacter::FireButtonReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABlasterCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABlasterCharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
