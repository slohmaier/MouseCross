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

#include <QApplication>
#include <QSharedMemory>
#include <QLocalSocket>
#include <QMessageBox>
#include <QTranslator>
#include <QLocale>
#include "MouseCrossApp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Prevent application from quitting when last window is closed
    QApplication::setQuitOnLastWindowClosed(false);
    
    QCoreApplication::setOrganizationName("MouseCross");
    QCoreApplication::setApplicationName("MouseCross");
    QCoreApplication::setApplicationVersion("0.1.0");
    
    // Load translations
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "mousecross_" + QLocale(locale).name().section('_', 0, 0);
        if (translator.load(":/translations/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    
    // Single instance check using QSharedMemory
    QSharedMemory sharedMemory("MouseCross-SingleInstance-Key");
    if (!sharedMemory.create(1)) {
        // Another instance is already running
        if (sharedMemory.attach()) {
            sharedMemory.detach();
        }
        return 0;
    }
    
    MouseCrossApp mouseCrossApp;
    
    if (!mouseCrossApp.init()) {
        return 1;
    }
    
    return app.exec();
}