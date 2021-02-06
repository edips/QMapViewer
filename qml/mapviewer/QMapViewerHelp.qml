import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Universal 2.2
import Fluid.Controls 1.0 as FluidControls
import Fluid.Core 1.0 as FluidCore
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3
import "components"
/** The form toolbar **/
TopSheet{
    id: mapviewer_help
    title: "Help"
    SFlickable {
        id:optionsPage
        anchors.top: toolbar.bottom
        contentHeight: Math.max(optionsColumn.implicitHeight, height)+50
SColumnHelp{
    id: optionsColumn
    STextHelp{
            stext:qsTr("QMapViewer displays QGIS projects on Android. You can display vector, raster, geodatabase and online maps formats on Android using QGIS. QGIS is a free geographic information system (GIS) application
 that supports viewing, editing, and analysis of geospatial data. It works on Windows, Linux, and Mac OSX. You can download QGIS from www.qgis.org.
 <br> <br>
You can easily import CAD or GIS data to QGIS. After you prepared QGIS project, you can transfer your qgs project with its files in a folder to your
 phone's storage with USB cable. You should copy your QGIS project folder to Surveying_Calculator/projects folder.

 ")

    }


    Rectangle{
        anchors.horizontalCenter: parent.horizontalCenter
        height:sin_g.height
        width:sin_g.width
        color: "transparent"
        Image {
            id:sin_g
            width:300
            fillMode: Image.PreserveAspectFit
            source:"qrc:/assets/images/help/mapviewer/mv1.png"
        }

    }
    Rectangle{
        anchors.horizontalCenter: parent.horizontalCenter
        height:sin_gr.height+10
        width:sin_gr.width
        color: "transparent"
        Image {
            id:sin_gr
            width:300
            fillMode: Image.PreserveAspectFit
            source:"qrc:/assets/images/help/mapviewer/mv2.png"
        }
    }
    Rectangle{
        anchors.horizontalCenter: parent.horizontalCenter
        height:sin_gq.height+10
        width:sin_gq.width
        color: "transparent"
        Image {
            id:sin_gq
            width: 300
            fillMode: Image.PreserveAspectFit
            source:"qrc:/assets/images/help/mapviewer/mv3.png"
        }
    }
    Rectangle{
        anchors.horizontalCenter: parent.horizontalCenter
        height:sin_gw.height+10
        width:sin_gw.width
        color: "transparent"
        Image {
            id:sin_gw
            width:300
            fillMode: Image.PreserveAspectFit
            source:"qrc:/assets/images/help/mapviewer/mv4.png"
        }
    }

    STextHelp{
        id:helplabel2
        stext:qsTr("
<b>Open Project:</b> Opens QGIS project in Surveying_Calculator/projects folder.<br>
<b>Enable GPS:</b> Centers map canvas to user's position. It works when the accuracy of GPS is under 20 meters.<br>
<b>Layers:</b> Displays available map layers. You can turn on or turn off layers. It doesn't support sub layers.<br>
<b>Zoom to Project:</b> Zooms the extent of all visible layers within the project.<br>
<b>Settings:</b> Configuration for scale bar visibility or scale bar units.<br>
<br>
<b>Getting Properties of Geometric Features</b><br>
<b>Polygon:</b> Click on a polygon to get area and attribute information. You can see area in any unit. You can display or edit attribute information of the polygon.<br>
<b>Line or multi-line:</b> Click on a line to get length or attribute information. You can see length of line in any unit. You can display or edit attribute information of the line.<br>
<b>Point:</b> You can display or edit attribute information of a point.<br>

<b>Full screen button:</b> You can use it to display maps in full screen mode. The map is in read-only mode in full screen. So the map is not editable and selectable.<br>
<br>
<br>QGIS projects can include vector, raster and geodatabase data.<br><b>Supported CAD-GIS formats:</b><br>
    • shp (ESRI shapefiles)<br>
    • mif, tab (MapInfo File)<br>
    • dgn (MicroStation v7)<br>
    • dxf (AutoCAD)<br>
    • PostGIS<br>
    • SpatiaLite<br>
    • e00 (ArcInfo ASCII Coverage)<br>
    • mdb (ESRI Personal GeoDatabase)<br>
    • gpkg (Geopackage)<br>
    • gpx<br>
    • GeoJSON<br>
    • TopoJSON<br>
    • GML<br>
    • Geospatial PDF<br>
    • svg<br>
    • kml, kmz<br>
    • osm (OpenStreet Map)<br>
    and many more. <br>
    <br>
    <b>Supported Raster formats:</b><br>
    • xyz (ASCII Gridded XYZ)<br>
    • asc (ArcInfo ASCII Grid)<br>
    • e00 (ArcInfo export E00 grid)<br>
    • img (ERDAS Imagine img)<br>
    • GeoTIFF<br>
    • grd (Golden Software 7 Binary Grid)<br>
    • JPEG, PNG, BMP<br>
    • DEM(USGS ASCII DEM)<br>
    and many more.<br>
    ")

        }


}
    }
}
