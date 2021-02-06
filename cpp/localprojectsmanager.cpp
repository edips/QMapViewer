#include "localprojectsmanager.h"
#include <QDir>
#include <QDirIterator>
#include <QDebug>

LocalProjectsManager::LocalProjectsManager( const QString &dataDir )
    : mDataDir( dataDir )
{
}

LocalProjectInfo LocalProjectsManager::projectFromDirectory( const QString &projectDir ) const
{
    for ( const LocalProjectInfo &info : mProjects )
    {
        if ( info.projectDir == projectDir )
            return info;
    }
    return LocalProjectInfo();
}

LocalProjectInfo LocalProjectsManager::projectFromMerginName( const QString &projectFullName ) const
{
  return LocalProjectInfo();
}

bool LocalProjectsManager::hasMerginProject( const QString &projectFullName ) const
{
  return projectFromMerginName( projectFullName ).isValid();
}

void LocalProjectsManager::updateProjectStatus( const QString &projectDir )
{
    for ( LocalProjectInfo &info : mProjects )
    {
        if ( info.projectDir == projectDir )
        {
            updateProjectStatus( info );
            return;
        }
    }
    Q_ASSERT( false );  // should not happen
}

void LocalProjectsManager::removeProject( const QString &projectDir )
{
    for ( int i = 0; i < mProjects.count(); ++i )
    {
        if ( mProjects[i].projectDir == projectDir )
        {
            mProjects.removeAt( i );
            emit localProjectRemoved( projectDir );
            return;
        }
    }
}

void LocalProjectsManager::deleteProjectDirectory( const QString &projectDir )
{
    qDebug() << "projectDir in LocalProjectManager: " << projectDir;

    for ( int i = 0; i < mProjects.count(); ++i )
    {
        if ( mProjects[i].projectDir == projectDir )
        {
            Q_ASSERT( !projectDir.isEmpty() && projectDir != "/" );
            QDir( projectDir ).removeRecursively();
            mProjects.removeAt( i );
            return;
        }
    }
}

static int _getProjectFilesCount( const QString &path )
{
    int count = 0;
    QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
    while ( it.hasNext() )
    {
        it.next();
    }
    return count;
}

ProjectStatus LocalProjectsManager::currentProjectStatus( const LocalProjectInfo &project )
{
    // There was no sync yet
    if ( project.localVersion < 0 )
    {
        return ProjectStatus::NoVersion;
    }

    //
    // TODO: this check for local modifications should be revisited
    //

    // Version is lower than latest one, last sync also before updated
    if ( project.localVersion < project.serverVersion )
    {
        return ProjectStatus::OutOfDate;
    }

    return ProjectStatus::UpToDate;
}

void LocalProjectsManager::updateProjectStatus( LocalProjectInfo &project )
{
    ProjectStatus newStatus = currentProjectStatus( project );
    if ( newStatus != project.status )
    {
        project.status = newStatus;
        emit projectMetadataChanged( project.projectDir );
    }
}
