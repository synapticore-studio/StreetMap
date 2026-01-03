#include "StreetMapRuntime.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FStreetMapRuntimeModule, StreetMapRuntime)

void FStreetMapRuntimeModule::StartupModule()
{
	// Module startup logic
	// PCG integration is automatically registered through the UPCGStreetMapSettings UCLASS
}

void FStreetMapRuntimeModule::ShutdownModule()
{
	// Module shutdown logic
}
