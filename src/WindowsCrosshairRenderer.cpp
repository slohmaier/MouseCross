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

#include "WindowsCrosshairRenderer.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <cmath>
#include <vector>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

WindowsCrosshairRenderer::WindowsCrosshairRenderer()
    : QWidget(nullptr)
    , m_updateTimer(nullptr)
{
}

WindowsCrosshairRenderer::~WindowsCrosshairRenderer()
{
    cleanup();
}

bool WindowsCrosshairRenderer::initialize()
{
    setupWindow();
    
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &WindowsCrosshairRenderer::updateCrosshair);
    
    return true;
}

void WindowsCrosshairRenderer::cleanup()
{
    stopRendering();
    
    if (m_updateTimer) {
        delete m_updateTimer;
        m_updateTimer = nullptr;
    }
}

void WindowsCrosshairRenderer::setupWindow()
{
    setWindowFlags(Qt::WindowStaysOnTopHint | 
                  Qt::FramelessWindowHint | 
                  Qt::Tool |
                  Qt::WindowTransparentForInput);
    
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_NoSystemBackground);
    
#ifdef Q_OS_WIN
    // Make window click-through on Windows
    HWND hwnd = reinterpret_cast<HWND>(winId());
    SetWindowLong(hwnd, GWL_EXSTYLE, 
                 GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
#endif
    
    // Cover all screens
    m_screenGeometry = QRect();
    for (QScreen *screen : QApplication::screens()) {
        m_screenGeometry = m_screenGeometry.united(screen->geometry());
    }
    setGeometry(m_screenGeometry);
}

void WindowsCrosshairRenderer::startRendering()
{
    if (m_isRendering) return;
    
    show();
    raise();
    
    if (m_updateTimer) {
        m_updateTimer->start(16); // ~60 FPS
    }
    
    m_isRendering = true;
    updateCrosshair();
}

void WindowsCrosshairRenderer::stopRendering()
{
    if (!m_isRendering) return;
    
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
    
    hide();
    m_isRendering = false;
}

void WindowsCrosshairRenderer::updateSettings(const Settings& settings)
{
    m_settings = settings;
    setWindowOpacity(settings.opacity);
    update();
}

void WindowsCrosshairRenderer::updateMousePosition(const QPoint& pos)
{
    m_mousePos = pos;
    update();
}

void WindowsCrosshairRenderer::updateCrosshair()
{
    QPoint newPos = QCursor::pos();
    if (newPos != m_mousePos) {
        updateMousePosition(newPos);
    }
}

void WindowsCrosshairRenderer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_settings.inverted) {
        painter.setCompositionMode(QPainter::CompositionMode_Difference);
    }
    
    drawCrosshair(painter);
}

void WindowsCrosshairRenderer::drawCrosshair(QPainter &painter)
{
    int x = m_mousePos.x();
    int y = m_mousePos.y();
    
    int distToLeft = x - m_screenGeometry.left();
    int distToRight = m_screenGeometry.right() - x;
    int distToTop = y - m_screenGeometry.top();
    int distToBottom = m_screenGeometry.bottom() - y;
    
    // Draw closing lines at the start of each crosshair arm (at offset positions)
    int baseThickness = getScaledLineWidth();
    QPen closingPen(m_settings.color);
    closingPen.setWidth(baseThickness);
    closingPen.setCapStyle(Qt::FlatCap);
    painter.setPen(closingPen);
    
    int closingLineLength = baseThickness; // Match the width of the crosshair at thinnest point
    
    // Left arm start - vertical closing line
    painter.drawLine(x - m_settings.offsetFromCursor, y - closingLineLength/2, 
                     x - m_settings.offsetFromCursor, y + closingLineLength/2);
    
    // Right arm start - vertical closing line
    painter.drawLine(x + m_settings.offsetFromCursor, y - closingLineLength/2, 
                     x + m_settings.offsetFromCursor, y + closingLineLength/2);
    
    // Top arm start - horizontal closing line
    painter.drawLine(x - closingLineLength/2, y - m_settings.offsetFromCursor, 
                     x + closingLineLength/2, y - m_settings.offsetFromCursor);
    
    // Bottom arm start - horizontal closing line
    painter.drawLine(x - closingLineLength/2, y + m_settings.offsetFromCursor, 
                     x + closingLineLength/2, y + m_settings.offsetFromCursor);
    
    drawGradientLine(painter, x - m_settings.offsetFromCursor, y, m_screenGeometry.left(), y, distToLeft);
    drawGradientLine(painter, x + m_settings.offsetFromCursor, y, m_screenGeometry.right(), y, distToRight);
    drawGradientLine(painter, x, y - m_settings.offsetFromCursor, x, m_screenGeometry.top(), distToTop);
    drawGradientLine(painter, x, y + m_settings.offsetFromCursor, x, m_screenGeometry.bottom(), distToBottom);
    
    // Draw thin center lines in each crosshair arm with main color
    QPen centerPen(m_settings.color);
    centerPen.setWidth(1);
    centerPen.setCapStyle(Qt::FlatCap);
    painter.setPen(centerPen);
    
    // Left arm center line
    painter.drawLine(x - m_settings.offsetFromCursor, y, m_screenGeometry.left(), y);
    // Right arm center line
    painter.drawLine(x + m_settings.offsetFromCursor, y, m_screenGeometry.right(), y);
    // Top arm center line
    painter.drawLine(x, y - m_settings.offsetFromCursor, x, m_screenGeometry.top());
    // Bottom arm center line
    painter.drawLine(x, y + m_settings.offsetFromCursor, x, m_screenGeometry.bottom());
}

