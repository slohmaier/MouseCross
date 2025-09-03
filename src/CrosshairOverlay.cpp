#include "CrosshairOverlay.h"
#include "SettingsManager.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <QTimer>
#include <QMouseEvent>
#include <QWindow>
#include <cmath>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

CrosshairOverlay::CrosshairOverlay(QWidget *parent)
    : QWidget(parent)
    , m_lineWidth(4)
    , m_offsetFromCursor(49)
    , m_thicknessMultiplier(3.0)
    , m_color(Qt::white)
    , m_opacity(0.8)
    , m_showArrows(true)
{
    setupWindow();
    
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &CrosshairOverlay::updateMousePosition);
    m_updateTimer->start(16); // ~60 FPS
    
    updateMousePosition();
}

void CrosshairOverlay::setupWindow()
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

void CrosshairOverlay::updateFromSettings(SettingsManager* settings)
{
    m_lineWidth = settings->crosshairLineWidth();
    m_offsetFromCursor = settings->crosshairOffsetFromCursor();
    m_thicknessMultiplier = settings->crosshairThicknessMultiplier();
    m_color = settings->crosshairColor();
    m_opacity = settings->crosshairOpacity();
    m_showArrows = settings->showArrows();
    
    setWindowOpacity(m_opacity);
    update();
}

void CrosshairOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    drawCrosshair(painter);
}

void CrosshairOverlay::drawCrosshair(QPainter &painter)
{
    // Get current mouse position
    int x = m_mousePos.x();
    int y = m_mousePos.y();
    
    // Calculate distances to edges
    int distToLeft = x - m_screenGeometry.left();
    int distToRight = m_screenGeometry.right() - x;
    int distToTop = y - m_screenGeometry.top();
    int distToBottom = m_screenGeometry.bottom() - y;
    
    // Draw gradient lines with increasing thickness away from center
    drawGradientLine(painter, x - m_offsetFromCursor, y, m_screenGeometry.left(), y, distToLeft);  // Left
    drawGradientLine(painter, x + m_offsetFromCursor, y, m_screenGeometry.right(), y, distToRight); // Right  
    drawGradientLine(painter, x, y - m_offsetFromCursor, x, m_screenGeometry.top(), distToTop);     // Up
    drawGradientLine(painter, x, y + m_offsetFromCursor, x, m_screenGeometry.bottom(), distToBottom); // Down
}

void CrosshairOverlay::drawGradientLine(QPainter &painter, int startX, int startY, int endX, int endY, int totalDistance)
{
    const int segments = 50; // Number of segments for gradient effect
    
    for (int i = 0; i < segments; ++i) {
        double progress = static_cast<double>(i) / segments;
        double nextProgress = static_cast<double>(i + 1) / segments;
        
        // Calculate thickness based on distance from center
        double thicknessMultiplier = 1.0 + (m_thicknessMultiplier - 1.0) * progress;
        int baseThickness = getScaledLineWidth();
        int currentThickness = static_cast<int>(baseThickness * thicknessMultiplier);
        
        // Calculate segment start and end points
        int segStartX = startX + static_cast<int>((endX - startX) * progress);
        int segStartY = startY + static_cast<int>((endY - startY) * progress);
        int segEndX = startX + static_cast<int>((endX - startX) * nextProgress);
        int segEndY = startY + static_cast<int>((endY - startY) * nextProgress);
        
        // Always draw dual-layer lines for contrast
        // Outer line with selected color
        QPen outerPen(m_color);
        outerPen.setWidth(currentThickness);
        outerPen.setCapStyle(Qt::FlatCap);
        painter.setPen(outerPen);
        painter.drawLine(segStartX, segStartY, segEndX, segEndY);
        
        // Inner line with inverse color (half thickness)
        QColor inverseColor;
        int r, g, b;
        m_color.getRgb(&r, &g, &b);
        inverseColor.setRgb(255 - r, 255 - g, 255 - b);
        
        QPen innerPen(inverseColor);
        innerPen.setWidth(currentThickness / 2);
        innerPen.setCapStyle(Qt::FlatCap);
        painter.setPen(innerPen);
        painter.drawLine(segStartX, segStartY, segEndX, segEndY);
    }
    
    // Draw arrows if enabled
    if (m_showArrows) {
        drawArrows(painter, startX, startY, endX, endY, totalDistance);
    }
}

void CrosshairOverlay::drawArrows(QPainter &painter, int startX, int startY, int endX, int endY, int totalDistance)
{
    // Draw arrows pointing toward the center, spaced along the line
    const int arrowSize = getScaledLineWidth() * 2; // Arrow size based on line width
    const int arrowSpacing = arrowSize * 3; // Halved spacing to double the arrows
    
    // Calculate direction vector (pointing toward center)
    double deltaX = startX - endX;
    double deltaY = startY - endY;
    double length = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    
    if (length == 0) return; // Avoid division by zero
    
    // Normalize direction vector
    double dirX = deltaX / length;
    double dirY = deltaY / length;
    
    // Perpendicular vector for arrow wings
    double perpX = -dirY;
    double perpY = dirX;
    
    // Set up pen for arrows (using selected color)
    QPen arrowPen(m_color);
    arrowPen.setWidth(getScaledLineWidth());
    arrowPen.setCapStyle(Qt::FlatCap);
    painter.setPen(arrowPen);
    
    // Draw arrows along the line, starting from some distance from the edge
    int startDistance = arrowSpacing; // Don't draw arrows too close to screen edge
    for (int distance = startDistance; distance < totalDistance - arrowSpacing; distance += arrowSpacing) {
        // Calculate arrow position
        double progress = static_cast<double>(distance) / totalDistance;
        int arrowX = endX + static_cast<int>((startX - endX) * progress);
        int arrowY = endY + static_cast<int>((startY - endY) * progress);
        
        // Draw arrow head (triangular shape pointing toward center)
        QPointF arrowTip(arrowX, arrowY);
        QPointF arrowLeft(arrowX - dirX * arrowSize + perpX * arrowSize * 0.5, 
                          arrowY - dirY * arrowSize + perpY * arrowSize * 0.5);
        QPointF arrowRight(arrowX - dirX * arrowSize - perpX * arrowSize * 0.5, 
                           arrowY - dirY * arrowSize - perpY * arrowSize * 0.5);
        
        painter.drawLine(arrowTip, arrowLeft);
        painter.drawLine(arrowTip, arrowRight);
    }
}

void CrosshairOverlay::updateMousePosition()
{
    QPoint newPos = QCursor::pos();
    if (newPos != m_mousePos) {
        m_mousePos = newPos;
        update();
    }
}

void CrosshairOverlay::mousePressEvent(QMouseEvent *event)
{
    // This should never be called due to WindowTransparentForInput
    Q_UNUSED(event);
}

bool CrosshairOverlay::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    Q_UNUSED(event);
    return false;
}

double CrosshairOverlay::getUIScaleFactor() const
{
    // Get the screen containing the mouse cursor
    QScreen *screen = QApplication::screenAt(QCursor::pos());
    if (!screen) {
        screen = QApplication::primaryScreen();
    }
    
    // Return device pixel ratio as scale factor
    return screen->devicePixelRatio();
}

int CrosshairOverlay::getScaledLineWidth() const
{
    double scaleFactor = getUIScaleFactor();
    int scaledWidth = static_cast<int>(m_lineWidth * scaleFactor);
    
    // Ensure minimum thickness of 3 pixels
    return qMax(3, scaledWidth);
}