#include "loader.h"
#include "inpututils.h"
#include "qgsvectorlayer.h"
#include "qgslayertree.h"
#include "qgslayertreelayer.h"
#include "qgslayertreegroup.h"
#include "qgsmapthemecollection.h"
#include "qgsquickmapcanvasmap.h"
#include "qgsquickmapsettings.h"


#include "layersmodel.h"

#if VERSION_INT >= 30500
// this header only exists in QGIS >= 3.6
#include "qgsexpressioncontextutils.h"
#endif
#include <QDebug>

const QString Loader::LOADING_FLAG_FILE_PATH = QString( "%1/.input_loading_project" ).arg( QStandardPaths::standardLocations( QStandardPaths::TempLocation ).first() );

Loader::Loader(
        MapThemesModel &mapThemeModel
        , AppSettings &appSettings
        , ActiveLayer &activeLayer
        , QObject *parent ) :

    QObject( parent )
  , mMapThemeModel( mapThemeModel )
  , mAppSettings( appSettings )
  , mActiveLayer( activeLayer )
{
    // we used to have our own QgsProject instance, but unfortunately few pieces of qgis_core
    // still work with QgsProject::instance() singleton hardcoded (e.g. vector layer's feature
    // iterator uses it for virtual fields, causing minor bugs with expressions)
    // so for the time being let's just stick to using the singleton until qgis_core is completely fixed
    mProject = QgsProject::instance();
}

QgsProject *Loader::project()
{
    return mProject;
}

void Loader::setPositionKit( QgsQuickPositionKit *kit )
{
    mPositionKit = kit;
    emit positionKitChanged();
}

void Loader::setRecording( bool isRecordingOn )
{
    if ( mRecording != isRecordingOn )
    {
        mRecording = isRecordingOn;
        emit recordingChanged();
    }
}

bool Loader::load( const QString &filePath )
{
    return forceLoad( filePath, false );
}

bool Loader::forceLoad( const QString &filePath, bool force )
{
    qDebug() << "Loading " << filePath << force;
    // Just clear project if empty
    if ( filePath.isEmpty() )
    {
        emit projectWillBeReloaded();
        mProject->clear();
        whileBlocking( &mActiveLayer )->resetActiveLayer();
        emit projectReloaded( mProject );
        return true;
    }

    if ( !force )
        emit loadingStarted();
    QFile flagFile( LOADING_FLAG_FILE_PATH );
    flagFile.open( QIODevice::WriteOnly );
    flagFile.close();

    // Give some time to other (GUI) processes before loading a project in the main thread
    QEventLoop loop;
    QTimer t;
    t.connect( &t, &QTimer::timeout, &loop, &QEventLoop::quit );
    t.start( 10 );
    loop.exec();

    bool res = true;
    if ( mProject->fileName() != filePath || force )
    {
        emit projectWillBeReloaded();
        res = mProject->read( filePath );
        mActiveLayer.resetActiveLayer();
        mMapThemeModel.reloadMapThemes( mProject );
        setActiveLayer( mAppSettings.defaultLayer() );
        setMapSettingsLayers();

        emit projectReloaded( mProject );
    }

    flagFile.remove();
    if ( !force )
        emit loadingFinished();
    return res;
}

bool Loader::reloadProject( QString projectDir )
{
    if ( mProject->homePath() == projectDir )
    {
        return forceLoad( mProject->fileName(), true );
    }
    return false;
}

void Loader::setMapSettings( QgsQuickMapSettings *mapSettings )
{
    if ( mMapSettings == mapSettings )
        return;

    mMapSettings = mapSettings;
    setMapSettingsLayers();

    emit mapSettingsChanged();
}

void Loader::setMapSettingsLayers() const
{
    if ( !mProject || !mMapSettings ) return;

    QgsLayerTree *root = mProject->layerTreeRoot();

    // Get list of all visible and valid layers in the project
    QList< QgsMapLayer * > allLayers;
    foreach ( QgsLayerTreeLayer *nodeLayer, root->findLayers() )
    {
        if ( nodeLayer->isVisible() )
        {
            QgsMapLayer *layer = nodeLayer->layer();
            if ( layer && layer->isValid() )
            {
                allLayers << layer;
            }
        }
    }

    mMapSettings->setLayers( allLayers );
    mMapSettings->setTransformContext( mProject->transformContext() );
}

QgsQuickMapSettings *Loader::mapSettings() const
{
    return mMapSettings;
}

