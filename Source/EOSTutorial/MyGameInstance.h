// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EOSTUTORIAL_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable , Category = "EOS Functions")
	void LogInWithEOS(const FString & ID , const FString & Token , const FString & LogInType);

	void OnLogInCompleted(int32 LocalUserNumber , bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(Blueprintcallable,BlueprintPure, Category = "EOS Functions")
	FString GetUserName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure , Category = "EOS Functions")
	bool bIsUserLoggedIn() const;

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void CreateSession(bool bIsDedicatedServer, bool bIsLANServer, int32 NumberofPublicConnections , const FString& ServerMap);
	
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessFul);

	UFUNCTION(BlueprintCallable , Category = "EOS Functions")
	void FindSessionsAndJoin();

	void OnFindSessionCompleted(bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void JoinSession();

	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

    UFUNCTION(BlueprintCallable, Category = "EOS Functions")
	void DestroySession();

	void OnSessionDestroyed(FName SessionName, bool bWasSuccessful);

	UPROPERTY(EditAnywhere ,  BlueprintReadWrite , Category = "EOS Variable")
	FString ServerMapText;
};
