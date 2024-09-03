#include "ItchLibrary.h"

FString UItchLibrary::GetEnvironmentVariable(const FString& EnvVariableName)
{
	return FPlatformMisc::GetEnvironmentVariable(*EnvVariableName);
}