void Loader::zoomToProject( QgsQuickMapSettings *mapSettings )
{
    if ( !mapSettings )
    {
        qDebug() << "Cannot zoom to extent, mapSettings is not defined";
        return;
    }
    QgsRectangle extent;

    // Check if WMSExtent is set in project
    bool hasWMS;
    QStringList WMSExtent = mProject->readListEntry( "WMSExtent", QStringLiteral( "/" ), QStringList(), &hasWMS );

    if ( hasWMS && ( WMSExtent.length() == 4 ) )
    {
        extent.set( WMSExtent[0].toDouble(), WMSExtent[1].toDouble(), WMSExtent[2].toDouble(), WMSExtent[3].toDouble() );
    }
    else // set layers extent
    {
        // We added this when changing the visibility of layers.
        const QVector<QgsMapLayer *> layers = mProject->layers<QgsMapLayer *>();

        QgsLayerTree *root = QgsProject::instance()->layerTreeRoot();
        foreach ( QgsLayerTreeLayer *nodeLayer, root->findLayers() )
        {
            if ( nodeLayer->isVisible() )
            {
                QgsMapLayer *nLayer = nodeLayer->layer();
                if ( nLayer && nLayer->isValid() && nLayer->id() == nodeLayer->layer()->id() )
                {
                    QgsRectangle layerExtent = mapSettings->mapSettings().layerExtentToOutputExtent( nodeLayer->layer(), nodeLayer->layer()->extent() );
                    extent.combineExtentWith( layerExtent );
                }
            }
        }
    }

    if ( extent.isEmpty() )
    {
        extent.grow( mProject->crs().isGeographic() ? 0.01 : 1000.0 );
    }
    extent.scale( 1.05 );

    mapSettings->setExtent( extent );
}

QString Loader::featureTitle( QgsQuickFeatureLayerPair pair )
{
    QgsExpressionContext context( globalProjectLayerScopes( pair.layer() ) );
    context.setFeature( pair.feature() );
    QgsExpression expr( pair.layer()->displayExpression() );
    return expr.evaluate( &context ).toString();
}

QString Loader::mapTipHtml( QgsQuickFeatureLayerPair pair )
{
    QgsExpressionContext context( globalProjectLayerScopes( pair.layer() ) );
    context.setFeature( pair.feature() );
    return QgsExpression::replaceExpressionText( pair.layer()->mapTipTemplate(), &context );
}

QString Loader::mapTipType( QgsQuickFeatureLayerPair pair )
{
    // Stripping extra CR char to unify Windows lines with Unix.
    QString mapTip = pair.layer()->mapTipTemplate().replace( QStringLiteral( "\r" ), QStringLiteral( "" ) );
    if ( mapTip.startsWith( "# image\n" ) )
        return "image";
    else if ( mapTip.startsWith( "# fields\n" ) || mapTip.isEmpty() )
        return "fields";
    else
        return "html";
}

QString Loader::mapTipImage( QgsQuickFeatureLayerPair pair )
{
    QgsExpressionContext context( globalProjectLayerScopes( pair.layer() ) );
    context.setFeature( pair.feature() );
    QString mapTip = pair.layer()->mapTipTemplate();
    QStringList lst = mapTip.split( '\n' ); // first line is "# image"
    if ( lst.count() >= 2 )
        return QgsExpression::replaceExpressionText( lst[1], &context );
    else
        return QString();
}

QStringList Loader::mapTipFields( QgsQuickFeatureLayerPair pair )
{
    QString mapTip = pair.layer()->mapTipTemplate();
    QStringList lst;
    const QgsFields fields = pair.layer()->fields();
    const int LIMIT = 3;  // max. 3 fields can fit in the preview

    if ( mapTip.isEmpty() )
    {
        // user has not provided any map tip - let's use first two fields to show
        // at least something.
        QString featureTitleExpression = pair.layer()->displayExpression();
        for ( QgsField field : fields )
        {
            if ( featureTitleExpression != field.name() )
                lst << field.displayName();  // yes, using alias, not the original field name
            if ( lst.count() == LIMIT )
                break;
        }
    }
    else
    {
        // user has specified "# fields" on the first line and then each next line is a field name
        QStringList lines = mapTip.split( '\n' );
        for ( int i = 1; i < lines.count(); ++i ) // starting from index to avoid first line with "# fields"
        {
            int index = fields.indexFromName( lines[i] );
            if ( index >= 0 )
                lst << fields[index].displayName();  // yes, using alias, not the original field name
            if ( lst.count() == LIMIT )
                break;
        }
    }
    return lst;
}

bool Loader::layerVisible( QgsMapLayer *layer )
{
    if ( !layer ) return false;

    // check if active layer is visible in current map theme too
    QgsLayerTree *root = QgsProject::instance()->layerTreeRoot();
    foreach ( QgsLayerTreeLayer *nodeLayer, root->findLayers() )
    {
        if ( nodeLayer->isVisible() )
        {
            QgsMapLayer *nLayer = nodeLayer->layer();
            if ( nLayer && nLayer->isValid() && nLayer->id() == layer->id() )
            {
                return true;
            }
        }
    }
    return false;
}

