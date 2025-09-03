#ifndef AUTOSTART_H
#define AUTOSTART_H

#include <QObject>

class AutoStart : public QObject
{
    Q_OBJECT

public:
    explicit AutoStart(QObject *parent = nullptr);
    
    static bool isAutoStartEnabled();
    static bool setAutoStartEnabled(bool enabled);
    static QString getAutoStartKey();

private:
    static QString getApplicationPath();
    
#ifdef Q_OS_WIN
    static bool setWindowsAutoStart(bool enabled);
#elif defined(Q_OS_MACOS)
    static bool setMacAutoStart(bool enabled);
#else
    static bool setLinuxAutoStart(bool enabled);
#endif
};

#endif // AUTOSTART_H