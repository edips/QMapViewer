#ifndef ACTIVELAYER_H
#define ACTIVELAYER_H

#include <QObject>

#include "qgsmaplayer.h"
#include "qgsvectorlayer.h"

/**
 * @brief The ActiveLayer class holds information about current active layer for recording.
 * It supports setting active layer by layer and offers information about layer.
 *
 * Registered for qml as __activeLayer
 */
class ActiveLayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString layerId READ layerId NOTIFY activeLayerChanged )
    Q_PROPERTY( QString layerName READ layerName NOTIFY activeLayerChanged )
    Q_PROPERTY( QgsMapLayer *layer WRITE setActiveLayer READ layer NOTIFY activeLayerChanged )
    Q_PROPERTY( QgsVectorLayer *vectorLayer READ vectorLayer NOTIFY activeLayerChanged )

  public:
    ActiveLayer();

    //! Functions returning information about active layer
    QString layerId() const;
    QString layerName() const;
    QgsMapLayer *layer() const;
    QgsVectorLayer *vectorLayer() const;

    /**
     * Updates active layer.
     * \param layerName Represents layer to be set as active.
     */
    void setActiveLayer( QgsMapLayer *layer );

  signals:

    /**
     * @brief activeLayerChanged signal emitted when layer is changed
     * @param layerName holds name of the new active layer, empty if active layer is null
     */
    void activeLayerChanged( const QString &layerName );

  public slots:
    /**
     * Resets active layer to nullptr.
     */
    void resetActiveLayer();

  private:
    QgsMapLayer *mLayer;
};

#endif // ACTIVELAYER_H
