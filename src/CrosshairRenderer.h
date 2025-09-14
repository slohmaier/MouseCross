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

#ifndef CROSSHAIRRENDERER_H
#define CROSSHAIRRENDERER_H

#include <QColor>
#include <QPoint>
#include <QRect>
#include <memory>

class CrosshairRenderer
{
public:
    enum class DirectionShape {
        Circle = 0,
        Arrow = 1,
        Cross = 2,
        Raute = 3
    };

    struct Settings {
        QColor color = Qt::white;
        int lineWidth = 4;
        int offsetFromCursor = 49;
        double thicknessMultiplier = 3.0;
        double opacity = 0.8;
        bool showArrows = true;
        bool inverted = false;
        double circleSpacingIncrease = 5.0;
        DirectionShape directionShape = DirectionShape::Circle;
    };

    virtual ~CrosshairRenderer() = default;

    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    
    virtual void startRendering() = 0;
    virtual void stopRendering() = 0;
    
    virtual void updateSettings(const Settings& settings) = 0;
    virtual void updateMousePosition(const QPoint& pos) = 0;
    
    virtual bool isRendering() const = 0;
    
    static std::unique_ptr<CrosshairRenderer> createPlatformRenderer();
    
protected:
    Settings m_settings;
    QPoint m_mousePos;
    QRect m_screenGeometry;
    bool m_isRendering = false;
};

#endif // CROSSHAIRRENDERER_H