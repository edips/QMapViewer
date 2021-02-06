#ifndef VARIABLESMANAGER_H
#define VARIABLESMANAGER_H

#include <QObject>
#include "qgsproject.h"

/*
 * The class sets global and project variables related to Mergin.
 */
class VariablesManager : public QObject
{
  public:
    VariablesManager( QObject *parent = nullptr );

  private:
    QgsProject *mCurrentProject = nullptr;

    void setProjectVariables();
};

#endif // VARIABLESMANAGER_H
