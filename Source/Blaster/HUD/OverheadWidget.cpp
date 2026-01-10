// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::setDisplayText(FString &textToDisplay)
{
    if(mDisplayTextPtr)
    {
        mDisplayTextPtr->SetText(FText::FromString(textToDisplay));
    }
}

void UOverheadWidget::showPlayerNetRole(APawn *InPawn)
{
    ENetRole remoteRole = InPawn->GetRemoteRole();
    FString role;
    switch (remoteRole)
    {
    case ENetRole::ROLE_Authority:  // server
        role = FString("Authority");
        break;
    case ENetRole::ROLE_AutonomousProxy:
        role = FString("Autonomous Proxy");
        break;
    case ENetRole::ROLE_SimulatedProxy:
        role = FString("Simulated Proxy");
        break;
    case ENetRole::ROLE_None:
        role = FString("None");
        break;
    }

    FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *role);
    setDisplayText(RemoteRoleString);    
}

void UOverheadWidget::NativeDestruct()
{
    RemoveFromParent();
	Super::NativeDestruct();
}