void WindowsCrosshairRenderer::drawGradientLine(QPainter &painter, int startX, int startY, int endX, int endY, int totalDistance)
{
    const int segments = 50;
    
    for (int i = 0; i < segments; ++i) {
        double progress = static_cast<double>(i) / segments;
        double nextProgress = static_cast<double>(i + 1) / segments;
        
        double thicknessMultiplier = 1.0 + (m_settings.thicknessMultiplier - 1.0) * progress;
        int baseThickness = getScaledLineWidth();
        int currentThickness = static_cast<int>(baseThickness * thicknessMultiplier);
        
        int segStartX = startX + static_cast<int>((endX - startX) * progress);
        int segStartY = startY + static_cast<int>((endY - startY) * progress);
        int segEndX = startX + static_cast<int>((endX - startX) * nextProgress);
        int segEndY = startY + static_cast<int>((endY - startY) * nextProgress);
        
        // Draw outer line with selected color
        QPen outerPen(m_settings.color);
        outerPen.setWidth(currentThickness);
        outerPen.setCapStyle(Qt::FlatCap);
        painter.setPen(outerPen);
        painter.drawLine(segStartX, segStartY, segEndX, segEndY);
        
        // Draw inner contrasting line if not inverted
        if (!m_settings.inverted) {
            QColor inverseColor;
            int r, g, b;
            m_settings.color.getRgb(&r, &g, &b);
            inverseColor.setRgb(255 - r, 255 - g, 255 - b);
            
            QPen innerPen(inverseColor);
            innerPen.setWidth(currentThickness / 2);
            innerPen.setCapStyle(Qt::FlatCap);
            painter.setPen(innerPen);
            painter.drawLine(segStartX, segStartY, segEndX, segEndY);
        }
    }
    
    if (m_settings.showArrows) {
        drawDirectionShapes(painter, startX, startY, endX, endY, totalDistance);
    }
}

