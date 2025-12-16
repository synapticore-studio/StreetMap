// Copyright Epic Games, Inc. All Rights Reserved.

#include "StreetMapPCGSettings.h"
#include "StreetMap.h"
#include "StreetMapPCGData.h"
#include "PCGContext.h"
#include "Data/PCGPointData.h"
#include "Metadata/PCGMetadata.h"
#include "Metadata/PCGMetadataAttribute.h"
#include "Metadata/PCGMetadataAttributeTpl.h"
#include "Engine/AssetManager.h"

UPCGStreetMapSettings::UPCGStreetMapSettings()
{
	AllowedRoadTypes.Add(EStreetMapRoadType::Highway);
	AllowedRoadTypes.Add(EStreetMapRoadType::MajorRoad);
	AllowedRoadTypes.Add(EStreetMapRoadType::Street);
}

TArray<FPCGPinProperties> UPCGStreetMapSettings::InputPinProperties() const
{
	// No inputs - we read directly from the street map asset
	return TArray<FPCGPinProperties>();
}

TArray<FPCGPinProperties> UPCGStreetMapSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> PinProperties;

	if (bOutputRoads)
	{
		FPCGPinProperties& RoadsPin = PinProperties.Emplace_GetRef(TEXT("Roads"), EPCGDataType::Point);
#if WITH_EDITOR
		RoadsPin.Tooltip = NSLOCTEXT("PCGStreetMapSettings", "RoadsPin", "Road points with metadata (RoadName, RoadType, RoadIndex, PointIndex, IsOneWay)");
#endif
	}

	if (bOutputBuildings)
	{
		FPCGPinProperties& BuildingsPin = PinProperties.Emplace_GetRef(TEXT("Buildings"), EPCGDataType::Point);
#if WITH_EDITOR
		BuildingsPin.Tooltip = NSLOCTEXT("PCGStreetMapSettings", "BuildingsPin", "Building centroids with metadata (BuildingName, Height, BuildingLevels, BuildingIndex, VertexCount)");
#endif
	}

	return PinProperties;
}

FPCGElementPtr UPCGStreetMapSettings::CreateElement() const
{
	return MakeShared<FPCGStreetMapElement>();
}

bool FPCGStreetMapElement::ExecuteInternal(FPCGContext* Context) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGStreetMapElement::Execute);

	const UPCGStreetMapSettings* Settings = Context->GetInputSettings<UPCGStreetMapSettings>();
	check(Settings);

	// Load the street map asset
	UStreetMap* LoadedStreetMap = Settings->StreetMap.LoadSynchronous();
	if (!LoadedStreetMap)
	{
		PCGE_LOG(Error, GraphAndLog, FText::Format(
			NSLOCTEXT("PCGStreetMapElement", "NoStreetMap", "No street map asset specified or failed to load: {0}"),
			FText::FromString(Settings->StreetMap.ToString())
		));
		return true;
	}

	TArray<FPCGTaggedData>& Outputs = Context->OutputData.TaggedData;

	// Output roads
	if (Settings->bOutputRoads)
	{
		UPCGPointData* RoadPointData = NewObject<UPCGPointData>();
		TArray<FPCGPoint>& Points = RoadPointData->GetMutablePoints();
		UPCGMetadata* Metadata = RoadPointData->MutableMetadata();

		// Create metadata attributes
		FPCGMetadataAttribute<FString>* RoadNameAttr = Metadata->CreateAttribute<FString>(TEXT("RoadName"), FString(), true, false);
		FPCGMetadataAttribute<int32>* RoadTypeAttr = Metadata->CreateAttribute<int32>(TEXT("RoadType"), 0, true, false);
		FPCGMetadataAttribute<int32>* RoadIndexAttr = Metadata->CreateAttribute<int32>(TEXT("RoadIndex"), 0, true, false);
		FPCGMetadataAttribute<int32>* PointIndexAttr = Metadata->CreateAttribute<int32>(TEXT("PointIndex"), 0, true, false);
		FPCGMetadataAttribute<bool>* IsOneWayAttr = Metadata->CreateAttribute<bool>(TEXT("IsOneWay"), false, true, false);

		const TArray<FStreetMapRoad>& Roads = LoadedStreetMap->GetRoads();

		for (int32 RoadIndex = 0; RoadIndex < Roads.Num(); ++RoadIndex)
		{
			const FStreetMapRoad& Road = Roads[RoadIndex];

			// Apply road type filter
			if (Settings->bFilterRoadsByType && !Settings->AllowedRoadTypes.Contains(Road.RoadType))
			{
				continue;
			}

			for (int32 PointIndex = 0; PointIndex < Road.RoadPoints.Num(); ++PointIndex)
			{
				const FVector2D& RoadPoint = Road.RoadPoints[PointIndex];

				FPCGPoint& NewPoint = Points.Emplace_GetRef();
				NewPoint.Transform = FTransform(FVector(RoadPoint.X, RoadPoint.Y, 0.0f));
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

		FPCGTaggedData& RoadOutput = Outputs.Emplace_GetRef();
		RoadOutput.Data = RoadPointData;
		RoadOutput.Pin = TEXT("Roads");
	}

	// Output buildings
	if (Settings->bOutputBuildings)
	{
		UPCGPointData* BuildingPointData = NewObject<UPCGPointData>();
		TArray<FPCGPoint>& Points = BuildingPointData->GetMutablePoints();
		UPCGMetadata* Metadata = BuildingPointData->MutableMetadata();

		// Create metadata attributes
		FPCGMetadataAttribute<FString>* BuildingNameAttr = Metadata->CreateAttribute<FString>(TEXT("BuildingName"), FString(), true, false);
		FPCGMetadataAttribute<double>* HeightAttr = Metadata->CreateAttribute<double>(TEXT("Height"), 0.0, true, false);
		FPCGMetadataAttribute<int32>* LevelsAttr = Metadata->CreateAttribute<int32>(TEXT("BuildingLevels"), 0, true, false);
		FPCGMetadataAttribute<int32>* BuildingIndexAttr = Metadata->CreateAttribute<int32>(TEXT("BuildingIndex"), 0, true, false);
		FPCGMetadataAttribute<int32>* VertexCountAttr = Metadata->CreateAttribute<int32>(TEXT("VertexCount"), 0, true, false);

		const TArray<FStreetMapBuilding>& Buildings = LoadedStreetMap->GetBuildings();

		for (int32 BuildingIndex = 0; BuildingIndex < Buildings.Num(); ++BuildingIndex)
		{
			const FStreetMapBuilding& Building = Buildings[BuildingIndex];

			// Apply height filter
			if (Settings->MinBuildingHeight > 0.0f && Building.Height < Settings->MinBuildingHeight)
			{
				continue;
			}

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

			// Calculate building extents from bounds
			FVector2D Extents2D = (Building.BoundsMax - Building.BoundsMin) * 0.5f;
			float Height = Building.Height > KINDA_SMALL_NUMBER ? Building.Height : 300.0f;

			FPCGPoint& NewPoint = Points.Emplace_GetRef();
			NewPoint.Transform = FTransform(FVector(Centroid.X, Centroid.Y, Height * 0.5f));
			NewPoint.SetExtents(FVector(FMath::Max(Extents2D.X, 50.0f), FMath::Max(Extents2D.Y, 50.0f), Height * 0.5f));
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

		FPCGTaggedData& BuildingOutput = Outputs.Emplace_GetRef();
		BuildingOutput.Data = BuildingPointData;
		BuildingOutput.Pin = TEXT("Buildings");
	}

	return true;
}
