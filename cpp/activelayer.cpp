#include "activelayer.h"

#include "qgsproject.h"
#include "qgslayertree.h"
#include "qgslayertreelayer.h"

ActiveLayer::ActiveLayer() :
    mLayer( nullptr )
{
}

QgsMapLayer *ActiveLayer::layer() const
{
    return mLayer;
}

QgsVectorLayer *ActiveLayer::vectorLayer() const
{
    if ( mLayer )
        return qobject_cast<QgsVectorLayer *>( mLayer );

    return nullptr;
}

QString ActiveLayer::layerId() const
{
    if ( mLayer )
        return mLayer->id();

    return QString();
}

QString ActiveLayer::layerName() const
{
    if ( mLayer )
        return mLayer->name();

    return QString();
}

void ActiveLayer::setActiveLayer( QgsMapLayer *layer )
{
    if ( !layer )
        return resetActiveLayer();

    if ( !mLayer || !mLayer->isValid() || layer->id() != mLayer->id() )
    {
        mLayer = layer;
        emit activeLayerChanged( layerName() );
    }
}

void ActiveLayer::resetActiveLayer()
{
    // mLayer can be already invalid pointer
    emit activeLayerChanged( QString() );	  // as a leftover from unloaded QGIS project
    void * lp = static_cast<void*>(mLayer);
    if ( lp != nullptr ) {
        mLayer = nullptr;
        emit activeLayerChanged( QString() );
    }
}
