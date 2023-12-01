// Fill out your copyright notice in the Description page of Project Settings.


#include "EOS_PlayerController.h"
#include "MyGameInstance.h"
void AEOS_PlayerController::OnNetCleanup(UNetConnection* Connection)
{

	if (auto GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
	{
		GameInstance->DestroySession();
	}

	Super::OnNetCleanup(Connection);
}
