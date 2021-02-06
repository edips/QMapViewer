#include "surveyingutils.h"
#include "qgsquickutils.h"

// for new project
#include "qgis.h"
#include "qgspointxy.h"
#include <qgsvectorlayer.h>
//#include <qgsvectorfilewriter.h>
#include <qgspallabeling.h>
#include <qgsvectorlayerlabeling.h>
#include <qgscategorizedsymbolrenderer.h>
#include <qgsmarkersymbollayer.h>
#include <qgssinglesymbolrenderer.h>
#include "layersmodel.h"
#include "inpututils.h"


SurveyingUtils::SurveyingUtils( QObject *parent )
    : QObject( parent )
    , mMapSettings( nullptr )
{

}

static QString getDataDir()
{
    QString dataPathRaw("Surveying_Calculator");

#ifdef ANDROID
    QFileInfo extDir( "/sdcard/" );
    if ( extDir.isDir() && extDir.isWritable() )
    {
        // seems that this directory transposes to the latter one in case there is no sdcard attached
        dataPathRaw = extDir.path() + "/" + dataPathRaw;
    }
    else
    {
        qDebug() << "extDir: " << extDir.path() << " not writable";

        QStringList split = QDir::homePath().split( "/" ); // something like /data/user/0/org.project.geoclass/files
        // TODO support active user from QDir::homePath()
        QFileInfo usrDir( "/storage/emulated/" + split[2] + "/" );
        dataPathRaw = usrDir.path() + "/" + dataPathRaw;
        if ( !( usrDir.isDir() && usrDir.isWritable() ) )
        {
            qDebug() << "usrDir: " << usrDir.path() << " not writable";
        }
    }
#endif
    qputenv( "QGIS_QUICK_DATA_PATH", dataPathRaw.toUtf8().constData() );
    QString dataDir = QString::fromLocal8Bit( qgetenv( "QGIS_QUICK_DATA_PATH" ) ) ;
    qDebug() << "QGIS_QUICK_DATA_PATH: " << dataDir;
    return dataDir;
}

long SurveyingUtils::epsg_code()
{
    /*
        // srsid givers wrong epsg id for many coordinate systems. Class reference also mentions to use PostgissrsID :createFromId	(long id, CrsType type = PostgisCrsId )
        qDebug() << "mProject->crs().srsid::::::::::::" << mProject->crs().srsid();
        // this works for wgs84 utm whick srsid didn't work
        qDebug() << "mProject->crs().postgisSrid::::::::::::" << mProject->crs().postgisSrid();
        //------------------------------------------------------------------------------------
        // mapUnits output is: QgsUnitTypes::DistanceMeters
        qDebug() << "mProject->crs().mapUnits::::::::::::" << mProject->crs().mapUnits();
        // name of EPSG coordinate system
        qDebug() << "mProject->crs().srsid::::::::::::" << mProject->crs().description();
    */
    return mProject->crs().postgisSrid();
}

QString SurveyingUtils::epsg_name()
{
    if(mProject->crs().isValid()){
        qDebug() << "EPSG Name in cpp: " << mProject->crs().description();
        return mProject->crs().description();
    }else{
        qDebug() << "CRS is not recognized!";
        return "WGS84";
    }
}

bool SurveyingUtils::isGeographic()
{
    if ( mProject->crs().isGeographic() ){
        return true;
    }
    else {
        return false;
    }
}

bool SurveyingUtils::crsValid()
{
    if(mProject->crs().postgisSrid()){
        return true;
    }
    else{
        return false;
    }
}

QString SurveyingUtils::homePath()
{
    QgsProject *maProj = mProject->instance();
    qDebug()<< "homeeeeeeeeeee pathhhhhhhh : " << maProj->homePath();
    return maProj->homePath();
}

