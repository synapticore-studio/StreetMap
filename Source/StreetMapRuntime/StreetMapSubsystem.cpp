// Copyright Epic Games, Inc. All Rights Reserved.

#include "StreetMapSubsystem.h"
#include "StreetMap.h"
#include "StreetMapComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

UStreetMapSubsystem* UStreetMapSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->GetSubsystem<UStreetMapSubsystem>();
		}
	}
	return nullptr;
}

void UStreetMapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Initialize any required state
	RegisteredStreetMaps.Empty();
	RegisteredComponents.Empty();
}

void UStreetMapSubsystem::Deinitialize()
{
	// Clean up
	RegisteredStreetMaps.Empty();
	RegisteredComponents.Empty();
	
	Super::Deinitialize();
}

bool UStreetMapSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Create subsystem for all worlds except preview worlds
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		return World->WorldType != EWorldType::EditorPreview;
	}
	return false;
}

bool UStreetMapSubsystem::RegisterStreetMap(UStreetMap* InStreetMap)
{
	if (!InStreetMap)
	{
		return false;
	}

	if (RegisteredStreetMaps.Contains(InStreetMap))
	{
		return false; // Already registered
	}

	RegisteredStreetMaps.Add(InStreetMap);
	OnStreetMapRegistered.Broadcast(InStreetMap);
	
	return true;
}

void UStreetMapSubsystem::UnregisterStreetMap(UStreetMap* InStreetMap)
{
	if (!InStreetMap)
	{
		return;
	}

	if (RegisteredStreetMaps.Remove(InStreetMap) > 0)
	{
		OnStreetMapUnregistered.Broadcast(InStreetMap);
	}
}

TArray<UStreetMap*> UStreetMapSubsystem::GetRegisteredStreetMaps() const
{
	TArray<UStreetMap*> Result;
	for (const TObjectPtr<UStreetMap>& Map : RegisteredStreetMaps)
	{
		if (Map)
		{
			Result.Add(Map);
		}
	}
	return Result;
}

UStreetMap* UStreetMapSubsystem::GetPrimaryStreetMap() const
{
	for (const TObjectPtr<UStreetMap>& Map : RegisteredStreetMaps)
	{
		if (Map)
		{
			return Map;
		}
	}
	return nullptr;
}

void UStreetMapSubsystem::RegisterComponent(UStreetMapComponent* InComponent)
{
	if (!InComponent)
	{
		return;
	}

	// Check if already registered
	for (const TWeakObjectPtr<UStreetMapComponent>& WeakComp : RegisteredComponents)
	{
		if (WeakComp.Get() == InComponent)
		{
			return;
		}
	}

	RegisteredComponents.Add(InComponent);

	// Also register the component's street map if it has one
	if (UStreetMap* Map = InComponent->GetStreetMap())
	{
		RegisterStreetMap(Map);
	}
}

void UStreetMapSubsystem::UnregisterComponent(UStreetMapComponent* InComponent)
{
	if (!InComponent)
	{
		return;
	}

	RegisteredComponents.RemoveAll([InComponent](const TWeakObjectPtr<UStreetMapComponent>& WeakComp)
	{
		return !WeakComp.IsValid() || WeakComp.Get() == InComponent;
	});
}

TArray<UStreetMapComponent*> UStreetMapSubsystem::GetRegisteredComponents() const
{
	TArray<UStreetMapComponent*> Result;
	for (const TWeakObjectPtr<UStreetMapComponent>& WeakComp : RegisteredComponents)
	{
		if (UStreetMapComponent* Comp = WeakComp.Get())
		{
			Result.Add(Comp);
		}
	}
	return Result;
}

bool UStreetMapSubsystem::FindNearestRoadPoint(const FVector& WorldLocation, int32& OutRoadIndex, int32& OutPointIndex, float MaxSearchDistance) const
{
	OutRoadIndex = INDEX_NONE;
	OutPointIndex = INDEX_NONE;

	UStreetMap* StreetMap = GetPrimaryStreetMap();
	if (!StreetMap)
	{
		return false;
	}

	const TArray<FStreetMapRoad>& Roads = StreetMap->GetRoads();
	// Use a reasonable maximum search distance (1km squared) if none specified to avoid performance issues
	constexpr float DefaultMaxSearchDistanceSquared = 100000.0f * 100000.0f;  // 1km in cm squared
	float BestDistanceSquared = MaxSearchDistance > 0.0f ? MaxSearchDistance * MaxSearchDistance : DefaultMaxSearchDistanceSquared;
	FVector2D Location2D(WorldLocation.X, WorldLocation.Y);

	for (int32 RoadIndex = 0; RoadIndex < Roads.Num(); ++RoadIndex)
	{
		const FStreetMapRoad& Road = Roads[RoadIndex];
		
		for (int32 PointIndex = 0; PointIndex < Road.RoadPoints.Num(); ++PointIndex)
		{
			const float DistSquared = FVector2D::DistSquared(Location2D, Road.RoadPoints[PointIndex]);
			if (DistSquared < BestDistanceSquared)
			{
				BestDistanceSquared = DistSquared;
				OutRoadIndex = RoadIndex;
				OutPointIndex = PointIndex;
			}
		}
	}

	return OutRoadIndex != INDEX_NONE;
}

TArray<int32> UStreetMapSubsystem::FindBuildingsInRadius(const FVector& WorldLocation, float Radius) const
{
	TArray<int32> Result;

	UStreetMap* StreetMap = GetPrimaryStreetMap();
	if (!StreetMap)
	{
		return Result;
	}

	const TArray<FStreetMapBuilding>& Buildings = StreetMap->GetBuildings();
	const float RadiusSquared = Radius * Radius;
	const FVector2D Location2D(WorldLocation.X, WorldLocation.Y);

	for (int32 BuildingIndex = 0; BuildingIndex < Buildings.Num(); ++BuildingIndex)
	{
		const FStreetMapBuilding& Building = Buildings[BuildingIndex];
		
		// Use building center for distance check
		const FVector2D BuildingCenter = (Building.BoundsMin + Building.BoundsMax) * 0.5f;
		
		if (FVector2D::DistSquared(Location2D, BuildingCenter) <= RadiusSquared)
		{
			Result.Add(BuildingIndex);
		}
	}

	return Result;
}
