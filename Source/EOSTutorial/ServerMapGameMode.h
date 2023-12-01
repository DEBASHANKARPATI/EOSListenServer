// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ServerMapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class EOSTUTORIAL_API AServerMapGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* NewPlayer)override;

};
