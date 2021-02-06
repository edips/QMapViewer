#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT

  public:
    explicit Settings( QObject *parent = nullptr );

    Q_INVOKABLE void setValue( const QString &key, const QVariant &value );

    Q_INVOKABLE QVariant value( const QString &key, const QVariant &defaultValue );

    /**
     * Properly evaluates the returned value to be boolean.
     * If the normal value() is used instead, a string "true" or "false"
     * will be returned which will be evaluated to true either way by JS.
     */
    Q_INVOKABLE bool valueBool( const QString &key, bool defaultValue );

  signals:
    void settingChanged( const QString &key );
};

#endif // SETTINGS_H
