#ifndef LOCALPROJECTSMANAGER_H
#define LOCALPROJECTSMANAGER_H

#include <QObject>


enum ProjectStatus
{
  NoVersion,  //!< the project is not available locally
  UpToDate,   //!< both server and local copy are in sync with no extra modifications
  OutOfDate,  //!< server has newer version than what is available locally (but the project is not modified locally)
  Modified    //!< there are some local modifications in the project that need to be pushed (note: also server may have newer version)
};
Q_ENUMS( ProjectStatus )


//! Summary information about a local project
struct LocalProjectInfo
{
  bool isValid() const { return !projectDir.isEmpty(); }

  QString projectDir;  //!< full path to the project directory

  QString qgisProjectFilePath;  //!< path to the .qgs/.qgz file (or empty if not have exactly one such file)

  QString projectName;
  QString projectNamespace;

  int localVersion = -1;  //!< the project version that is currently available locally
  int serverVersion = -1;  //!< the project version most recently seen on server (may be -1 if no info from server is available)

  ProjectStatus status = NoVersion;

  // Sync status (e.g. progress) is not kept here because if a project does not exist locally yet
  // and it is only being downloaded for the first time, it's not in the list of local projects either
  // and we would need to do some workarounds for that.
};


class LocalProjectsManager : public QObject
{
    Q_OBJECT
  public:
    explicit LocalProjectsManager( const QString &dataDir );

    QString dataDir() const { return mDataDir; }

    QList<LocalProjectInfo> projects() const { return mProjects; }

    LocalProjectInfo projectFromDirectory( const QString &projectDir ) const;

    LocalProjectInfo projectFromMerginName( const QString &projectFullName ) const;
    bool hasMerginProject( const QString &projectFullName ) const;

    void updateProjectStatus( const QString &projectDir );

    //! Should forget about that project (it has been removed already)
    void removeProject( const QString &projectDir );

    //! Recursively removes project's directory (only when it exists in the list)
    void deleteProjectDirectory( const QString &projectDir );


    static ProjectStatus currentProjectStatus( const LocalProjectInfo &project );

  signals:
    void projectMetadataChanged( const QString &projectDir );
    void localProjectAdded( const QString &projectDir );
    void localProjectRemoved( const QString &projectDir );

  private:
    void updateProjectStatus( LocalProjectInfo &project );

  private:
    QString mDataDir;   //!< directory with all local projects
    QList<LocalProjectInfo> mProjects;
};


#endif // LOCALPROJECTSMANAGER_H
