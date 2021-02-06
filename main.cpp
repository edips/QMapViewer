#define STR1(x)  #x
#define STR(x)  STR1(x)
#include <QtQml>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QtDebug>
#include <QQmlError>
#include <QQmlContext>
#include <QQuickWindow>
#include <qqml.h>
#include <qgsmessagelog.h>
#include "cpp/androidutils.h"
#include <QDesktopWidget>
#include "qgsconfig.h"
#include "cpp/inpututils.h"
#include "cpp/projectsmodel.h"
#include "cpp/layersmodel.h"
#include "cpp/digitizingcontroller.h"
#include "cpp/activelayer.h"

#include "qgsquickutils.h"
#include "qgsproject.h"
#include "qgsproviderregistry.h"

#include "cpp/mapthemesmodel.h"

#ifndef NDEBUG
#include <QQmlDebuggingEnabler>
#endif

#ifdef ANDROID
#include <QCommandLineParser>
#include <qgis.h>
#include <QFile>
#include <QDir>
#include <QtAndroidExtras>
#include "./QtAndroidTools/QtAndroidTools.h"
#endif

#include "qgsapplication.h"
#include "cpp/loader.h"
#include "cpp/appsettings.h"
#include "cpp/surveyingutils.h"
#include "cpp/layersproxymodel.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QUrl>
#include <QFontDatabase>
#include <QQuickStyle>

/*! TODO
 * Move functions a new class named InitProject
 * Try to remove VariablesManager.cpp
 * Combine InputUtils with SurveyingUtils
*/


static QString getDataDir()
{
#ifdef QGIS_QUICK_DATA_PATH
    QString dataPathRaw( STR( QGIS_QUICK_DATA_PATH ) );

#ifdef ANDROID
    QFileInfo extDir( "/sdcard/" );
    if ( extDir.isDir() && extDir.isWritable() )
    {
        // seems that this directory transposes to the latter one in case there is no sdcard attached
        dataPathRaw = extDir.path() + "/" + dataPathRaw;
    }
    else
    {
        qDebug() << "extDir: " << extDir.path() << " not writable";

        QStringList split = QDir::homePath().split( "/" ); // something like /data/user/0/uk.co.lutraconsulting/files
        // TODO support active user from QDir::homePath()
        QFileInfo usrDir( "/storage/emulated/" + split[2] + "/" );
        dataPathRaw = usrDir.path() + "/" + dataPathRaw;
        if ( !( usrDir.isDir() && usrDir.isWritable() ) )
        {
            qDebug() << "usrDir: " << usrDir.path() << " not writable";
        }
    }
#endif

    qputenv( "QGIS_QUICK_DATA_PATH", dataPathRaw.toUtf8().constData() );
#else
    qDebug( "== Must set QGIS_QUICK_DATA_PATH in order to get QGIS Quick running! ==" );
#endif
    QString dataDir = QString::fromLocal8Bit( qgetenv( "QGIS_QUICK_DATA_PATH" ) ) ;
    qDebug() << "QGIS_QUICK_DATA_PATH: " << dataDir;
    return dataDir;
}

static void setEnvironmentQgisPrefixPath()
{
#ifdef DESKTOP_OS
#ifdef QGIS_PREFIX_PATH
    // it is very important to set#include "./QtAndroidTools/QtAndroidTools.h" Prefix path, elsewhere we will get QgsQuick not installed error
    qputenv( "QGIS_PREFIX_PATH", STR( QGIS_PREFIX_PATH ) );
#endif
    if ( QString::fromLocal8Bit( qgetenv( "QGIS_PREFIX_PATH" ) ).isEmpty() )
    {
        // if not on Android, QGIS_PREFIX_PATH env variable should have been set already or defined as C++ define
        qDebug( "== Must set QGIS_PREFIX_PATH in order to get QGIS Quick module running! ==" );
    }
#endif

#if defined (ANDROID) || defined (Q_OS_IOS)
    QDir myDir( QDir::homePath() );
    myDir.cdUp();
    QString prefixPath = myDir.absolutePath();  // something like: /data/data/org.qgis.quick
    qputenv( "QGIS_PREFIX_PATH", prefixPath.toUtf8().constData() );
#elif defined (Q_OS_WIN32)
    QString prefixPath = QCoreApplication::applicationDirPath();
    qputenv( "QGIS_PREFIX_PATH", prefixPath.toUtf8().constData() );
#endif

    qDebug() << "QGIS_PREFIX_PATH: " << QString::fromLocal8Bit( qgetenv( "QGIS_PREFIX_PATH" ) );
}