void Loader::setActiveMapTheme( int index )
{
    QString name = mMapThemeModel.setActiveThemeIndex( index );

    // if active layer is no longer visible, reset it
    if ( !layerVisible( mActiveLayer.layer() ) )
        setActiveLayer( nullptr );

    setMapSettingsLayers();
}

void Loader::appStateChanged( Qt::ApplicationState state )
{
    QString msg;

    // Instatiate QDebug with QString to redirect output to string
    // It is used to convert enum to string
    QDebug logHelper( &msg );

    logHelper << "Application changed state to: " << state;
    InputUtils::log( "Input", msg );

    if ( !mRecording && mPositionKit )
    {
        if ( state == Qt::ApplicationActive )
        {
            mPositionKit->source()->startUpdates();
        }
        else
        {
            mPositionKit->source()->stopUpdates();
        }
    }
}

void Loader::appAboutToQuit()
{
    InputUtils::log( "Input", "Application has quit" );
}

QList<QgsExpressionContextScope *> Loader::globalProjectLayerScopes( QgsMapLayer *layer )
{
    // can't use QgsExpressionContextUtils::globalProjectLayerScopes() because it uses QgsProject::instance()
    QList<QgsExpressionContextScope *> scopes;
    scopes << QgsExpressionContextUtils::globalScope();
    scopes << QgsExpressionContextUtils::projectScope( mProject );
    scopes << QgsExpressionContextUtils::layerScope( layer );
    return scopes;
}

void Loader::setActiveLayer( QString layerName ) const
{
    if ( !layerName.isEmpty() )
    {
        QList<QgsMapLayer *> layersByName = QgsProject::instance()->mapLayersByName( layerName );

        if ( !layersByName.isEmpty() )
        {
            return setActiveLayer( layersByName.at( 0 ) );
        }
    }

    setActiveLayer( nullptr );
}

void Loader::setActiveLayer( QgsMapLayer *layer ) const
{
    if ( !layer || !layer->isValid() )
        mActiveLayer.resetActiveLayer();
    else
    {
        mActiveLayer.setActiveLayer( layer );
        mAppSettings.setDefaultLayer( mActiveLayer.layerName() );
    }
}

QString Loader::loadIconFromLayer( QgsMapLayer *layer )
{
    if ( !layer )
        return QString();

    QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );

    if ( vectorLayer )
    {
        QgsWkbTypes::GeometryType geometry = vectorLayer->geometryType();
        return iconFromGeometry( geometry );
    }
    else
        return QString( "qrc:/assets/icons/mIconRaster.svg" );
}

QString Loader::loadIconFromFeature( QgsFeature feature )
{
    return iconFromGeometry( feature.geometry().type() );
}

QString Loader::iconFromGeometry( const QgsWkbTypes::GeometryType &geometry )
{
    switch ( geometry )
    {
    case QgsWkbTypes::GeometryType::PointGeometry: return QString( "qrc:/assets/icons/mIconPointLayer.svg" );
    case QgsWkbTypes::GeometryType::LineGeometry: return QString( "qrc:/assets/icons/mIconLineLayer.svg" );
    case QgsWkbTypes::GeometryType::PolygonGeometry: return QString( "qrc:/assets/icons/mIconPolygonLayer.svg" );
    default: return QString( "qrc:/assets/icons/mIconTableLayer.svg" );
    }
}


// By Edip
long Loader::epsg_code()
{
    return mProject->crs().postgisSrid();
}

QString Loader::epsg_name()
{
    if( mActiveLayer.layer()->crs().postgisSrid() ){
        return mActiveLayer.layer()->crs().description();
    }else{
        return "Invalid Coordinate System";
    }
}

bool Loader::isGeographic()
{
    if( mProject->crs().isGeographic() ){
        return true;
    }
    return false;
}

bool Loader::isLayerGeographic()
{
    if( mActiveLayer.layer()->crs().isGeographic() ) {
        return true;
    }
    return false;
}

bool Loader::crsValid()
{
    if(mProject->crs().postgisSrid()){
        return true;
    }
    else{
        return false;
    }
}

QString Loader::homePath()
{
    QgsProject *maProj = mProject->instance();
    qDebug()<< "homeeeeeeeeeee pathhhhhhhh : " << maProj->homePath();
    return maProj->homePath();
}

