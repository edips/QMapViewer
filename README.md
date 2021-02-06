# Surveying Calculator
A modern engineering app with surveying and GIS aromas

Surveying Calculator has commonly used land surveying tools. This includes coordinate geometry calculations, offline point data collection, displaying CAD, GIS, raster data and online maps in QGIS 3 project format. Surveying Calculator supports QGIS 3 projects. (qgs, qgz)
It is available on Play Store: https://play.google.com/store/apps/details?id=org.project.geoclass
Features:
- Basic Surveying Tools: X(N), Y(E) Calculation, Distance, Azimuth Calculation, Interior Angle, Latitude Longitude from point, distance and bearing
- Intersection Methods: Forward Intersection, Line - Line Intersection, Circle by 3 Points
- Distance between Coordinates: 2D, 3D distance from X, Y and distance from Latitude, Longitude
- Area calculation from coordinates
- Degree - Decimal Conversion
- Coordinate Transformation: WGS84 Latitude/Longitude to UTM XY or UTM XY to Latitude/Longitude Conventer, Coordinate Converter tool, 2D Helmert transformation
- Point Data Collector - Survey:
→ Create projects in GeoPackage formats with coordinate system code (EPSG code).
→ You can also prepare a job in QGIS on desktop with CAD-GIS data. Import CAD, GIS, raster data or add online map services in QGIS. And transfer the data folder to Surveying_Calculator/projects folder on your device. Select the project to display in Survey module.
→ Collect unlimited points with "name" and "description". You can record your location or any place you want.
→ This app supports the formats to display:
→ Raster (GeoTIFF, DEM, JPEG, PNG, GRD, XYZ, ..),
→ CAD (DXF, DGN v7),
→ GIS (ArcGIS shp, Google Earth (kml, kmz), GeoJSON, GPX..),
→ Online map services. Online maps can be prepared using QuickMapServices plugin in QGIS. You can add Open Street Maps, hybrid or satellite maps in QGIS project for Surveying Calculator.
- Generate Coordinates: Generate coordinates in latitude/ longitude or in X, Y.
- Settings: Coordinate order of northing, easting or latitude, longitude. DMS or decimal options for geographic coordinate systems. Scale bar unit settings in metric or imperial.
- and more.
Surveying Calculator supports Android 6.0 and up. Minimum 2GB RAM with 300MB free space are recommended.
If you need help or if you have any suggestion about this app please mail to geosoft@gmail.com or join Surveying Calculator Facebook group to ask questions. You can follow news about the app on Facebook Page:
fb.me/surveyingcalculator
You can open issues for any suggestions or bug reports.

# Development
Surveying calculator uses open source GIS libraries (QGIS, GDAL, Proj, SpatialIndex, GEOS) libraries. This app uses source codes of Input app(https://github.com/lutraconsulting/input). 

Requirements:

Qt 5.14.2

Fluid QML Library (https://github.com/lirios/fluid)

QGIS libraries from input-sdk/android-13 (armv7 and armv8), also QgsQuick libraries and files
