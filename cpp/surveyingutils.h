#ifndef SURVEYINGUTILS_H
#define SURVEYINGUTILS_H
#include <QString>
#include <QObject>
#include "qgsproject.h"
#include "qgsquickmapsettings.h"
#include "projectsmodel.h"
#include "qgsquickfeaturelayerpair.h"
#include "cpp/appsettings.h"

class SurveyingUtils: public QObject
{
    Q_OBJECT
public:
    SurveyingUtils( QObject *parent = nullptr );

    // By Edip
    Q_INVOKABLE long epsg_code();
    Q_INVOKABLE QString epsg_name();
    Q_INVOKABLE bool isGeographic();
    Q_INVOKABLE bool crsValid();
    Q_INVOKABLE QString homePath();

    // Transfornm CRS, used in CoordinateConvertor app
    Q_INVOKABLE QString transformer(QString x, QString y, QString src, QString dst);
    // projdef is valid?
    Q_INVOKABLE bool projDefValid(QString projDef);
    // If a project exists or not
    Q_INVOKABLE bool no_project();
    Q_INVOKABLE bool featureIsPoint( QgsQuickFeatureLayerPair p );
    Q_INVOKABLE bool featureIsLine( QgsQuickFeatureLayerPair p );
    Q_INVOKABLE bool featureIsPolygon( QgsQuickFeatureLayerPair p );
    Q_INVOKABLE QString getArea( QgsQuickFeatureLayerPair p );
    Q_INVOKABLE QString getLength( QgsQuickFeatureLayerPair p);
    

private:
    QgsProject *mProject = nullptr;
    QgsQuickMapSettings *mMapSettings = nullptr;
    //LocalProjectsManager &mLocalProjects;
};

#endif // SURVEYINGUTILS_H