QString SurveyingUtils::transformer(QString x, QString y, QString src, QString dst)
{
    // Source CRS, type of it is QgsCoordinateReferenceSystem
    QgsCoordinateReferenceSystem source = QgsCoordinateReferenceSystem("EPSG:" + src);
    // Destination CRS, type of it is QgsCoordinateReferenceSystem
    QgsCoordinateReferenceSystem destination = QgsCoordinateReferenceSystem("EPSG:" + dst);

    qDebug() << "src epsg: " << src << " destination epsg: " << dst;
    qDebug() << "source.isValid()? " << source.isValid() << " destination.isValid()? " << destination.isValid();
    //qDebug() << "source.toProj4(): " << source.toProj() << " destination.toProj4(): " << destination.toProj();
    qDebug() << "x: " << x << " y: " << y;
    qDebug() << "x.toDouble(): " << x.toDouble() << " y.toDouble(): " << y.toDouble();

    if(!(source.isValid()) || !(destination.isValid())){
        qDebug() << "source.isValid()? " << source.isValid() << " destination.isValid()? " << destination.isValid();
        return "";
    }
    // srcPoint from double values, type is QgsPointXY
    QgsPoint srcPoint = QgsPoint( x.toDouble(), y.toDouble(), 0);
    qDebug() << "debug transform 1";
    QString coords;

    QgsCoordinateTransformContext context;
    if ( mMapSettings )
        context = mMapSettings->transformContext();
    qDebug() << "debug transform 2";
    QgsCoordinateTransform mTransform(source, destination, context);
    qDebug() << "debug transform 3";
    // Transform coordinate and assign it to pt variable
    mTransform.transform( srcPoint );
    coords = mTransform.transform( srcPoint ).toString(8);
    return coords;
}

bool SurveyingUtils::projDefValid(QString projDef)
{
    QgsCoordinateReferenceSystem destination = QgsCoordinateReferenceSystem::fromProj(projDef);
    return destination.isValid();
}

/*!
    Under Construction
*/
bool SurveyingUtils::no_project()
{
    //qDebug() << "Project file length**************** " << mProjectFiles.length();
    //return mProjectFiles.length() == 0 ? true : false;
    return false;
}

bool SurveyingUtils::featureIsPoint(QgsQuickFeatureLayerPair p)
{
    QgsVectorLayer *layer = p.layer();
    return layer && layer->geometryType() == QgsWkbTypes::PointGeometry;
}

bool SurveyingUtils::featureIsLine(QgsQuickFeatureLayerPair p)
{
    QgsVectorLayer *layer = p.layer();
    return layer && layer->geometryType() == QgsWkbTypes::LineGeometry;
}

bool SurveyingUtils::featureIsPolygon(QgsQuickFeatureLayerPair p)
{
    QgsVectorLayer *layer = p.layer();
    return layer && layer->geometryType() == QgsWkbTypes::PolygonGeometry;
}

QString SurveyingUtils::getArea(QgsQuickFeatureLayerPair p)
{
    QString area_str;
    if( featureIsPolygon( p ) ) {
        QgsDistanceArea area;
        // Get area from ellipsoidal earth
        if( p.layer()->crs().isGeographic() ) {
            area.setEllipsoid( p.layer()->crs().ellipsoidAcronym() );
            area_str = QString::number( area.measureArea( p.feature().geometry() ), 'f', 2 );
        }
        // Get area from cartesien flat earth (accurate for big scale maps)
        else {
            area_str = QString::number( area.measureArea( p.feature().geometry() ), 'f', 2 );
        }
    }
    return area_str;
}

QString SurveyingUtils::getLength(QgsQuickFeatureLayerPair p)
{
    QString length_str;
    if( featureIsLine( p ) ) {
        QgsDistanceArea length;
        // Get length from ellipsoidal earth
        if( p.layer()->crs().isGeographic() ) {
            length.setEllipsoid( p.layer()->crs().ellipsoidAcronym() );
            length_str = QString::number( length.measureLength( p.feature().geometry() ), 'f', 2 );
        }
        // Get length from cartesien flat earth (accurate for big scale maps)
        else {
            length_str = QString::number( length.measureLength( p.feature().geometry() ), 'f', 2 );
        }
    }
    return length_str;
}