void Loader::zoom_to_point(QgsQuickMapSettings *mapSettings, QPointF point)
{
    if ( !mapSettings )
    {
        qDebug() << "Cannot zoom to extent, mapSettings is not defined";
        return;
    }
    //QgsRectangle extent = mapSettings->extent();
    QgsPoint mousePos( mapSettings->screenToCoordinate( point ) );
    qDebug() << "X: " << mousePos.x() << " Y: " << QString::number(mousePos.y(), 'f', 2);
    qDebug() << "5";
    QgsPointXY newCenter( mousePos.x(), mousePos.y() );

    double scale_param = isGeographic() ? 0.002 : 50.0;

    QgsPointXY corner1 = QgsPointXY(newCenter.x() + scale_param, newCenter.y() + scale_param);
    QgsPointXY corner2 = QgsPointXY(newCenter.x() - scale_param, newCenter.y() - scale_param);

    qDebug() << "6";
    QgsRectangle extent = QgsRectangle(corner1, corner2);

    // same as zoomWithCenter (no coordinate transformations are needed)
    // extent.scale( 1, &newCenter );
    mapSettings->setExtent( extent );
}


// Work in process, there are bugs and crashes after importing layer.
// try it on a simple project
// Projects model may be a folder list model if only folder model works
bool Loader::addVectorLayer(QString path)
{

    QFileInfo file(path);
    qDebug() << "file path is " << path;
    QString fileName = file.fileName();
    qDebug() << "file name is " << fileName;
    QgsVectorLayer *vectorLayer = new QgsVectorLayer(path, fileName, "ogr");

    if(!vectorLayer->isValid()) {
        qDebug() << "not a valid layer!";
        return false;
    }

    //qDebug() << "vectorLayer->crs().toProj(): " << vectorLayer->crs().toProj();

    //qDebug() << "mProject->crs().toProj(): " << mProject->crs().toProj();

    qDebug() << "converting layer crs to proj crs";
    vectorLayer->setCrs(mProject->crs(), true);
    qDebug() << "crs conversion is done";

    qDebug() << "debug 1";

    mProject->instance()->addMapLayer(vectorLayer);

    qDebug() << "debug 2";
    mActiveLayer.resetActiveLayer();
    qDebug() << "d2.1";
    mMapThemeModel.reloadMapThemes( mProject );
    qDebug() << "d2.2";
    setActiveLayer( mAppSettings.defaultLayer() );
    qDebug() << "d2.3";
    setMapSettingsLayers();


    qDebug() << "debug 3";
    mProject->reloadAllLayers();

    mProject->write();

    qDebug() << "debug 4";

    qDebug() << "is it ok?";



    delete vectorLayer;

    return true;
}

bool Loader::layerVisibility( QString layerId ) {
    // check if active layer is visible in current map theme too
    QgsLayerTree *root = QgsProject::instance()->layerTreeRoot();
    QgsMapLayer *layer = root->findLayer( layerId )->layer();
    if ( !layer ) return false;

    foreach ( QgsLayerTreeLayer *nodeLayer, root->findLayers() )
    {
        if ( nodeLayer->isVisible() )
        {
            QgsMapLayer *nLayer = nodeLayer->layer();
            if ( nLayer && nLayer->isValid() && nLayer->id() == layer->id() )
            {
                return true;
            }
        }
    }
    return false;
}

bool Loader::layerChecked( QString layerId )
{
    mProject = QgsProject::instance();
    QgsLayerTree *root = mProject->layerTreeRoot();
    // Layer visibility setting:
    root->findLayer(layerId)->setItemVisibilityChecked( !root->findLayer( layerId )->itemVisibilityChecked() );

    mActiveLayer.resetActiveLayer();

    mMapThemeModel.reloadMapThemes( mProject );

    setActiveLayer( mAppSettings.defaultLayer() );

    setMapSettingsLayers();

    mProject->reloadAllLayers();

    emit projectReloaded( mProject );


    root->findLayer( layerId )->layer()->triggerRepaint();

    return root->findLayer( layerId )->itemVisibilityChecked();
}


QVector<qreal> Loader::coordTransformer( QgsPoint sourceP, QgsCoordinateTransformContext context, QString sourceEPSG, QString destinationEPSG )
{
    double x = sourceP.x();
    double y = sourceP.y();
    double z = sourceP.z();

    // If Z is NaN, coordinate transformation (proj4) will
    // also set X and Y to NaN. But we also want to get projected
    // coords if we do not have any Z coordinate.
    if ( std::isnan( z ) )
    {
        z = 0;
    }

    QgsCoordinateTransform mCoordinateTransform;
    mCoordinateTransform.setSourceCrs( QgsCoordinateReferenceSystem("EPSG:" + sourceEPSG ) );
    mCoordinateTransform.setDestinationCrs( QgsCoordinateReferenceSystem("EPSG:" + destinationEPSG ) );
    mCoordinateTransform.setContext( context );
    try
    {
        mCoordinateTransform.transformInPlace( x, y, z );
    }
    catch ( const QgsCsException &exp )
    {
        qDebug() << exp.what();
    }

    QgsPoint mProjectedPosition = QgsPoint( x, y );
    mProjectedPosition.addZValue( sourceP.z() );
    QVector<qreal> coords;
    coords << mProjectedPosition.x() << mProjectedPosition.y();
    return coords;
}




