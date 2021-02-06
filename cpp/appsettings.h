#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QHash>

class AppSettings: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString defaultProject READ defaultProject WRITE setDefaultProject NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString activeProject READ activeProject WRITE setActiveProject NOTIFY activeProjectChanged )
    Q_PROPERTY( QString defaultProjectName READ defaultProjectName NOTIFY defaultProjectChanged )
    Q_PROPERTY( QString defaultLayer READ defaultLayer WRITE setDefaultLayer NOTIFY defaultLayerChanged )
    Q_PROPERTY( bool autoCenterMapChecked READ autoCenterMapChecked WRITE setAutoCenterMapChecked NOTIFY autoCenterMapCheckedChanged )
    Q_PROPERTY( int lineRecordingInterval READ lineRecordingInterval WRITE setLineRecordingInterval NOTIFY lineRecordingIntervalChanged )
    Q_PROPERTY( int gpsAccuracyTolerance READ gpsAccuracyTolerance WRITE setGpsAccuracyTolerance NOTIFY gpsAccuracyToleranceChanged )

    Q_PROPERTY( int scaleUnit READ scaleUnit WRITE setScaleUnit NOTIFY scaleUnitChanged)

public:
    explicit AppSettings( QObject *parent = nullptr );

    QString defaultProject() const;
    void setDefaultProject( const QString &value );

    QString defaultProjectName() const;

    QString activeProject() const;
    void setActiveProject( const QString &value );

    QString defaultLayer() const;
    void setDefaultLayer( const QString &value );

    bool autoCenterMapChecked();
    void setAutoCenterMapChecked( const bool value );

    int gpsAccuracyTolerance() const;
    void setGpsAccuracyTolerance( int gpsAccuracyTolerance );

    int lineRecordingInterval() const;
    void setLineRecordingInterval( int lineRecordingInterval );

    // scale unit
    int scaleUnit() const;
    void setScaleUnit( int value );

signals:
    void defaultProjectChanged();
    void activeProjectChanged();
    void defaultLayerChanged();
    void autoCenterMapCheckedChanged();
    void gpsAccuracyToleranceChanged();
    void lineRecordingIntervalChanged();
    void scaleUnitChanged();

private:
    // Projects path
    QString mDefaultProject;
    // Path to active project
    QString mActiveProject;
    // flag for following GPS position
    bool mAutoCenterMapChecked = false;
    // used in GPS signal indicator
    int mGpsAccuracyTolerance = -1;
    // Digitizing period in seconds
    int mLineRecordingInterval = 3;

    // Projects path -> defaultLayer name
    QHash<QString, QString> mDefaultLayers;

    const QString mGroupName = QString( "inputApp" );

    // scale unit
    int mScaleUnit;
};

#endif // APPSETTINGS_H
