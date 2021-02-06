#ifndef LAYERSPROXYMODEL_H
#define LAYERSPROXYMODEL_H

#include <QObject>

#include "qgsmaplayer.h"
#include "qgsmaplayerproxymodel.h"

#include "layersmodel.h"

enum ModelTypes
{
  ActiveLayerSelection,
  BrowseDataLayerSelection,
  AllLayers
};

class LayersProxyModel : public QgsMapLayerProxyModel
{
    Q_OBJECT

  public:
    LayersProxyModel( LayersModel *model, ModelTypes modelType = ModelTypes::AllLayers );

    bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;

    //! Helper methods that convert layer to/from index/name
    Q_INVOKABLE QModelIndex indexFromLayerId( QString layerId ) const;
    Q_INVOKABLE QgsVectorLayer *layerFromLayerId( QString layerId ) const;

    //! Helper method to get data from source model to skip converting indexes
    Q_INVOKABLE QVariant getData( QModelIndex index, int role ) const;

    //! Returns first layer from proxy model's layers list (filtered with filter function)
    Q_INVOKABLE QgsMapLayer *firstUsableLayer() const;

    /**
     * @brief layers method return layers from source model filtered with filter function
     */
    QList<QgsMapLayer *> layers() const;

  public slots:
    void onMapThemeChanged();

  private:

    //! returns if input layer is capable of recording new features
    bool recordingAllowed( QgsMapLayer *layer ) const;

    //! filters if input layer should be visible for browsing
    bool browsingAllowed( QgsMapLayer *layer ) const;

    //! filters if input layer is visible in current map theme
    bool layerVisible( QgsMapLayer *layer ) const;

    ModelTypes mModelType;
    LayersModel *mModel;

    /**
     * @brief filterFunction method takes layer and outputs if \bold this model type accepts layer.
     * Returns true for proxy model built without specific type or AllLayers type.
     *
     * In future will allow dependency injection of custom filter functions.
     */
    std::function<bool( QgsMapLayer * )> filterFunction;
};

#endif // LAYERSPROXYMODEL_H
