#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * StreetMap Runtime Module
 * Provides runtime functionality for OpenStreetMap data import and rendering
 * with PCG (Procedural Content Generation) Graph integration for UE 5.7+
 */
class FStreetMapRuntimeModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Get the module instance */
	static FStreetMapRuntimeModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FStreetMapRuntimeModule>("StreetMapRuntime");
	}

	/** Check if the module is loaded */
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("StreetMapRuntime");
	}
};
