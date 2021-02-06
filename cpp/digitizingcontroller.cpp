#include "digitizingcontroller.h"

#include "qgslinestring.h"
#include "qgsvectorlayer.h"
#include "qgswkbtypes.h"
#include "qgspolygon.h"

#include "qgsquickutils.h"
#include "qgsvectorlayerutils.h"

DigitizingController::DigitizingController( QObject *parent )
  : QObject( parent )
  , mMapSettings( nullptr )
{
  mRecordingModel = new QgsQuickAttributeModel( this );
}

void DigitizingController::setPositionKit( QgsQuickPositionKit *kit )
{
  if ( mPositionKit )
    disconnect( mPositionKit, &QgsQuickPositionKit::positionChanged, this, &DigitizingController::onPositionChanged );

  mPositionKit = kit;

  if ( mPositionKit )
    connect( mPositionKit, &QgsQuickPositionKit::positionChanged, this, &DigitizingController::onPositionChanged );

  emit positionKitChanged();
}

QgsQuickFeatureLayerPair DigitizingController::featureLayerPair() const
{
  return mRecordingModel->featureLayerPair();
}

void DigitizingController::setFeatureLayerPair( QgsQuickFeatureLayerPair pair )
{
  if ( pair == mRecordingModel->featureLayerPair() )
    return;

  mRecordingModel->setFeatureLayerPair( pair );
  emit layerChanged();
}

QgsVectorLayer *DigitizingController::layer() const
{
  return mRecordingModel->featureLayerPair().layer();
}

void DigitizingController::setLayer( QgsVectorLayer *layer )
{
  if ( layer == mRecordingModel->featureLayerPair().layer() )
    return;

  QgsQuickFeatureLayerPair pair( mRecordingModel->featureLayerPair().featureRef(), layer );
  mRecordingModel->setFeatureLayerPair( pair );
  emit layerChanged();
}

bool DigitizingController::hasLineGeometry( QgsVectorLayer *layer ) const
{
  return layer && layer->geometryType() == QgsWkbTypes::LineGeometry;
}

bool DigitizingController::hasPolygonGeometry( QgsVectorLayer *layer ) const
{
  return layer && layer->geometryType() == QgsWkbTypes::PolygonGeometry;
}

bool DigitizingController::hasPointGeometry( QgsVectorLayer *layer ) const
{
  return layer && layer->geometryType() == QgsWkbTypes::PointGeometry;
}

bool DigitizingController::isPairValid( QgsQuickFeatureLayerPair pair ) const
{
  return pair.isValid() && hasEnoughPoints();
}

void DigitizingController::fixZ( QgsPoint &point ) const
{
  if ( !featureLayerPair().layer() )
    return;

  bool layerIs3D = QgsWkbTypes::hasZ( featureLayerPair().layer()->wkbType() );
  bool pointIs3D = QgsWkbTypes::hasZ( point.wkbType() );

  if ( layerIs3D )
  {
    if ( !pointIs3D )
    {
      point.addZValue();
    }
  }
  else /* !layerIs3D */
  {
    if ( pointIs3D )
    {
      point.dropZValue();
    }
  }
}

QgsCoordinateTransform DigitizingController::transformer() const
{
  QgsCoordinateTransformContext context;
  if ( mMapSettings )
    context = mMapSettings->transformContext();

  QgsCoordinateTransform transform( QgsCoordinateReferenceSystem( "EPSG:4326" ),
                                    featureLayerPair().layer()->crs(),
                                    context );
  return transform;
}

bool DigitizingController::hasEnoughPoints() const
{
  if ( hasLineGeometry( featureLayerPair().layer() ) )
  {
    return mRecordedPoints.length() >= 2;
  }
  else if ( hasPolygonGeometry( featureLayerPair().layer() ) )
  {
    return mRecordedPoints.length() >= 3;
  }

  // Point capturing doesn't use mRecordedPoints
  return true;
}

bool DigitizingController::useGpsPoint() const
{
  return mUseGpsPoint;
}

void DigitizingController::setUseGpsPoint( bool useGpsPoint )
{
  mUseGpsPoint = useGpsPoint;
  emit useGpsPointChanged();
}

bool DigitizingController::manualRecording() const
{
  return mManualRecording;
}

void DigitizingController::setManualRecording( bool manualRecording )
{
  mManualRecording = manualRecording;
  emit manualRecordingChanged();
}

int DigitizingController::lineRecordingInterval() const
{
  return mLineRecordingInterval;
}

void DigitizingController::setLineRecordingInterval( int lineRecordingInterval )
{
  mLineRecordingInterval = lineRecordingInterval;
  emit lineRecordingIntervalChanged();
}


std::unique_ptr<QgsPoint> DigitizingController::getLayerPoint( const QgsPoint &point, bool isGpsPoint )
{
  std::unique_ptr<QgsPoint> layerPoint = nullptr;
  if ( isGpsPoint )
  {

    QgsPoint *tempPoint = point.clone();
    QgsGeometry geom( tempPoint );
    QgsCoordinateTransform ct( mPositionKit->positionCRS(), featureLayerPair().layer()->crs(), mMapSettings->mapSettings().transformContext() );
    geom.transform( ct );
    layerPoint = std::unique_ptr<QgsPoint>( qgsgeometry_cast<QgsPoint *>( geom.get() )->clone() );

  }
  else
  {
    QgsPointXY layerPointXY = mMapSettings->mapSettings().mapToLayerCoordinates( featureLayerPair().layer(), QgsPointXY( point.x(), point.y() ) );
    layerPoint = std::unique_ptr<QgsPoint>( new QgsPoint( layerPointXY ) );
  }
  fixZ( *layerPoint );

  return layerPoint;
}

