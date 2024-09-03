#pragma once

#include "ItchLibrary.generated.h"

UCLASS()
class ITCHINTEGRATION_API UItchLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	static FString GetEnvironmentVariable(const FString& EnvVariableName);
};
