#include "mapprovider.h"

mapProvider::mapProvider(QObject *parent):
    QObject(parent)
{

}

//---------------------------------------------------------------
void mapProvider::setMapType(QString mapProvider) {
        m_mapType = mapProvider;
        mapTypeChanged(mapProvider);
}