QgsGeometry DigitizingController::getPointGeometry( const QgsPoint &point, bool isGpsPoint )
{
  std::unique_ptr<QgsPoint> layerPoint = getLayerPoint( point, isGpsPoint );
  QgsGeometry geom( layerPoint.release() );

  return geom;
}

QgsQuickFeatureLayerPair DigitizingController::createFeatureLayerPair( const QgsGeometry &geometry )
{
  QgsAttributes attrs( featureLayerPair().layer()->fields().count() );
  QgsExpressionContext context = featureLayerPair().layer()->createExpressionContext();
  QgsFeature feat = QgsVectorLayerUtils::createFeature( featureLayerPair().layer(), geometry, attrs.toMap(), &context );

  return QgsQuickFeatureLayerPair( feat, featureLayerPair().layer() );
}

QgsQuickFeatureLayerPair DigitizingController::pointFeatureFromPoint( const QgsPoint &point, bool isGpsPoint )
{
  if ( !featureLayerPair().layer() || !mMapSettings )
    return QgsQuickFeatureLayerPair();

  QgsGeometry geom = getPointGeometry( point, isGpsPoint );

  return createFeatureLayerPair( geom );
}

void DigitizingController::startRecording()
{
  if ( mRecording ) return;

  mRecordedPoints.clear();
  mRecording = true;
  emit recordingChanged();
}

void DigitizingController::stopRecording()
{
  mRecording = false;
  emit recordingChanged();
}

void DigitizingController::onPositionChanged()
{
  if ( mManualRecording )
    return;

  if ( !mRecording )
    return;

  if ( !mPositionKit->hasPosition() )
    return;

  QgsPoint point = mPositionKit->position();
  std::unique_ptr<QgsPoint> layerPoint = getLayerPoint( point, true );

  if ( mLastTimeRecorded.addSecs( mLineRecordingInterval ) <= QDateTime::currentDateTime() )
  {
    mLastTimeRecorded = QDateTime::currentDateTime();
    mRecordedPoints.append( *layerPoint.get() );
  }
  else
  {
    if ( !mRecordedPoints.isEmpty() )
    {
      mRecordedPoints.last().setX( layerPoint->x() );
      mRecordedPoints.last().setY( layerPoint->y() );
      mRecordedPoints.last().setZ( layerPoint->z() );
    }
  }
  mRecordingModel->setFeatureLayerPair( lineOrPolygonFeature() );
}


QgsQuickFeatureLayerPair DigitizingController::lineOrPolygonFeature()
{
  if ( !featureLayerPair().layer() )
    return QgsQuickFeatureLayerPair();

  if ( mRecordedPoints.isEmpty() )
    return QgsQuickFeatureLayerPair();

  QgsGeometry geom;
  QgsLineString *linestring = new QgsLineString;
  Q_FOREACH ( const QgsPoint &pt, mRecordedPoints )
    linestring->addVertex( pt );
  if ( hasLineGeometry( featureLayerPair().layer() ) )
  {
    geom = QgsGeometry( linestring );
  }
  else if ( hasPolygonGeometry( featureLayerPair().layer() ) )
  {
    QgsPolygon *polygon = new QgsPolygon();
    polygon->setExteriorRing( linestring );
    geom = QgsGeometry( polygon );
  }

  return createFeatureLayerPair( geom );
}

QgsPoint DigitizingController::pointFeatureMapCoordinates( QgsQuickFeatureLayerPair pair )
{
  if ( !pair.layer() )
    return QgsPoint();

  QgsPointXY res = mMapSettings->mapSettings().layerToMapCoordinates( pair.layer(), QgsPoint( pair.feature().geometry().asPoint() ) );
  return QgsPoint( res );
}

QgsQuickFeatureLayerPair DigitizingController::changePointGeometry( QgsQuickFeatureLayerPair pair, QgsPoint point, bool isGpsPoint )
{
  QgsGeometry geom = getPointGeometry( point, isGpsPoint );
  pair.featureRef().setGeometry( geom );
  return pair;
}

void DigitizingController::addRecordPoint( const QgsPoint &point, bool isGpsPoint )
{
  if ( !mRecording )
    return;

  std::unique_ptr<QgsPoint> layerPoint = getLayerPoint( point, isGpsPoint );
  mRecordedPoints.append( *layerPoint.get() );

  // update geometry so we can use the model for highlight in map
  mRecordingModel->setFeatureLayerPair( lineOrPolygonFeature() );
}

void DigitizingController::removeLastPoint()
{
  if ( mRecordedPoints.isEmpty() )
    return;

  if ( mRecordedPoints.size() == 1 )
  {
    // cancel recording
    mRecording = false;
    emit recordingChanged();

    return;
  }

  mRecordedPoints.removeLast();
  mRecordingModel->setFeatureLayerPair( lineOrPolygonFeature() );
}
