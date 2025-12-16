// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PCGSettings.h"
#include "StreetMap.h"
#include "StreetMapPCGSettings.generated.h"

/**
 * Settings for getting street map data in PCG graphs
 * This node outputs roads and/or buildings as PCG point data
 */
UCLASS(BlueprintType, ClassGroup = (Procedural))
class STREETMAPRUNTIME_API UPCGStreetMapSettings : public UPCGSettings
{
	GENERATED_BODY()

public:
	UPCGStreetMapSettings();

	// UPCGSettings interface
#if WITH_EDITOR
	virtual FName GetDefaultNodeName() const override { return FName(TEXT("StreetMapData")); }
	virtual FText GetDefaultNodeTitle() const override { return NSLOCTEXT("PCGStreetMapSettings", "NodeTitle", "Street Map Data"); }
	virtual FText GetNodeTooltipText() const override { return NSLOCTEXT("PCGStreetMapSettings", "NodeTooltip", "Outputs street map roads and buildings as PCG point data"); }
	virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::Spatial; }
#endif

	virtual TArray<FPCGPinProperties> InputPinProperties() const override;
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;

protected:
	virtual FPCGElementPtr CreateElement() const override;

public:
	/** The street map asset to use for data */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TSoftObjectPtr<UStreetMap> StreetMap;

	/** Output road points */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bOutputRoads = true;

	/** Output building centroids */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	bool bOutputBuildings = true;

	/** Filter roads by type */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Filtering")
	bool bFilterRoadsByType = false;

	/** Road types to include when filtering */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Filtering", meta = (EditCondition = "bFilterRoadsByType"))
	TArray<TEnumAsByte<EStreetMapRoadType>> AllowedRoadTypes;

	/** Minimum building height to include (0 = include all) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Filtering", meta = (ClampMin = "0"))
	float MinBuildingHeight = 0.0f;
};

/**
 * PCG element implementation for street map data extraction
 */
class FPCGStreetMapElement : public IPCGElement
{
protected:
	virtual bool ExecuteInternal(FPCGContext* Context) const override;
};
