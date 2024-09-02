#include "HttpServer.h"

#include "HttpRequestHandler.h"
#include "HttpServerModule.h"
#include "HttpServerModule.h"
#include "HttpServerRequest.h"
#include "IHttpRouter.h"

AHttpServer::AHttpServer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHttpServer::BeginPlay()
{
	StartServer();
	Super::BeginPlay();
}

void AHttpServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	StopServer();
}

void AHttpServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHttpServer::StartServer()
{
	if (ServerPort <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not start HttpServer, port number must be greater than zero!"));
		return;
	}

	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(ServerPort);

	// If port already binded by another process, then this check must be failed
	// !!! BUT !!!
	// this check always true
	// I don't no why...
	if (httpRouter.IsValid())
	{
		// You can bind as many routes as you need
		Handles.Emplace(httpRouter->BindRoute(FHttpPath(HttpPathGET), EHttpServerRequestVerbs::VERB_GET,
			FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return RequestGET(Request, OnComplete); })));
		
		Handles.Emplace(httpRouter->BindRoute(FHttpPath(HttpPathPOST), EHttpServerRequestVerbs::VERB_POST,
			FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return RequestPOST(Request, OnComplete); })));
		httpRouter->BindRoute(FHttpPath(HttpPathPUT), EHttpServerRequestVerbs::VERB_PUT,
			FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return RequestPUT(Request, OnComplete); }));
		httpServerModule.StartAllListeners();

		_isServerStarted = true;
		UE_LOG(LogTemp, Log, TEXT("Web server started on port = %d"), ServerPort);
	}
	else
	{
		_isServerStarted = false;
		UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = %d"), ServerPort);
	}
}

void AHttpServer::StopServer()
{
	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(ServerPort);
	for (auto& RouteHandle : Handles)
	{
		httpRouter->UnbindRoute(RouteHandle);
	}
	Handles.Empty();
	
	httpServerModule.StopAllListeners();
}

bool AHttpServer::RequestGET(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	RequestPrint(Request);
	//TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample GET"), TEXT("text/html"));
	auto Response = FHttpServerResponse::Create(ResponseHTML, TEXT("text/html"));
	OnComplete(MoveTemp(Response));
	return true;
}

bool AHttpServer::RequestPOST(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	RequestPrint(Request);
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample POST"), TEXT("text/html"));
	OnComplete(MoveTemp(response));
	
	FUTF8ToTCHAR bodyTCHARData(reinterpret_cast<const ANSICHAR*>(Request.Body.GetData()), Request.Body.Num());
	FString strBodyData{ bodyTCHARData.Length(), bodyTCHARData.Get() };
	APIKey = strBodyData;
	OnGetAPIKey.Broadcast(APIKey);
	
	return true;
}

bool AHttpServer::RequestPUT(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	RequestPrint(Request);
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample PUT"), TEXT("text/html"));
	OnComplete(MoveTemp(response));
	return true;
}

void AHttpServer::RequestPrint(const FHttpServerRequest& Request, bool PrintBody)
{
	FString strRequestType;
	switch (Request.Verb)
	{
	case EHttpServerRequestVerbs::VERB_GET:
		strRequestType = TEXT("GET");
		break;
	case EHttpServerRequestVerbs::VERB_POST:
		strRequestType = TEXT("POST");
		break;
	case EHttpServerRequestVerbs::VERB_PUT:
		strRequestType = TEXT("PUT");
		break;
	default:
		strRequestType = TEXT("Invalid");
	}
	UE_LOG(LogTemp, Log, TEXT("RequestType = '%s'"), *strRequestType);

	HttpVersion::EHttpServerHttpVersion httpVersion{ Request.HttpVersion };
	UE_LOG(LogTemp, Log, TEXT("HttpVersion = '%s'"), *HttpVersion::ToString(httpVersion));

	UE_LOG(LogTemp, Log, TEXT("RelativePath = '%s'"), *Request.RelativePath.GetPath());

	for (const auto& header : Request.Headers)
	{
		FString strHeaderVals;
		for (const auto& val : header.Value)
		{
			strHeaderVals += TEXT("'") + val + TEXT("' ");
		}
		UE_LOG(LogTemp, Log, TEXT("Header = '%s' : %s"), *header.Key, *strHeaderVals);
	}

	for (const auto& pathParam : Request.PathParams)
	{
		UE_LOG(LogTemp, Log, TEXT("PathParam = '%s' : '%s'"), *pathParam.Key, *pathParam.Value);
	}

	for (const auto& queryParam : Request.QueryParams)
	{
		UE_LOG(LogTemp, Log, TEXT("QueryParam = '%s' : '%s'"), *queryParam.Key, *queryParam.Value);
	}

	// Convert UTF8 to FString
	FUTF8ToTCHAR bodyTCHARData(reinterpret_cast<const ANSICHAR*>(Request.Body.GetData()), Request.Body.Num());
	FString strBodyData{ bodyTCHARData.Length(), bodyTCHARData.Get() };

	UE_LOG(LogTemp, Log, TEXT("Body = '%s'"), *strBodyData);
};