// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "Engine/CancellableAsyncAction.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ListenForItchKey.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReceiveItchAPIKey, const FString&, APIKey);

/**
 * 
 */
UCLASS()
class ITCHINTEGRATION_API UAsyncAction_ListenForItchKey : public UCancellableAsyncAction
{
	GENERATED_BODY()

	// Opens an httpserver so that it can listen for itch OAuth requests. When the API key is received, we return and finish.
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", Category = "Itch Integration",  WorldContext = "WorldContextObject"))
	static UAsyncAction_ListenForItchKey* ListenForItchKey(UObject* WorldContextObject);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	UPROPERTY(BlueprintAssignable)
	FReceiveItchAPIKey OnReceiveItchAPIKey;

	DECLARE_LOG_CATEGORY_CLASS(LogListenForItchKey, Log, All);
	
private:

	// Get initial request
	bool RequestGET(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	// Get api key
	bool RequestPOST(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	
	TArray<FHttpRouteHandle> Handles;
};