static void init_qgis( const QString &pkgPath )
{
    QgsApplication::init();

#ifdef MOBILE_OS
    // QGIS plugins on Android are in the same path as other libraries
    QgsApplication::setPluginPath( QApplication::applicationDirPath() );
    QgsApplication::setPkgDataPath( pkgPath );
#else
    Q_UNUSED( pkgPath )
#endif

    QgsApplication::initQgis();

    // make sure the DB exists - otherwise custom projections will be failing
    if ( !QgsApplication::createDatabase() )
        qDebug( "Can't create qgis user DB!!!" );

    qDebug( "qgis providers:\n%s", QgsProviderRegistry::instance()->pluginList().toLatin1().data() );
}

static void init_proj( const QString &pkgPath )
{
#ifdef MOBILE_OS
#ifdef ANDROID
    // win and ios resources are already in the bundle
    InputUtils::cpDir( "assets:/qgis-data", pkgPath );
    QString prefixPath = pkgPath + "/proj";
    QString projFilePath = prefixPath + "/proj.db";
#endif

#ifdef Q_OS_IOS
    QString prefixPath = pkgPath + "/proj";
    QString projFilePath = prefixPath + "/proj.db";
#endif

#ifdef Q_OS_WIN32
    QString prefixPath = pkgPath + "\\proj";
    QString projFilePath = prefixPath + "\\proj.db";
#endif

    qDebug( "PROJ6 resources: %s", prefixPath.toLatin1().data() );
    QFile projdb( projFilePath );
    if ( projdb.exists() )
    {
        qputenv( "PROJ_LIB", prefixPath.toUtf8().constData() );
    }
    else
    {
        InputUtils::log( QStringLiteral( "PROJ6 error" ), QStringLiteral( "The Input has failed to load PROJ6 database." ) );
    }

#else
    // proj share lib is set from the proj installation on the desktop,
    // so it should work without any modifications.
    // to test check QgsProjUtils.searchPaths() in QGIS Python Console
    Q_UNUSED( pkgPath )
#endif

}

void initDeclarative()
{
    qmlRegisterUncreatableType<ProjectModel>( "lc", 1, 0, "ProjectModel", "" );
    qmlRegisterUncreatableType<LayersModel>( "lc", 1, 0, "LayersModel", "" );
    qmlRegisterUncreatableType<MapThemesModel>( "lc", 1, 0, "MapThemesModel", "" );
    qmlRegisterUncreatableType<Loader>( "lc", 1, 0, "Loader", "" );
    qmlRegisterUncreatableType<AppSettings>( "lc", 1, 0, "AppSettings", "" );
    qmlRegisterType<DigitizingController>( "lc", 1, 0, "DigitizingController" );
    qmlRegisterType<SurveyingUtils>( "lc", 1, 0, "SurveyingUtils" );
    qmlRegisterUncreatableType<LayersProxyModel>( "lc", 1, 0, "LayersProxyModel", "" );
    qmlRegisterUncreatableType<ActiveLayer>( "lc", 1, 0, "ActiveLayer", "" );
}

void addQmlImportPath( QQmlEngine &engine )
{
    // This adds a runtime qml directory containing QgsQuick plugin
    // when Input is installed (e.g. Android/Win32)
    engine.addImportPath( QgsApplication::qmlImportPath() );
    qDebug() << "adding QML import Path: " << QgsApplication::qmlImportPath();

#ifdef QML_BUILD_IMPORT_DIR
    // Adds a runtime qml directory containing QgsQuick plugin
    // if we are using the developer mode (not installed Input)
    // e.g. Linux/MacOS
    QString qmlBuildImportPath( STR( QML_BUILD_IMPORT_DIR ) );
    engine.addImportPath( qmlBuildImportPath );
    qDebug() << "adding QML import Path: " << qmlBuildImportPath;
#endif

#ifdef Q_OS_IOS
    // REQUIRED FOR IOS - to load QgsQuick/*.qml files defined in qmldir
    engine.addImportPath( "qrc:///" );
    qDebug() << "adding QML import Path: " << "qrc:///";
#endif
}

quint64 dir_size(const QString & str)
{
    quint64 sizex = 0;
    QFileInfo str_info(str);
    if (str_info.isDir())
    {
        QDir dir(str);
        QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs |  QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        for (int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list.at(i);
            if(fileInfo.isDir())
            {
                    sizex += dir_size(fileInfo.absoluteFilePath());
            }
            else
                sizex += fileInfo.size();

        }
    }
    return sizex;
}

void clearDir( const QString path )
{
    QDir dir( path );

    dir.setFilter( QDir::NoDotAndDotDot | QDir::Files );
    foreach( QString dirItem, dir.entryList() )
        dir.remove( dirItem );

    dir.setFilter( QDir::NoDotAndDotDot | QDir::Dirs );
    foreach( QString dirItem, dir.entryList() )
    {
        QDir subDir( dir.absoluteFilePath( dirItem ) );
        subDir.removeRecursively();
        qDebug() << dir.entryList().length();
    }
}