void WindowsCrosshairRenderer::drawDirectionShapes(QPainter &painter, int startX, int startY, int endX, int endY, int totalDistance)
{
    double deltaX = startX - endX;
    double deltaY = startY - endY;
    double length = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    
    if (length == 0) return;
    
    painter.setBrush(QBrush(m_settings.color));
    painter.setPen(Qt::NoPen);
    
    // Get actual screen bounds for clipping  
    QRect screenBounds = QApplication::primaryScreen()->geometry();
    
    // Generate circle positions with fixed diameter-based spacing
    std::vector<double> circlePositions;
    int baseThickness = getScaledLineWidth();
    
    // Calculate initial circle diameter at center (smallest size)
    // Circle radius = currentThickness / 4, so diameter = currentThickness / 2
    double baseDiameter = baseThickness / 2.0;       // Base circle diameter
    double initialSpacing = baseDiameter * 2.0;      // Start with 2x circle diameter (half of 4x)
    double spacingMultiplier = 1.0 + (m_settings.circleSpacingIncrease / 100.0);  // Convert percentage to multiplier
    
    double currentDistance = initialSpacing;         // First circle position
    double currentSpacing = initialSpacing;
    
    // Generate positions from center toward edge with fixed progressive spacing
    while (currentDistance < totalDistance * 1.2) {  // Go slightly beyond line end for edge cases
        circlePositions.push_back(currentDistance);
        
        // Calculate circle diameter at this position for next spacing
        double progress = currentDistance / totalDistance;
        double thickMultiplier = 1.0 + (m_settings.thicknessMultiplier - 1.0) * progress;
        double currentThickness = baseThickness * thickMultiplier;
        double circleDiameter = currentThickness / 2.0;  // Circle diameter = thickness / 2
        
        // Next spacing is 2x the current circle diameter, increased by setting percentage
        currentSpacing = (circleDiameter * 2.0) * spacingMultiplier;
        spacingMultiplier *= (1.0 + (m_settings.circleSpacingIncrease / 100.0));  // Compound the percentage increase
        currentDistance += currentSpacing;
    }
    
    // Draw circles from center outward
    for (double dist : circlePositions) {
        // Skip if beyond the crosshair line
        if (dist > totalDistance) continue;
        
        // Calculate position along the line (0.0 at center, 1.0 at edge)
        double progress = dist / totalDistance;
        int circleX = startX + static_cast<int>((endX - startX) * progress);
        int circleY = startY + static_cast<int>((endY - startY) * progress);
        
        // Calculate circle size: small at center, large at edge
        double thicknessMultiplier = 1.0 + (m_settings.thicknessMultiplier - 1.0) * progress;
        int currentThickness = static_cast<int>(baseThickness * thicknessMultiplier);
        int circleRadius = currentThickness / 4;
        
        // Create shape bounds
        QRect shapeBounds(circleX - circleRadius, circleY - circleRadius, 
                         circleRadius * 2, circleRadius * 2);
        
        // Check if shape intersects screen bounds and draw with clipping
        if (shapeBounds.intersects(screenBounds)) {
            // Save painter state
            painter.save();
            
            // Set clipping region for partial shapes
            painter.setClipRect(screenBounds);
            
            // Draw the appropriate shape
            switch (m_settings.directionShape) {
                case CrosshairRenderer::DirectionShape::Circle:
                    painter.drawEllipse(QPoint(circleX, circleY), circleRadius, circleRadius);
                    break;
                    
                case CrosshairRenderer::DirectionShape::Arrow:
                {
                    // Calculate arrow pointing toward center - scale clearly with thickness
                    double arrowSize = circleRadius * 2.0; // Make arrow much larger for visible scaling
                    double normalizedDeltaX = deltaX / length;
                    double normalizedDeltaY = deltaY / length;
                    
                    // Arrow tip points toward center (closer to center for better visibility)
                    int tipX = circleX + static_cast<int>(normalizedDeltaX * arrowSize * 0.3);
                    int tipY = circleY + static_cast<int>(normalizedDeltaY * arrowSize * 0.3);
                    
                    // Arrow base perpendicular to direction (wider base for better scaling visibility)
                    double perpX = -normalizedDeltaY;
                    double perpY = normalizedDeltaX;
                    
                    int baseX1 = circleX - static_cast<int>(normalizedDeltaX * arrowSize * 0.5 - perpX * arrowSize * 0.6);
                    int baseY1 = circleY - static_cast<int>(normalizedDeltaY * arrowSize * 0.5 - perpY * arrowSize * 0.6);
                    int baseX2 = circleX - static_cast<int>(normalizedDeltaX * arrowSize * 0.5 + perpX * arrowSize * 0.6);
                    int baseY2 = circleY - static_cast<int>(normalizedDeltaY * arrowSize * 0.5 + perpY * arrowSize * 0.6);
                    
                    QPolygon arrow;
                    arrow << QPoint(tipX, tipY) << QPoint(baseX1, baseY1) << QPoint(baseX2, baseY2);
                    painter.drawPolygon(arrow);
                    break;
                }
                    
                case CrosshairRenderer::DirectionShape::Cross:
                {
                    int crossSize = static_cast<int>(circleRadius * 1.2); // Make cross larger and more visible
                    int lineWidth = static_cast<int>(circleRadius * 0.6); // Keep thickness proportional
                    
                    // Horizontal line
                    QRect hLine(circleX - crossSize, circleY - lineWidth/2, 
                               crossSize * 2, lineWidth);
                    painter.drawRect(hLine);
                    
                    // Vertical line
                    QRect vLine(circleX - lineWidth/2, circleY - crossSize, 
                               lineWidth, crossSize * 2);
                    painter.drawRect(vLine);
                    break;
                }
                    
                case CrosshairRenderer::DirectionShape::Raute:
                {
                    int rauteSize = static_cast<int>(circleRadius * 1.3); // Make Raute larger and more visible
                    
                    // Create diamond (Raute) shape
                    QPolygon diamond;
                    diamond << QPoint(circleX, circleY - rauteSize)      // Top
                            << QPoint(circleX + rauteSize, circleY)      // Right
                            << QPoint(circleX, circleY + rauteSize)      // Bottom
                            << QPoint(circleX - rauteSize, circleY);     // Left
                    
                    painter.drawPolygon(diamond);
                    break;
                }
            }
            
            // Restore painter state
            painter.restore();
        }
    }
}

double WindowsCrosshairRenderer::getUIScaleFactor() const
{
    QScreen *screen = QApplication::screenAt(QCursor::pos());
    if (!screen) {
        screen = QApplication::primaryScreen();
    }
    return screen->devicePixelRatio();
}

int WindowsCrosshairRenderer::getScaledLineWidth() const
{
    double scaleFactor = getUIScaleFactor();
    int scaledWidth = static_cast<int>(m_settings.lineWidth * scaleFactor);
    return qMax(3, scaledWidth);
}