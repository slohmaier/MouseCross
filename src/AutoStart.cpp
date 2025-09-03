#include "AutoStart.h"
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#ifdef Q_OS_WIN
#include <QSettings>
#include <windows.h>
#elif defined(Q_OS_MACOS)
#include <QProcess>
#include <QFile>
#include <QTextStream>
#elif defined(Q_OS_LINUX)
#include <QFile>
#include <QTextStream>
#include <QDir>
#endif

AutoStart::AutoStart(QObject *parent)
    : QObject(parent)
{
}

QString AutoStart::getAutoStartKey()
{
    return "MouseCross";
}

QString AutoStart::getApplicationPath()
{
    return QApplication::applicationFilePath();
}

bool AutoStart::isAutoStartEnabled()
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
                      QSettings::NativeFormat);
    return settings.contains(getAutoStartKey());
#elif defined(Q_OS_MACOS)
    QString plistPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + 
                       "/Library/LaunchAgents/com.mousecross.plist";
    return QFile::exists(plistPath);
#else
    QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + 
                           "/autostart/MouseCross.desktop";
    return QFile::exists(autostartPath);
#endif
}

bool AutoStart::setAutoStartEnabled(bool enabled)
{
#ifdef Q_OS_WIN
    return setWindowsAutoStart(enabled);
#elif defined(Q_OS_MACOS)
    return setMacAutoStart(enabled);
#else
    return setLinuxAutoStart(enabled);
#endif
}

#ifdef Q_OS_WIN
bool AutoStart::setWindowsAutoStart(bool enabled)
{
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
                      QSettings::NativeFormat);
    
    if (enabled) {
        QString appPath = getApplicationPath();
        settings.setValue(getAutoStartKey(), QDir::toNativeSeparators(appPath));
    } else {
        settings.remove(getAutoStartKey());
    }
    
    settings.sync();
    return settings.status() == QSettings::NoError;
}
#endif

#ifdef Q_OS_MACOS
bool AutoStart::setMacAutoStart(bool enabled)
{
    QString launchAgentsPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + 
                              "/Library/LaunchAgents";
    QString plistPath = launchAgentsPath + "/com.mousecross.plist";
    
    if (enabled) {
        // Create LaunchAgents directory if it doesn't exist
        QDir().mkpath(launchAgentsPath);
        
        QFile plistFile(plistPath);
        if (plistFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&plistFile);
            out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            out << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
            out << "<plist version=\"1.0\">\n";
            out << "<dict>\n";
            out << "    <key>Label</key>\n";
            out << "    <string>com.mousecross</string>\n";
            out << "    <key>ProgramArguments</key>\n";
            out << "    <array>\n";
            out << "        <string>" << getApplicationPath() << "</string>\n";
            out << "    </array>\n";
            out << "    <key>RunAtLoad</key>\n";
            out << "    <true/>\n";
            out << "</dict>\n";
            out << "</plist>\n";
            plistFile.close();
            
            // Load the plist
            QProcess::execute("launchctl", QStringList() << "load" << plistPath);
            return true;
        }
    } else {
        // Unload and remove the plist
        QProcess::execute("launchctl", QStringList() << "unload" << plistPath);
        return QFile::remove(plistPath);
    }
    
    return false;
}
#endif

#ifdef Q_OS_LINUX
bool AutoStart::setLinuxAutoStart(bool enabled)
{
    QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + 
                           "/autostart";
    QString desktopFilePath = autostartPath + "/MouseCross.desktop";
    
    if (enabled) {
        // Create autostart directory if it doesn't exist
        QDir().mkpath(autostartPath);
        
        QFile desktopFile(desktopFilePath);
        if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&desktopFile);
            out << "[Desktop Entry]\n";
            out << "Type=Application\n";
            out << "Name=MouseCross\n";
            out << "Comment=Visual Mouse Locator\n";
            out << "Exec=" << getApplicationPath() << "\n";
            out << "Icon=mousecross\n";
            out << "Hidden=false\n";
            out << "NoDisplay=false\n";
            out << "X-GNOME-Autostart-enabled=true\n";
            desktopFile.close();
            return true;
        }
    } else {
        return QFile::remove(desktopFilePath);
    }
    
    return false;
}
#endif