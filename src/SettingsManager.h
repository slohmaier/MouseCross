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

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include "CrosshairRenderer.h"

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);
    
    // First run
    bool isFirstRun() const;
    void setFirstRun(bool firstRun);
    
    // Crosshair appearance
    int crosshairLineWidth() const;
    void setCrosshairLineWidth(int width);
    
    int crosshairOffsetFromCursor() const;
    void setCrosshairOffsetFromCursor(int offset);
    
    double crosshairThicknessMultiplier() const;
    void setCrosshairThicknessMultiplier(double multiplier);
    
    QColor crosshairColor() const;
    void setCrosshairColor(const QColor& color);
    
    
    double crosshairOpacity() const;
    void setCrosshairOpacity(double opacity);
    
    bool showArrows() const;
    void setShowArrows(bool show);
    
    double circleSpacingIncrease() const;
    void setCircleSpacingIncrease(double percentage);
    
    CrosshairRenderer::DirectionShape directionShape() const;
    void setDirectionShape(CrosshairRenderer::DirectionShape shape);
    
    // Behavior
    bool autoStart() const;
    void setAutoStart(bool enabled);
    
    bool activateOnStart() const;
    void setActivateOnStart(bool activate);
    
    // Hotkey
    QString toggleHotkey() const;
    void setToggleHotkey(const QString& hotkey);

signals:
    void settingsChanged();

private:
    QSettings m_settings;
    
    void setDefaultValues();
};

#endif // SETTINGSMANAGER_H