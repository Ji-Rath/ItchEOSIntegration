// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncAction_ListenForItchKey.h"

#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "ItchDeveloperSettings.h"

DEFINE_LOG_CATEGORY_CLASS(UAsyncAction_ListenForItchKey, LogListenForItchKey);

UAsyncAction_ListenForItchKey* UAsyncAction_ListenForItchKey::ListenForItchKey(UObject* WorldContextObject)
{
	auto Action = NewObject<UAsyncAction_ListenForItchKey>();
	Action->RegisterWithGameInstance(WorldContextObject);

	return Action;
}

void UAsyncAction_ListenForItchKey::Activate()
{
	auto ItchSettings = GetDefault<UItchDeveloperSettings>();
	check(ItchSettings);
	
	if (ItchSettings->ServerPort <= 0)
	{
		UE_LOG(LogListenForItchKey, Error, TEXT("Could not start HttpServer, port number must be greater than zero!"));
		SetReadyToDestroy();
		return;
	}

	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(ItchSettings->ServerPort, true);
	
	if (httpRouter.IsValid())
	{
		Handles.Emplace(httpRouter->BindRoute(FHttpPath(ItchSettings->HttpPathGET), EHttpServerRequestVerbs::VERB_GET, FHttpRequestHandler::CreateUObject(this, &ThisClass::RequestGET)));
		
		Handles.Emplace(httpRouter->BindRoute(FHttpPath(ItchSettings->HttpPathPOST), EHttpServerRequestVerbs::VERB_POST, FHttpRequestHandler::CreateUObject(this, &ThisClass::RequestPOST)));
		httpServerModule.StartAllListeners();
		
		UE_LOG(LogListenForItchKey, Log, TEXT("Itch Web server started on port = %d"), ItchSettings->ServerPort);
	}
	else
	{
		UE_LOG(LogListenForItchKey, Error, TEXT("Could not start Itch web server on port = %d"), ItchSettings->ServerPort);
		SetReadyToDestroy();
	}
}

void UAsyncAction_ListenForItchKey::SetReadyToDestroy()
{
	Super::SetReadyToDestroy();

	auto ItchSettings = GetDefault<UItchDeveloperSettings>();
	check(ItchSettings);
	
	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(ItchSettings->ServerPort);
	for (auto& RouteHandle : Handles)
	{
		httpRouter->UnbindRoute(RouteHandle);
	}
	Handles.Empty();
	
	httpServerModule.StopAllListeners();
}

bool UAsyncAction_ListenForItchKey::RequestGET(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	auto ItchSettings = GetDefault<UItchDeveloperSettings>();
	check(ItchSettings);
	
	auto Response = FHttpServerResponse::Create(ItchSettings->ResponseHTML, TEXT("text/html"));
	OnComplete(MoveTemp(Response));
	
	UE_LOG(LogListenForItchKey, Log, TEXT("Received Itch response, sending html to retrieve API key..."));
	return true;
}

bool UAsyncAction_ListenForItchKey::RequestPOST(const FHttpServerRequest& Request,
	const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogListenForItchKey, Log, TEXT("Received Itch response, attempting to read API Key..."));
	
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("Received Itch API key"), TEXT("text/html"));
	OnComplete(MoveTemp(response));
	
	FUTF8ToTCHAR bodyTCHARData(reinterpret_cast<const ANSICHAR*>(Request.Body.GetData()), Request.Body.Num());
	FString strBodyData{ bodyTCHARData.Length(), bodyTCHARData.Get() };
	OnReceiveItchAPIKey.Broadcast(strBodyData);
	SetReadyToDestroy();
	
	return true;
}
