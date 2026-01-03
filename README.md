# Street Map Plugin for Unreal Engine 5.7

This plugin allows you to import **OpenStreetMap** XML data into your **Unreal Engine** project as a new StreetMap asset type. You can use the example **Street Map Component** to render streets and buildings, and the new **PCG Graph integration** to procedurally generate content based on map data.

![UE4OSMBrooklyn](Docs/UE4OSMBrooklyn.png)

![UE4OSMRaleigh](Docs/UE4OSMRaleigh.png)

Have fun!!  --[Mike](http://twitter.com/mike_fricker)
[LICENSE.txt](LICENSE.txt)
*(Note: This plugin is a just a fun weekend project and not officially supported by Epic.)*


## Unreal Engine 5.7 Features

This version has been updated for **Unreal Engine 5.7** with the following enhancements:

### PCG (Procedural Content Generation) Graph Integration

The plugin now includes full PCG Graph support, allowing you to use street map data as input for procedural content generation:

- **Street Map Data Node**: A new PCG node that outputs road and building data as PCG point data with rich metadata
- **Roads Output**: Get road points with metadata including RoadName, RoadType, RoadIndex, PointIndex, and IsOneWay
- **Buildings Output**: Get building centroids with metadata including BuildingName, Height, BuildingLevels, BuildingIndex, and VertexCount
- **Filtering Options**: Filter roads by type (Highway, MajorRoad, Street) and buildings by minimum height

### Street Map Subsystem

Following Lyra/City Sample architecture patterns, the plugin now includes a World Subsystem:

- **UStreetMapSubsystem**: Manages street map data within a world
- Register/unregister street maps and components
- Query functions like `FindNearestRoadPoint` and `FindBuildingsInRadius`
- Blueprint-accessible delegates for map registration events

### API Updates

- Updated to modern UE 5.7 module syntax
- Removed deprecated EditorStyle dependency
- Updated include paths for newer Engine API
- PCHUsage set to UseExplicitOrSharedPCHs for faster compilation


## Quick Start

It's easy to get up and running:

* Download the StreetMap plugin source from this page (click **Clone or download** -> **Download ZIP**).

* Unzip the files into a new **StreetMap** sub-folder under your project's **Plugins** folder.  It should end up looking like *"/MyProject/Plugins/StreetMap/<files>"*

* **Rebuild** your C++ project.  The new plugin will be compiled too!

* Load the editor.  You can now drag and drop **OpenStreetMap XML files** (.osm) into Content Browser to import map data!

* Drag and Drop imported **Street Map Data Asset** into the viewport and a **Street Map Actor** will be automatically generated. You should now see your streets and buildings in the 3D viewport.

![UE4OSMManhattan](Docs/UE4OSMActor.png)


If the rebuild was successful but you don't see the new features, double check that the **Street Map** plugin is enabled by clicking the **Settings** toolbar button, then click **Plugins**.  Locate the **Street Map** plugin and make sure **Enabled** is checked.

If you're new to plugins in UE, you can find lots of information [right here](https://wiki.unrealengine.com/An_Introduction_to_UE4_Plugins).


## Getting OpenStreetMap Data

**Legal:**  OpenStreetMap data is licensed under the [ODC Open Database License (ODbL)](http://opendatacommons.org/licenses/odbl/).  If you use this data in your project, *make sure you understand and comply with the terms of that license* e.g. lookup the [Legal FAQ](https://wiki.openstreetmap.org/wiki/Legal_FAQ).

![UE4OSMExport](Docs/UE4OSMExport.png)

Here's how to get data for a location you're interested in:

**For larger areas (more than a neighborhood or small town) you should use [Mapzen Extracts](https://mapzen.com/data/metro-extracts).**

* Go to [OpenStreetMap.org](http://www.openstreetmap.org) and use the search feature to navigate to your *favorite location on Earth*.

* Click the **Export** button on navigation bar at the top of the page to go into *Export Mode*.

* Scroll and zoom such that the region you want to export fills your browser window.  Start with something reasonably small so that the export and download will complete quickly.  Try zooming in on a small town or a city block.

* When you're ready, click **Export** on the left.  OpenStreetMap will **generate an XML file** and initiate the download soon.  

If you want to fine tune the rectangle that's saved, you can click "Manually select a different area" in the OpenStreetMap window, and adjust a rectangle over the map that will be exported.

Keep in mind that many locations may have limited information about building geometry.  In particular, the heights of buildings may be missing or incorrect in many cities.

If you receive an error message after clicking **Export**, OpenStreetMap may be too busy to accomodate the request.  Try clicking **Overpass API** or check one of the other sources.  Make sure the downloaded file has the extension ".osm", as this is what the plugin will be expecting.  You can rename the downloaded file as needed.

Of course, there are many other places you can find raw OpenStreetMap XML data on the web also, but keep in mind the plugin has only been tested with files exported directly from OpenStreetMap so far.

## Editing OpenStreetMap

**Attention:** OSM covers the real world and includes only fact based knowledge. If you like to build up an fictional map, you can use the [JOSM offline editor](https://wiki.openstreetmap.org/wiki/JOSM), to create an local XML file, which you don't upload(!) to the project.

You can easily contribute back to OSM, for example to improve your hometown. Just signup at www.openstreetmap.org and click at the edit tab. The online iD editor allows you to trace aerial imagery and to add POIs easily. To learn more details, just look over here:
* http://learnosm.org
* https://wiki.openstreetmap.org/wiki/Video_tutorials

Please be aware, that the project community (the inhabitants!) is the essential part. Thus it's wise to [get in touch](https://wiki.openstreetmap.org/wiki/Contact_channels) with mappers close to you, to get more tips on local tagging, or unwritten rules. Happy mapping!

## Plugin Details

### Street Map Assets

When you **import an OSM** file, the plugin will create a new **Street Map asset** to represent the map data in UE.  You can assign these to **Street Map Components**, or directly interact with the map data in C++ code.

Roads are imported with *full connectivity data*!  This means you can design your own navigation algorithms pretty easily.

OpenStreetMap positional data is stored in *geographic coordinates* (latitude and longitude), but UE doesn't support that coordinate system natively.  That is, we can't easily deal with spherical worlds in UE currently.  So during the import process, we project all map coordinates to a flat 2D plane.

The OSM data is imported at double precision, but we truncate everything to single precision floating point before saving our UE street map asset.  If you're planning to work with enormous map data sets at runtime, you'll need to modify this.


### Street Map Components

An example implementation of a **Street Map Component** is included that generates a renderable mesh from loaded street and building data.  This is a very simple component that you can use as a starting point.

The example implementation creates a custom primitive component mesh instead of a traditional static mesh.  The reason for this was to allow for more flexible rendering behavior of city streets and buildings, or even dynamic aspects.

All mesh data is generated at load time from the cartographic data in the map asset, including colorized road strips and simple building meshes with triangulated roof polygons.  No spline interpolation is performed on the roads.

The generated street map mesh has vertex colors and normals, and you can assign a custom material to it.  If you want to use the built-in colors, make sure your material multiplies Vertex Color with Base Color.  The mesh is setup to render very efficiently in a single draw call.  Roads are represented as simple quad strips (no tesselation).  Texture coordinates are not supported yet.

There are various "tweakable" variables to control how the renderable mesh is generated.  You can find these at the top of the *UStreetMapComponent::GenerateMesh()* function body.

*(Street Map Component also serves as a straightforward example of how to write your own primitive components in UE.)*


### Using PCG Graphs with Street Map Data

The plugin provides a **Street Map Data** PCG node that can be used to procedurally generate content based on imported OpenStreetMap data.

**Basic Usage:**

1. Create a new PCG Graph asset
2. Add a "Street Map Data" node to your graph
3. Configure the node settings:
   - Assign your imported Street Map asset
   - Enable/disable Roads and/or Buildings output
   - Configure filtering options as needed
4. Connect the output pins to other PCG nodes for processing

**Available Output Pins:**

- **Roads**: Point data for each road point, with metadata:
  - `RoadName` (String): Name of the road
  - `RoadType` (Int32): 0=Street, 1=MajorRoad, 2=Highway, 3=Other
  - `RoadIndex` (Int32): Index of the road in the street map
  - `PointIndex` (Int32): Index of this point along the road
  - `IsOneWay` (Bool): Whether the road is one-way

- **Buildings**: Point data for each building centroid, with metadata:
  - `BuildingName` (String): Name of the building
  - `Height` (Double): Building height in centimeters
  - `BuildingLevels` (Int32): Number of floors
  - `BuildingIndex` (Int32): Index of the building in the street map
  - `VertexCount` (Int32): Number of polygon vertices

**Example Use Cases:**

- Place street lights along roads using road point data
- Spawn vegetation in areas between buildings
- Generate procedural signs based on road names
- Create LOD meshes for buildings based on building data


### Street Map Subsystem

The **UStreetMapSubsystem** provides a centralized way to access street map data in your world:

```cpp
// Get the subsystem
UStreetMapSubsystem* Subsystem = UStreetMapSubsystem::Get(WorldContextObject);

// Find the nearest road point
int32 RoadIndex, PointIndex;
if (Subsystem->FindNearestRoadPoint(Location, RoadIndex, PointIndex))
{
    // Use the road data
}

// Find buildings within a radius
TArray<int32> BuildingIndices = Subsystem->FindBuildingsInRadius(Location, 5000.0f);
```


### OSM Files

While importing OpenStreetMap XML files, we store all of the data that's interesting to us in an **FOSMFile** data structure in memory.  This contains data that is very close to raw representation in the XML file.  Coordinates are stored as geographic positions in double precision floating point.

After loading everything into **FOSMFile**, we digest the data and convert it to a format that can be serialized to disk and loaded efficiently at runtime (the **UStreetMap** class.)

Depending on your use case, you may want to heavily customize the **UStreetMap** class to store data that is more close to the raw representation of the map.  For example, if you wanted to perform large-scale GPS navigation, you'd want higher precision data available at runtime.


### Known Issues

There are various loose ends.

* Importing files larger than 2GB will crash.  This is a current UE limitation.

* Some variants of generated OSM XML files won't load correctly.  For example, single-quote delimeters around values are not supported yet.

* Street Map APIs should be easy to use from C++, but Blueprint support hasn't been a focus for this plugin.  Many methods are inlined for high performance.  Blueprint scripting hooks could be added if there is demand for it, though.

* As mentioned above, coordinates are truncated to single-precision which won't be sufficient for advanced use cases.  Similarly, geographic coordinates are not retained beyond the initial import phase.  All coordinates are projected onto a plane and transposed to be relative to the center of the map's bounding rectangle.

* Runtime data structures are setup to support pathfinding (see **FStreetMapNode** member functions), but no example implementation of a GPS algorithm is included yet.

* Generated mesh data is currently very simple and lacks collision information, navigation mesh support and has no texture coordinates.  This is really just designed to serve as an example.  For more rendering flexibility and faster performance, the importer could be changed to generate actual Static Mesh assets for map geometry.

* You can search for **@todo** in the plugin source code for other minor improvements that could be made.


### Compatibility

This plug-in requires Visual Studio and either a C++ code project or the full Unreal Engine source code from GitHub.  If you are new to programming in UE, please see the official [Programming Guide](https://docs.unrealengine.com/latest/INT/Programming/index.html)! 

The Street Map plugin should work on all platforms that UE supports, but the latest version has not been tested on every platform.

We'll try to keep the source code up to date so that it works with new versions Unreal Engine as they are released.


## Support

I'm not planning to actively update the plugin on a regular basis, but if any critical fixes are contributed, I'll certainly try to review and integrate them.
 
For bugs, please [file an issue](https://github.com/ue4plugins/StreetMap/issues), submit a [pull request](https://github.com/ue4plugins/StreetMap/pulls?q=is%3Aopen+is%3Apr) or catch me [on Twitter](http://twitter.com/mike_fricker).

Finally, a **big thanks** to the [OpenStreetMap Foundation](http://wiki.osmfoundation.org/wiki/Main_Page) and the fantastic community who contribute map data and maintain the database.

