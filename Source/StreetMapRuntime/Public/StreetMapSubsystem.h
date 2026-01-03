// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "StreetMapSubsystem.generated.h"

class UStreetMap;
class UStreetMapComponent;

/**
 * Subsystem for managing street map data within a world.
 * Follows Lyra/City Sample architecture patterns for better integration.
 */
UCLASS()
class STREETMAPRUNTIME_API UStreetMapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Get the street map subsystem from the world */
	static UStreetMapSubsystem* Get(const UObject* WorldContextObject);

	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/**
	 * Register a street map asset for use in this world
	 * @param InStreetMap The street map asset to register
	 * @return True if successfully registered
	 */
	UFUNCTION(BlueprintCallable, Category = "StreetMap")
	bool RegisterStreetMap(UStreetMap* InStreetMap);

	/**
	 * Unregister a street map asset
	 * @param InStreetMap The street map asset to unregister
	 */
	UFUNCTION(BlueprintCallable, Category = "StreetMap")
	void UnregisterStreetMap(UStreetMap* InStreetMap);

	/**
	 * Get all registered street maps
	 * @return Array of registered street map assets
	 */
	UFUNCTION(BlueprintCallable, Category = "StreetMap")
	TArray<UStreetMap*> GetRegisteredStreetMaps() const;

	/**
	 * Get the primary/main street map for this world
	 * Returns the first registered street map
	 */
	UFUNCTION(BlueprintCallable, Category = "StreetMap")
	UStreetMap* GetPrimaryStreetMap() const;

	/**
	 * Register a street map component for tracking
	 * @param InComponent The component to register
	 */
	void RegisterComponent(UStreetMapComponent* InComponent);

	/**
	 * Unregister a street map component
	 * @param InComponent The component to unregister
	 */
	void UnregisterComponent(UStreetMapComponent* InComponent);

	/**
	 * Get all registered street map components
	 * @return Array of registered components
	 */
	UFUNCTION(BlueprintCallable, Category = "StreetMap")
	TArray<UStreetMapComponent*> GetRegisteredComponents() const;

	/**
	 * Find the nearest road point to a world location
	 * @param WorldLocation The location to search from
	 * @param OutRoadIndex The index of the found road (or INDEX_NONE)
	 * @param OutPointIndex The index of the point on the road (or INDEX_NONE)
	 * @param MaxSearchDistance Maximum distance to search (0 = unlimited)
	 * @return True if a road point was found
	 */
	UFUNCTION(BlueprintCallable, Category = "StreetMap")
	bool FindNearestRoadPoint(const FVector& WorldLocation, int32& OutRoadIndex, int32& OutPointIndex, float MaxSearchDistance = 0.0f) const;

	/**
	 * Find all buildings within a radius
	 * @param WorldLocation The center location
	 * @param Radius The search radius
	 * @return Array of building indices that are within the radius
	 */
	UFUNCTION(BlueprintCallable, Category = "StreetMap")
	TArray<int32> FindBuildingsInRadius(const FVector& WorldLocation, float Radius) const;

	/** Delegate broadcast when a street map is registered */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStreetMapRegistered, UStreetMap*, StreetMap);
	
	UPROPERTY(BlueprintAssignable, Category = "StreetMap")
	FOnStreetMapRegistered OnStreetMapRegistered;

	/** Delegate broadcast when a street map is unregistered */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStreetMapUnregistered, UStreetMap*, StreetMap);
	
	UPROPERTY(BlueprintAssignable, Category = "StreetMap")
	FOnStreetMapUnregistered OnStreetMapUnregistered;

protected:
	/** Registered street map assets */
	UPROPERTY()
	TArray<TObjectPtr<UStreetMap>> RegisteredStreetMaps;

	/** Registered street map components */
	UPROPERTY()
	TArray<TWeakObjectPtr<UStreetMapComponent>> RegisteredComponents;
};
