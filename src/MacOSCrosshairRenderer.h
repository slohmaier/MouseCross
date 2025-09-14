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

#ifndef MACOSCROSSHAIRRENDERER_H
#define MACOSCROSSHAIRRENDERER_H

#include "CrosshairRenderer.h"
#include <QTimer>
#include <memory>

#ifdef __OBJC__
@class CrosshairView;
#else
typedef struct objc_object CrosshairView;
#endif

class MacOSCrosshairRenderer : public QObject, public CrosshairRenderer
{
    Q_OBJECT
    
public:
    MacOSCrosshairRenderer();
    ~MacOSCrosshairRenderer() override;
    
    bool initialize() override;
    void cleanup() override;
    
    void startRendering() override;
    void stopRendering() override;
    
    void updateSettings(const Settings& settings) override;
    void updateMousePosition(const QPoint& pos) override;
    
    bool isRendering() const override { return m_isRendering; }
    
private slots:
    void updateCrosshair();
    
private:
    CrosshairView* m_view;
    QTimer* m_updateTimer;
    void* m_window; // NSWindow*
};

#endif // MACOSCROSSHAIRRENDERER_H