// Copyright Epic Games, Inc. All Rights Reserved.

#include "StreetMapPCGData.h"
#include "Data/PCGPointData.h"
#include "Metadata/PCGMetadata.h"
#include "Metadata/PCGMetadataAttribute.h"
#include "Metadata/PCGMetadataAttributeTpl.h"

//////////////////////////////////////////////////////////////////////////
// UStreetMapRoadsPCGData

void UStreetMapRoadsPCGData::Initialize(const UStreetMap* InStreetMap)
{
	StreetMap = InStreetMap;
	
	if (StreetMap)
	{
		// Calculate bounds from street map
		const FVector2D& BoundsMin = StreetMap->GetBoundsMin();
		const FVector2D& BoundsMax = StreetMap->GetBoundsMax();
		CachedBounds = FBox(
			FVector(BoundsMin.X, BoundsMin.Y, 0.0f),
			FVector(BoundsMax.X, BoundsMax.Y, 0.0f)
		);
	}
	else
	{
		CachedBounds = FBox(EForceInit::ForceInit);
	}
}

FBox UStreetMapRoadsPCGData::GetBounds() const
{
	return CachedBounds;
}

bool UStreetMapRoadsPCGData::SamplePoint(const FTransform& Transform, const FBox& Bounds, FPCGPoint& OutPoint, UPCGMetadata* OutMetadata) const
{
	// Basic sampling implementation
	if (!CachedBounds.IsValid || !Bounds.Intersect(CachedBounds))
	{
		return false;
	}

	OutPoint.Transform = Transform;
	OutPoint.SetExtents(FVector(50.0f, 50.0f, 50.0f));
	OutPoint.Density = 1.0f;
	
	return true;
}

const UPCGPointData* UStreetMapRoadsPCGData::ToPointData(FPCGContext* Context, const FBox& InBounds) const
{
	return CreatePointDataFromRoads(Context, InBounds);
}

UPCGPointData* UStreetMapRoadsPCGData::CreatePointDataFromRoads(FPCGContext* Context, const FBox& InBounds) const
{
	if (!StreetMap)
	{
		return nullptr;
	}

	UPCGPointData* PointData = NewObject<UPCGPointData>();
	TArray<FPCGPoint>& Points = PointData->GetMutablePoints();
	UPCGMetadata* Metadata = PointData->MutableMetadata();

	// Create metadata attributes for road properties
	FPCGMetadataAttribute<FString>* RoadNameAttr = Metadata->CreateAttribute<FString>(TEXT("RoadName"), FString(), true, false);
	FPCGMetadataAttribute<int32>* RoadTypeAttr = Metadata->CreateAttribute<int32>(TEXT("RoadType"), 0, true, false);
	FPCGMetadataAttribute<int32>* RoadIndexAttr = Metadata->CreateAttribute<int32>(TEXT("RoadIndex"), 0, true, false);
	FPCGMetadataAttribute<int32>* PointIndexAttr = Metadata->CreateAttribute<int32>(TEXT("PointIndex"), 0, true, false);
	FPCGMetadataAttribute<bool>* IsOneWayAttr = Metadata->CreateAttribute<bool>(TEXT("IsOneWay"), false, true, false);

	const TArray<FStreetMapRoad>& Roads = StreetMap->GetRoads();
	
	for (int32 RoadIndex = 0; RoadIndex < Roads.Num(); ++RoadIndex)
	{
		const FStreetMapRoad& Road = Roads[RoadIndex];
		
		for (int32 PointIndex = 0; PointIndex < Road.RoadPoints.Num(); ++PointIndex)
		{
			const FVector2D& RoadPoint = Road.RoadPoints[PointIndex];
			FVector Location(RoadPoint.X, RoadPoint.Y, 0.0f);
			
			// Check bounds filter
			if (InBounds.IsValid && !InBounds.IsInsideOrOn(Location))
			{
				continue;
			}
			
			FPCGPoint& NewPoint = Points.Emplace_GetRef();
			NewPoint.Transform = FTransform(Location);
			NewPoint.SetExtents(FVector(50.0f, 50.0f, 50.0f));
			NewPoint.Density = 1.0f;
			// Deterministic seed based on indices for reproducible PCG results
			NewPoint.Seed = GetTypeHash(FVector2f(RoadPoint)) ^ (RoadIndex * 1000 + PointIndex);

			// Set metadata
			PCGMetadataEntryKey MetadataKey = Metadata->AddEntry();
			NewPoint.MetadataEntry = MetadataKey;

			if (RoadNameAttr)
			{
				RoadNameAttr->SetValue(MetadataKey, Road.RoadName);
			}
			if (RoadTypeAttr)
			{
				RoadTypeAttr->SetValue(MetadataKey, static_cast<int32>(Road.RoadType.GetValue()));
			}
			if (RoadIndexAttr)
			{
				RoadIndexAttr->SetValue(MetadataKey, RoadIndex);
			}
			if (PointIndexAttr)
			{
				PointIndexAttr->SetValue(MetadataKey, PointIndex);
			}
			if (IsOneWayAttr)
			{
				IsOneWayAttr->SetValue(MetadataKey, Road.IsOneWay());
			}
		}
	}

	return PointData;
}

//////////////////////////////////////////////////////////////////////////
// UStreetMapBuildingsPCGData

