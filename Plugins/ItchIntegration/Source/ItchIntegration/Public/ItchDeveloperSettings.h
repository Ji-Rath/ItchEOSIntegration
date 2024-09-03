// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ItchDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, DefaultConfig)
class ITCHINTEGRATION_API UItchDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(config)
	FString HttpPathPOST = TEXT("/post");
	
	UPROPERTY(config)
	FString HttpPathGET = TEXT("/get");

	// The port that we use for the httpserver. This should match the port used in the callback url in Itch
	UPROPERTY(EditAnywhere, config, Category = ItchSettings)
	int32 ServerPort = 34567;

	// The response that we send to the user when we would like to retrieve the Itch API key. Will usually contain some javascript and a POST
	// to let the httpserver know about the API key
	UPROPERTY(EditAnywhere, config, Category = ItchSettings, meta=(MultiLine=true))
	FString ResponseHTML = "<div><h3>Login Successful! You may close this window.</h3><pre id=\'result\'></pre></div><script>var queryString = window.location.hash.slice(1); var params = new URLSearchParams(queryString); var accessToken = params.get(\'access_token\'); document.querySelector(\'#result\').textContent = accessToken; var client = new XMLHttpRequest(); client.open(\'POST\', \'/post\'); client.setRequestHeader(\'Content-Type\', \'text/plain\'); client.send(accessToken);</script>";
};
