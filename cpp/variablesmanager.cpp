#include "variablesmanager.h"

#include "qgsexpressioncontextutils.h"

VariablesManager::VariablesManager( QObject *parent )
  : QObject( parent )
{
}


void VariablesManager::setProjectVariables()
{
  if ( !mCurrentProject )
    return;


  QString filePath = mCurrentProject->fileName();
}
