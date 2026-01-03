// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PCGData.h"
#include "StreetMap.h"
#include "StreetMapPCGData.generated.h"

/**
 * PCG data representing street map roads as point data
 * Each road point becomes a PCG point with associated metadata
 */
UCLASS(BlueprintType, ClassGroup = (Procedural))
class STREETMAPRUNTIME_API UStreetMapRoadsPCGData : public UPCGSpatialData
{
	GENERATED_BODY()

public:
	/** Initialize this data from a street map asset */
	void Initialize(const UStreetMap* InStreetMap);

	// UPCGSpatialData interface
	virtual int GetDimension() const override { return 2; }
	virtual FBox GetBounds() const override;
	virtual FBox GetStrictBounds() const override { return GetBounds(); }
	virtual bool SamplePoint(const FTransform& Transform, const FBox& Bounds, FPCGPoint& OutPoint, UPCGMetadata* OutMetadata) const override;

	/** Returns the underlying point data for this spatial data */
	virtual const UPCGPointData* ToPointData(FPCGContext* Context, const FBox& InBounds = FBox(EForceInit::ForceInit)) const override;

protected:
	/** Create point data from the street map roads */
	UPCGPointData* CreatePointDataFromRoads(FPCGContext* Context, const FBox& InBounds) const;

	/** The source street map */
	UPROPERTY()
	TObjectPtr<const UStreetMap> StreetMap;

	/** Cached bounds */
	UPROPERTY()
	FBox CachedBounds;
};

/**
 * PCG data representing street map buildings as point data
 * Each building becomes a PCG point at its centroid with associated metadata
 */
UCLASS(BlueprintType, ClassGroup = (Procedural))
class STREETMAPRUNTIME_API UStreetMapBuildingsPCGData : public UPCGSpatialData
{
	GENERATED_BODY()

public:
	/** Initialize this data from a street map asset */
	void Initialize(const UStreetMap* InStreetMap);

	// UPCGSpatialData interface
	virtual int GetDimension() const override { return 2; }
	virtual FBox GetBounds() const override;
	virtual FBox GetStrictBounds() const override { return GetBounds(); }
	virtual bool SamplePoint(const FTransform& Transform, const FBox& Bounds, FPCGPoint& OutPoint, UPCGMetadata* OutMetadata) const override;

	/** Returns the underlying point data for this spatial data */
	virtual const UPCGPointData* ToPointData(FPCGContext* Context, const FBox& InBounds = FBox(EForceInit::ForceInit)) const override;

protected:
	/** Create point data from the street map buildings */
	UPCGPointData* CreatePointDataFromBuildings(FPCGContext* Context, const FBox& InBounds) const;

	/** The source street map */
	UPROPERTY()
	TObjectPtr<const UStreetMap> StreetMap;

	/** Cached bounds */
	UPROPERTY()
	FBox CachedBounds;
};
