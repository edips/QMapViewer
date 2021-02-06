#ifndef MAPPROVIDER_H
#define MAPPROVIDER_H

#include <QObject>


class mapProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString mapType MEMBER m_mapType NOTIFY mapTypeChanged)
public:
    explicit mapProvider(QObject *parent = nullptr);
private:
    QString m_mapType;
signals:
    void mapTypeChanged(QString newValue);
public slots:
    void setMapType(QString mapType);
};

#endif // MAPPROVIDER_H
