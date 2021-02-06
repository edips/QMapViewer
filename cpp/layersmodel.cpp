#include "layersmodel.h"
#include "qgsquickmapsettings.h"
#include "qgsproject.h"

#include "qgslayertree.h"
#include "qgslayertreelayer.h"
#include "qgslayertreegroup.h"


LayersModel::LayersModel()
{
}

QVariant LayersModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if ( role < LayerNameRole ) // if requested role from parent
        return QgsMapLayerModel::data( index, role );

    QgsMapLayer *layer = layerFromIndex( index );
    if ( !layer || !layer->isValid() ) return QVariant();

    QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );

    switch ( role )
    {
    case LayerNameRole: return layer->name();
    case VectorLayerRole: return vectorLayer ? QVariant::fromValue<QgsVectorLayer *>( vectorLayer ) : QVariant();
    case HasGeometryRole: return vectorLayer ? vectorLayer->wkbType() != QgsWkbTypes::NoGeometry && vectorLayer->wkbType() != QgsWkbTypes::Type::Unknown : QVariant();
    case HasPointGeometry:
    {
        QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );
        return vectorLayer && vectorLayer->geometryType() == QgsWkbTypes::PointGeometry;

    }
    case IconSourceRole:
    {
        if ( vectorLayer )
        {
            QgsWkbTypes::GeometryType type = vectorLayer->geometryType();
            switch ( type )
            {
            case QgsWkbTypes::GeometryType::PointGeometry: return "qrc:/assets/icons/mIconPointLayer.svg";
            case QgsWkbTypes::GeometryType::LineGeometry: return "qrc:/assets/icons/mIconLineLayer.svg";
            case QgsWkbTypes::GeometryType::PolygonGeometry: return "qrc:/assets/icons/mIconPolygonLayer.svg";

            case QgsWkbTypes::GeometryType::UnknownGeometry: // fall through
            case QgsWkbTypes::GeometryType::NullGeometry: return "qrc:/assets/icons/mIconTableLayer.svg";
            }
            return QVariant();
        }
        else return "qrc:/assets/icons/mIconRaster.svg";
    }
    case LayerIdRole: return layer->id();
    case LayerCheckedRole: {
        //mProject = QgsProject::instance();
        QgsLayerTree *root = mProject->instance()->layerTreeRoot();
        return root->findLayer( layer->id() )->isVisible();
    }

    }
    return QVariant();
}

QHash<int, QByteArray> LayersModel::roleNames() const
{
    QHash<int, QByteArray> roles = QgsMapLayerModel::roleNames();
    roles[LayerNameRole] = QStringLiteral( "layerName" ).toLatin1();
    roles[IconSourceRole] = QStringLiteral( "iconSource" ).toLatin1();
    roles[HasGeometryRole] = QStringLiteral( "hasGeometry" ).toLatin1();
    roles[VectorLayerRole] = QStringLiteral( "vectorLayer" ).toLatin1();
    roles[LayerIdRole] = QStringLiteral( "layerId" ).toLatin1();
    roles[HasPointGeometry] = QStringLiteral( "hasPointGeometry" ).toLatin1();
    roles[LayerCheckedRole] = QStringLiteral( "layerChecked" ).toLatin1();
    return roles;
}

/** TODO
 * feature id should be automatically incremented. Else it can cause big problems when deleting s feature or when processing it.
 *
 * create point
 *
 * zoom to point feature
 *
 * active record and open feature panel to edit features
*/
// Add Points action: add feature from user's input and coordinates.
QgsPoint LayersModel::addFeatureSurvey( QString n_str, QString e_str )
{
    double n = n_str.toDouble();
    double e = e_str.toDouble();

    QgsPoint mypoint = QgsPoint(e, n, 0);

    if( mypoint.isEmpty() )
        return QgsPoint();

    return mypoint;
}

bool LayersModel::pointIsEmpty( QgsPoint p )
{
    if( p.x() == 0.0 && p.y() == 0.0 )
        return false;
    return p.isEmpty();
}