void UStreetMapBuildingsPCGData::Initialize(const UStreetMap* InStreetMap)
{
	StreetMap = InStreetMap;
	
	if (StreetMap)
	{
		// Calculate bounds from street map
		const FVector2D& BoundsMin = StreetMap->GetBoundsMin();
		const FVector2D& BoundsMax = StreetMap->GetBoundsMax();
		CachedBounds = FBox(
			FVector(BoundsMin.X, BoundsMin.Y, 0.0f),
			FVector(BoundsMax.X, BoundsMax.Y, 0.0f)
		);
	}
	else
	{
		CachedBounds = FBox(EForceInit::ForceInit);
	}
}

FBox UStreetMapBuildingsPCGData::GetBounds() const
{
	return CachedBounds;
}

bool UStreetMapBuildingsPCGData::SamplePoint(const FTransform& Transform, const FBox& Bounds, FPCGPoint& OutPoint, UPCGMetadata* OutMetadata) const
{
	// Basic sampling implementation
	if (!CachedBounds.IsValid || !Bounds.Intersect(CachedBounds))
	{
		return false;
	}

	OutPoint.Transform = Transform;
	OutPoint.SetExtents(FVector(100.0f, 100.0f, 100.0f));
	OutPoint.Density = 1.0f;
	
	return true;
}

const UPCGPointData* UStreetMapBuildingsPCGData::ToPointData(FPCGContext* Context, const FBox& InBounds) const
{
	return CreatePointDataFromBuildings(Context, InBounds);
}

UPCGPointData* UStreetMapBuildingsPCGData::CreatePointDataFromBuildings(FPCGContext* Context, const FBox& InBounds) const
{
	if (!StreetMap)
	{
		return nullptr;
	}

	UPCGPointData* PointData = NewObject<UPCGPointData>();
	TArray<FPCGPoint>& Points = PointData->GetMutablePoints();
	UPCGMetadata* Metadata = PointData->MutableMetadata();

	// Create metadata attributes for building properties
	FPCGMetadataAttribute<FString>* BuildingNameAttr = Metadata->CreateAttribute<FString>(TEXT("BuildingName"), FString(), true, false);
	FPCGMetadataAttribute<double>* HeightAttr = Metadata->CreateAttribute<double>(TEXT("Height"), 0.0, true, false);
	FPCGMetadataAttribute<int32>* LevelsAttr = Metadata->CreateAttribute<int32>(TEXT("BuildingLevels"), 0, true, false);
	FPCGMetadataAttribute<int32>* BuildingIndexAttr = Metadata->CreateAttribute<int32>(TEXT("BuildingIndex"), 0, true, false);
	FPCGMetadataAttribute<int32>* VertexCountAttr = Metadata->CreateAttribute<int32>(TEXT("VertexCount"), 0, true, false);

	const TArray<FStreetMapBuilding>& Buildings = StreetMap->GetBuildings();
	
	for (int32 BuildingIndex = 0; BuildingIndex < Buildings.Num(); ++BuildingIndex)
	{
		const FStreetMapBuilding& Building = Buildings[BuildingIndex];
		
		// Calculate centroid of building polygon
		FVector2D Centroid = FVector2D::ZeroVector;
		for (const FVector2D& Point : Building.BuildingPoints)
		{
			Centroid += Point;
		}
		if (Building.BuildingPoints.Num() > 0)
		{
			Centroid /= Building.BuildingPoints.Num();
		}
		
		FVector Location(Centroid.X, Centroid.Y, 0.0f);
		
		// Check bounds filter
		if (InBounds.IsValid && !InBounds.IsInsideOrOn(Location))
		{
			continue;
		}
		
		// Calculate building extents from bounds
		FVector2D Extents2D = (Building.BoundsMax - Building.BoundsMin) * 0.5f;
		float Height = Building.Height > KINDA_SMALL_NUMBER ? Building.Height : 300.0f;  // Default height
		
		FPCGPoint& NewPoint = Points.Emplace_GetRef();
		NewPoint.Transform = FTransform(FVector(Centroid.X, Centroid.Y, Height * 0.5f));
		NewPoint.SetExtents(FVector(Extents2D.X, Extents2D.Y, Height * 0.5f));
		NewPoint.Density = 1.0f;
		// Deterministic seed based on building index and centroid for reproducible PCG results
		NewPoint.Seed = GetTypeHash(FVector2f(Centroid)) ^ BuildingIndex;

		// Set metadata
		PCGMetadataEntryKey MetadataKey = Metadata->AddEntry();
		NewPoint.MetadataEntry = MetadataKey;

		if (BuildingNameAttr)
		{
			BuildingNameAttr->SetValue(MetadataKey, Building.BuildingName);
		}
		if (HeightAttr)
		{
			HeightAttr->SetValue(MetadataKey, Building.Height);
		}
		if (LevelsAttr)
		{
			LevelsAttr->SetValue(MetadataKey, Building.BuildingLevels);
		}
		if (BuildingIndexAttr)
		{
			BuildingIndexAttr->SetValue(MetadataKey, BuildingIndex);
		}
		if (VertexCountAttr)
		{
			VertexCountAttr->SetValue(MetadataKey, Building.BuildingPoints.Num());
		}
	}

	return PointData;
}
