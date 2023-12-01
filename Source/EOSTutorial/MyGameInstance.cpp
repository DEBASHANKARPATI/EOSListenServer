// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include <Kismet/GameplayStatics.h>
#include "Interfaces/OnlineIdentityInterface.h"

void UMyGameInstance::LogInWithEOS(const FString& ID, const FString& Token, const FString& LogInType)
{
	IOnlineSubsystem* SubSystem = Online::GetSubsystem(this->GetWorld());
	if (SubSystem) // validate subsystem
	{
		IOnlineIdentityPtr IdentityPtr = SubSystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			FOnlineAccountCredentials OnlineAccountCredentials;
			OnlineAccountCredentials.Id = ID;
			OnlineAccountCredentials.Token = Token;
			OnlineAccountCredentials.Type = LogInType;
			IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UMyGameInstance::OnLogInCompleted);
			IdentityPtr->Login(0, OnlineAccountCredentials);

		}
	}

}

void UMyGameInstance::OnLogInCompleted(int32 LocalUserNumber, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login successful"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LogIn Failed reason ---- %S"),*Error);
	}
}

FString UMyGameInstance::GetUserName() const
{
	IOnlineSubsystem* SubSystem = Online::GetSubsystem(this->GetWorld());
	if (SubSystem) // validate subsystem
	{
		IOnlineIdentityPtr IdentityPtr = SubSystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			if (IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return IdentityPtr->GetPlayerNickname(0);
			}
		}
	}
	return FString();
}

bool UMyGameInstance::bIsUserLoggedIn() const
{
	IOnlineSubsystem* SubSystem = Online::GetSubsystem(this->GetWorld());
	if (SubSystem) // validate subsystem
	{
		IOnlineIdentityPtr IdentityPtr = SubSystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			if (IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return true;
			}
		}
	}
	return false;
}

void UMyGameInstance::CreateSession(bool bIsDedicatedServer, bool bIsLANServer, int32 NumberofPublicConnections , const FString& ServerMap)
{
	IOnlineSubsystem* SubSystem = Online::GetSubsystem(this->GetWorld());
	if (SubSystem) // validate subsystem
	{
	   IOnlineSessionPtr SessionPtr = SubSystem->GetSessionInterface();
	   if (SessionPtr)
	   {
		   FOnlineSessionSettings OnlineSessionSettings;
		   OnlineSessionSettings.bAllowInvites = true;
		   OnlineSessionSettings.bIsDedicated = bIsDedicatedServer;
		   OnlineSessionSettings.bIsLANMatch = bIsLANServer;
		   OnlineSessionSettings.NumPublicConnections = NumberofPublicConnections;
		   OnlineSessionSettings.bUseLobbiesIfAvailable = false;
		   OnlineSessionSettings.bShouldAdvertise = true;
		   OnlineSessionSettings.Set(SEARCH_KEYWORDS, FString("RandomHI"), EOnlineDataAdvertisementType::ViaOnlineService);
		   SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionCompleted);
		   SessionPtr->CreateSession(0, FName("MainSession"), OnlineSessionSettings);
		   ServerMapText = ServerMap;
	   }
	}
}

void UMyGameInstance::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session creation successful"));
		UE_LOG(LogTemp, Warning, TEXT("Travelling to %S map"),*ServerMapText);

		GetWorld()->ServerTravel(ServerMapText);
	}
}

void UMyGameInstance::FindSessionsAndJoin()
{
	IOnlineSubsystem* SubSystem = Online::GetSubsystem(this->GetWorld());
	if (SubSystem) // validate subsystem
	{
		IOnlineSessionPtr SessionPtr = SubSystem->GetSessionInterface();
		if (SessionPtr)
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, false , EOnlineComparisonOp::Equals );
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->bIsLanQuery = false;

			SessionSearch->QuerySettings.SearchParams.Empty();

			SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionCompleted);
			SessionPtr->FindSessions(0, SessionSearch.ToSharedRef());
		}
	}
}

void UMyGameInstance::OnFindSessionCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		IOnlineSubsystem* SubSystem = Online::GetSubsystem(this->GetWorld());
		if (SubSystem) // validate subsystem
		{
			IOnlineSessionPtr SessionPtr = SubSystem->GetSessionInterface();
			if (SessionPtr)
			{
				if (SessionSearch->SearchResults.Num() > 0)
				{
					SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionCompleted);
					SessionPtr->JoinSession(0, FName("MainSession"), SessionSearch->SearchResults[0]);
				}
				else
				{
					CreateSession(false, false, 5, ServerMapText);
				}
			}
		}

	}
	else
	{
		CreateSession(false, false, 5, ServerMapText);
	}
}

void UMyGameInstance::JoinSession()
{
}

void UMyGameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString JoinAddress;
		if (APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this->GetWorld(), 0))
		{
			IOnlineSubsystem* SubSystem = Online::GetSubsystem(this->GetWorld());
			if (SubSystem) // validate subsystem
			{
				IOnlineSessionPtr SessionPtr = SubSystem->GetSessionInterface();
				if (SessionPtr)
				{
					SessionPtr->GetResolvedConnectString(FName("MainSession"), JoinAddress);

					if (!JoinAddress.IsEmpty())
					{
						LocalPlayerController->ClientTravel(JoinAddress,TRAVEL_Absolute);
					}
				}
			}
		}
	}
}

void UMyGameInstance::DestroySession()
{
	IOnlineSubsystem* SubSystem = Online::GetSubsystem(this->GetWorld());
	if (SubSystem) // validate subsystem
	{
		IOnlineSessionPtr SessionPtr = SubSystem->GetSessionInterface();
		if (SessionPtr)
		{
			SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnSessionDestroyed);
			SessionPtr->DestroySession("MainSession");
		}
	}
}

void UMyGameInstance::OnSessionDestroyed(FName SessionName , bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sesssion successfully destroyed"));
	}
}
