#include "projectsmodel.h"

#include <QString>
#include <QDirIterator>
#include <QDebug>
#include <QDateTime>


// for new project
#include "qgis.h"
#include "qgspointxy.h"
#include <qgsvectorlayer.h>

#include <qgspallabeling.h>
#include <qgsvectorlayerlabeling.h>
#include <qgscategorizedsymbolrenderer.h>
#include <qgsmarkersymbollayer.h>
#include <qgssinglesymbolrenderer.h>

#include <qgsprojectviewsettings.h>
#include "inpututils.h"

ProjectModel::ProjectModel( LocalProjectsManager &localProjects, QObject *parent )
    : QAbstractListModel( parent )
    , mLocalProjects( localProjects )
{
    findProjectFiles();
}

ProjectModel::~ProjectModel() {}



static QString findQgisProjectFile( const QString &projectDir )
{
    QList<QString> foundProjectFiles;
    QDirIterator it( projectDir, QStringList() << QStringLiteral( "*.qgs" ) << QStringLiteral( "*.qgz" ), QDir::Files, QDirIterator::Subdirectories );
    while ( it.hasNext() )
    {
        it.next();
        foundProjectFiles << it.filePath();
    }
    if ( foundProjectFiles.count() == 1 )
    {
        return foundProjectFiles.first();
    }
    return QString();
}


void ProjectModel::findProjectFiles()
{
    // populate from mLocalProjects
    mProjectFiles.clear();


    QList<LocalProjectInfo> projects = mLocalProjects.projects();



    QStringList entryList = QDir( dataDir() ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );


    for ( QString folderName : entryList )
    {
        LocalProjectInfo info;
        info.projectDir = dataDir() + "/" + folderName;
        info.qgisProjectFilePath = findQgisProjectFile( info.projectDir );
        info.projectName = folderName;

        projects << info;
    }


    for ( const LocalProjectInfo &project : projects )
    {
        QDir dir( project.projectDir );
        QFileInfo fi( project.qgisProjectFilePath );

        ProjectFile projectFile;
        projectFile.path = project.qgisProjectFilePath;
        projectFile.folderName = dir.dirName();
        projectFile.projectName = project.projectName;
        projectFile.projectNamespace = project.projectNamespace;
        QDateTime created = fi.created().toUTC();   // TODO: why UTC ???
        if ( !project.qgisProjectFilePath.isEmpty() )
        {
            projectFile.info = QString( created.toString() );
            projectFile.isValid = true;
        }
        else
        {
            projectFile.info = "invalid project";
            projectFile.isValid = false;
        }
        mProjectFiles << projectFile;
    }

    std::sort( mProjectFiles.begin(), mProjectFiles.end() );
}


QVariant ProjectModel::data( const QModelIndex &index, int role ) const
{
    int row = index.row();
    if ( row < 0 || row >= mProjectFiles.count() )
        return QVariant( "" );

    const ProjectFile &projectFile = mProjectFiles.at( row );

    switch ( role )
    {
    case ProjectName: return QVariant( projectFile.projectName );
    case ProjectNamespace: return QVariant( projectFile.projectNamespace );
    case FolderName: return QVariant( projectFile.folderName );
    case Path: return QVariant( projectFile.path );
    case ProjectInfo: return QVariant( projectFile.info );
    case IsValid: return QVariant( projectFile.isValid );
    }
    return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames[ProjectName] = "projectName";
    roleNames[ProjectNamespace] = "projectNamespace";
    roleNames[FolderName] = "folderName";
    roleNames[Path] = "path";
    roleNames[ProjectInfo] = "projectInfo";
    roleNames[IsValid] = "isValid";
    return roleNames;
}

QModelIndex ProjectModel::index( int row, int column, const QModelIndex &parent ) const
{
    Q_UNUSED( column );
    Q_UNUSED( parent );
    return createIndex( row, 0, nullptr );
}

int ProjectModel::rowAccordingPath( QString path ) const
{
    int i = 0;
    for ( ProjectFile prj : mProjectFiles )
    {
        if ( prj.path == path )
        {
            return i;
        }
        i++;
    }
    return -1;
}



void ProjectModel::deleteProjectDirectory(const QString &projectDir)
{
    qDebug() << "projectDir in deleteProjectDirectory: " << projectDir;
    qDebug() << "mProjectfiles count: " << mProjectFiles.count();

    for ( int i = 0; i < mProjectFiles.count(); ++i )
    {
        if ( dataDir() + "/" + mProjectFiles[i].folderName == projectDir )
        {
            Q_ASSERT( !projectDir.isEmpty() && projectDir != "/" );
            QDir( projectDir ).removeRecursively();
            mProjectFiles.removeAt( i );
            return;
        }
    }
}

void ProjectModel::deleteProject( int row )
{
    if ( row < 0 || row >= mProjectFiles.length() )
    {
        return;
    }

    ProjectFile project = mProjectFiles.at( row );

    deleteProjectDirectory( dataDir() + "/" + project.folderName  );
    qDebug() << "project folder path: " << dataDir() + "/" + project.folderName;

    beginResetModel();
    findProjectFiles();
    endResetModel();
}


int ProjectModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return mProjectFiles.count();
}



QString ProjectModel::dataDir() const
{
    return mLocalProjects.dataDir();
}

bool ProjectModel::containsProject( const QString &projectNamespace, const QString &projectName )
{
    return mLocalProjects.hasMerginProject( projectName );
}

