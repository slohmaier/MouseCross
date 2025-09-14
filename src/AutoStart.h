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