// main cpp
int main(int argc, char *argv[])
{
    // if size of cache or app data is more than 10 MB, set it to zero
    int cache_size = dir_size( QStandardPaths::writableLocation( QStandardPaths::CacheLocation ) );
    int data_size = dir_size( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) );

    if( data_size > 10000000 ) {
        qputenv("QML_DISABLE_DISK_CACHE", "true");
    }

    if( cache_size > 10000000 ) {
        QString path = QStandardPaths::writableLocation( QStandardPaths::CacheLocation );
        clearDir( path );
    }
    // High DPI
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle("Universal");

    QgsApplication app( argc, argv, true );

    QQmlApplicationEngine engine;

    // Description of the app for making settings work in QML
    QCoreApplication::setOrganizationName("Geosoft");
    QCoreApplication::setOrganizationDomain("qmapviewer.org");
    QCoreApplication::setApplicationName("QMapViewer");
    QCoreApplication::setApplicationVersion( "9" );

    qDebug() << "Built with QGIS version " << VERSION_INT;
    // Add fonts to fontDatabase
    QFontDatabase::addApplicationFont(":/assets/fonts/Lato-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Lato-Bold.ttf");

    // Require permissions before accessing data folder
#ifdef ANDROID
    AndroidUtils::requirePermissions();
    QtAndroidTools::initializeQmlTools();
#endif
    // Set/Get enviroment
    QString dataDir = getDataDir( );
    QString projectDir = dataDir + "/projects";
    QDir projectDirr ( projectDir );
    qDebug() << "project dir is " << projectDirr.path();
    qDebug() << "project dir exists? " << projectDirr.exists();
    if ( !projectDirr.exists() )
    {
      projectDirr.mkpath( projectDir );
    }
    qDebug() << "project dir is " << projectDirr.path();
    qDebug() << "project dir exists? " << projectDirr.exists();
    InputUtils::setLogFilename( projectDir + "/.logs" );
    setEnvironmentQgisPrefixPath();

    QString appBundleDir;
#ifdef ANDROID
    appBundleDir = dataDir + "/qgis-data";
#endif

    init_proj( appBundleDir );
    init_qgis( appBundleDir );

    // Create classes
    AndroidUtils au;
    LocalProjectsManager localProjects( projectDir );
    ProjectModel pm( localProjects );
    MapThemesModel mtm;
    AppSettings as;
    InputUtils iu;
    LayersModel lm;
    LayersProxyModel recordingLpm( &lm, ModelTypes::ActiveLayerSelection );
    ActiveLayer al;
    Loader loader( mtm, as, al );

    SurveyingUtils surv;

    QObject::connect( &app, &QGuiApplication::applicationStateChanged, &loader, &Loader::appStateChanged );

    QFile projectLoadingFile( Loader::LOADING_FLAG_FILE_PATH );
    if ( projectLoadingFile.exists() )
    {
        // Cleaning default project due to a project loading has crashed during the last run.
        as.setDefaultProject( QString() );
        projectLoadingFile.remove();
        InputUtils::log( QStringLiteral( "Loading project error" ), QStringLiteral( "The Input has been unexpectedly finished during the last run." ) );
    }

    engine.addImportPath( QgsApplication::qmlImportPath() );
    initDeclarative();

    // QGIS environment variables to set
    // OGR_SQLITE_JOURNAL is set to DELETE to avoid working with WAL files
    // and properly close connection after writting changes to gpkg.
    ::setenv( "OGR_SQLITE_JOURNAL", "DELETE", 1 );

    // Register to QQmlEngine
    engine.rootContext()->setContextProperty( "__androidUtils", &au );
    engine.rootContext()->setContextProperty( "__inputUtils", &iu );
    engine.rootContext()->setContextProperty( "__projectsModel", &pm );
    engine.rootContext()->setContextProperty( "__loader", &loader );
    engine.rootContext()->setContextProperty( "__layersModel", &lm );
    engine.rootContext()->setContextProperty( "__appSettings", &as );
    engine.rootContext()->setContextProperty( "__mapThemesModel", &mtm );
    engine.rootContext()->setContextProperty("__surveyingUtils", &surv );
    engine.rootContext()->setContextProperty( "__recordingLayersModel", &recordingLpm );

    engine.rootContext()->setContextProperty( "__activeLayer", &al );

    engine.load(QUrl(QStringLiteral("qrc:///qml/main.qml")) );

#ifndef ANDROID
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.process( app );
#endif
    return app.exec();
}