void ProjectModel::addProject( QString projectFolder, QString projectName, bool successful )
{
    if ( !successful ) return;

    if ( projectFolder.isEmpty() ) return;

    Q_UNUSED( projectName );
    beginResetModel();
    findProjectFiles();
    endResetModel();
}


// Get project index with ProjectName
int ProjectModel::rowAccordingName(QString name) const
{
    int i = 0;
    for ( ProjectFile prj : mProjectFiles )
    {
        if ( prj.projectName == name )
        {
            return i;
        }
        i++;
    }
    return -1;
}


/*!
  New Project
*/
/*
QString ProjectModel::addNewProject(QString name, QString epsgcode )
{
    QString new_path = dataDir() + "/" + name;

    QDir dir(new_path);
    QgsCoordinateReferenceSystem my_crs = QgsCoordinateReferenceSystem("EPSG:" + epsgcode);

    if ( !(my_crs.isValid()) ) {
        return "crs_not_found";
    }
    else if ( !(dir.exists()) ) {
        dir.mkpath(".");
        QString temp_projectName = name;
        QString layerName = new_path + "/" + temp_projectName;
        //qDebug() << "layer  name path: " << layerName;

        QString projectName = new_path + "/" + name + ".qgs";
        //layerName = new_path + "/" + projectName;
        //qDebug() << "project path is: " << projectName;
        mProject = QgsProject::instance();
        mProject->clear();
        mProject->setFileName(projectName);


        QgsVectorLayer *tempLayer = new QgsVectorLayer( "Point?crs=epsg:" + epsgcode , QStringLiteral( "Survey_Point" ), "memory" );

        QgsCoordinateTransformContext mTransformContext;
        if ( tempLayer )
          {
            mTransformContext = tempLayer->transformContext();
          }

        QString extention = ".gpkg";
        QString ogr_driver = "GPKG";


        QgsVectorFileWriter::SaveVectorOptions mOptions;
        mOptions.driverName = "GPKG";

        // vector layer, layer path, transform context of layer, saveVectorOptions to set driver,
        mError = QgsVectorFileWriter::writeAsVectorFormatV2(tempLayer, layerName + extention, mTransformContext, mOptions );

        if(mError == QgsVectorFileWriter::NoError) {
            qDebug() << "it is ok, no error";
        } else {
            qDebug() << "ERROR! File couldn't created! ";
            return "crs_not_found";
        }


        QgsVectorLayer *temp2 = new QgsVectorLayer(layerName + extention, "Survey_Point", "ogr");

        //QgsVectorFileWriter::writeAsVectorFormat(tempLayer,"Survey_Point_new", "UTF-8", my_crs, "ESRI Shapefile");
        // Create fields instance
        QgsFields fields;
        // Append Name and Description fields to fields
        fields.append(QgsField("Name", QVariant::String, "text", 50));
        fields.append(QgsField("Desc", QVariant::String, "text", 200));
        // Add Attributes of the vector layer
        temp2->dataProvider()->addAttributes(fields.toList());

        // Labelling the layer
        QgsPalLayerSettings layer_settings;
        QgsTextFormat text_format;
        text_format.setFont(QFont("Ubuntu Mono",9));
        text_format.setSize(8.0);

        layer_settings.setFormat(text_format);
        layer_settings.fieldName = "Name";

        //layer_settings.placement = "2";

        temp2->setLabelsEnabled(true);
        temp2->setLabeling(new QgsVectorLayerSimpleLabeling(layer_settings));

        // MARKER SIZE
        QgsMarkerSymbol *symbol = new QgsMarkerSymbol();

        QgsStringMap properties;

        symbol->setSize(0.8);
        symbol->setColor("black");

        temp2->setRenderer( new QgsSingleSymbolRenderer( symbol ) );
        temp2->triggerRepaint();

        mProject->addMapLayer(temp2);

        // Setting up extent of the point layer:
        //temp2->setExtent(QgsRectangle(1000.0, 1000.0, 2000.0, 2000.0));

        qDebug() << "temp2->extent().toString(2) :::::::::::::::::::" << temp2->extent().toString(2);

        mProject->setCrs(my_crs);
        mProject->write(projectName);
        mProject->clear();

        qDebug() << "mProject->fileName(): " << mProject->fileName();
        qDebug() << "mProject->crs().toProj(); " << mProject->crs().toProj();

        return "ok";
    }
    else{
        return "fileExists";
    }
}*/

static QString getDataDir()
{
    QString dataPathRaw("Surveying_Calculator");

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

        QStringList split = QDir::homePath().split( "/" ); // something like /data/user/0/org.project.geoclass/files
        QFileInfo usrDir( "/storage/emulated/" + split[2] + "/" );
        dataPathRaw = usrDir.path() + "/" + dataPathRaw;
        if ( !( usrDir.isDir() && usrDir.isWritable() ) )
        {
            qDebug() << "usrDir: " << usrDir.path() << " not writable";
        }
    }
#endif
    qputenv( "QGIS_QUICK_DATA_PATH", dataPathRaw.toUtf8().constData() );
    QString dataDir = QString::fromLocal8Bit( qgetenv( "QGIS_QUICK_DATA_PATH" ) ) ;
    qDebug() << "QGIS_QUICK_DATA_PATH: " << dataDir;
    return dataDir;
}

void ProjectModel::refreshModel()
{
    beginResetModel();
    findProjectFiles();
    endResetModel();
}
