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

#include "CrosshairOverlay.h"
#include "CrosshairRenderer.h"
#include "SettingsManager.h"
#include <QDebug>

CrosshairOverlay::CrosshairOverlay(QObject *parent)
    : QObject(parent)
{
    m_renderer = CrosshairRenderer::createPlatformRenderer();
    if (m_renderer) {
        if (!m_renderer->initialize()) {
            qWarning() << "Failed to initialize crosshair renderer";
            m_renderer.reset();
        }
    } else {
        qWarning() << "Failed to create platform-specific crosshair renderer";
    }
}

CrosshairOverlay::~CrosshairOverlay()
{
    if (m_renderer) {
        m_renderer->cleanup();
    }
}

void CrosshairOverlay::updateFromSettings(SettingsManager* settings)
{
    if (!m_renderer) return;
    
    CrosshairRenderer::Settings rendererSettings;
    rendererSettings.color = settings->crosshairColor();
    rendererSettings.lineWidth = settings->crosshairLineWidth();
    rendererSettings.offsetFromCursor = settings->crosshairOffsetFromCursor();
    rendererSettings.thicknessMultiplier = settings->crosshairThicknessMultiplier();
    rendererSettings.opacity = settings->crosshairOpacity();
    rendererSettings.showArrows = settings->showArrows();
    rendererSettings.circleSpacingIncrease = settings->circleSpacingIncrease();
    rendererSettings.directionShape = settings->directionShape();
    rendererSettings.inverted = false; // Can be added to settings later if needed
    
    m_renderer->updateSettings(rendererSettings);
}

void CrosshairOverlay::show()
{
    if (m_renderer) {
        m_renderer->startRendering();
    }
}

void CrosshairOverlay::hide()
{
    if (m_renderer) {
        m_renderer->stopRendering();
    }
}

bool CrosshairOverlay::isVisible() const
{
    return m_renderer && m_renderer->isRendering();
}