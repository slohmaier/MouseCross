/*
 * MouseCross - A crosshair overlay application for visually impaired users
 * Copyright (C) 2025 Stefan Lohmaier <stefan@slohmaier.de>
 *
 * This file is part of MouseCross.
 *
 * MouseCross is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MouseCross is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MouseCross. If not, see <https://www.gnu.org/licenses/>.
 *
 * Project website: https://slohmaier.de/mousecross
 */

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
    // Check if MouseCross is in Login Items using AppleScript
    QProcess process;
    QString script = "tell application \"System Events\" to get the name of every login item";

    process.start("osascript", QStringList() << "-e" << script);
    process.waitForFinished();

    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        return output.contains("MouseCross");
    }
    return false;
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
    // Use osascript to add/remove from Login Items
    // This is the modern approach that works with macOS 10.14+
    QString appPath = getApplicationPath();

    // For app bundles, we need to use the .app path, not the executable inside
    if (appPath.contains(".app/Contents/MacOS/")) {
        // Extract the .app bundle path
        int appIndex = appPath.indexOf(".app");
        if (appIndex != -1) {
            appPath = appPath.left(appIndex + 4); // Include ".app"
        }
    }

    QProcess process;

    if (enabled) {
        // Add to Login Items using AppleScript
        QString script = QString(
            "tell application \"System Events\" to make login item at end with properties "
            "{path:\"%1\", hidden:false, name:\"MouseCross\"}"
        ).arg(appPath);

        process.start("osascript", QStringList() << "-e" << script);
        process.waitForFinished();

        if (process.exitCode() != 0) {
            // If the item already exists, that's OK
            QString output = process.readAllStandardError();
            if (!output.contains("already exists")) {
                qDebug() << "Failed to add login item:" << output;
                return false;
            }
        }
        return true;
    } else {
        // Remove from Login Items using AppleScript
        QString script = QString(
            "tell application \"System Events\" to delete login item \"MouseCross\""
        );

        process.start("osascript", QStringList() << "-e" << script);
        process.waitForFinished();

        // Even if it fails (item not found), we consider it a success
        return true;
    }
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