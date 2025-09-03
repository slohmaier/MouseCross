#include "CrosshairOverlay.h"
#include "SettingsManager.h"
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <QTimer>
#include <QMouseEvent>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

CrosshairOverlay::CrosshairOverlay(QWidget *parent)
    : QWidget(parent)
    , m_lineWidth(2)
    , m_offsetFromCursor(10)
    , m_thicknessMultiplier(3.0)
    , m_color(Qt::red)
    , m_invertedMode(false)
    , m_opacity(0.8)
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
    m_invertedMode = settings->invertedMode();
    m_opacity = settings->crosshairOpacity();
    
    setWindowOpacity(m_opacity);
    update();
}

void CrosshairOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_invertedMode) {
        painter.setCompositionMode(QPainter::CompositionMode_Difference);
    }
    
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
        int currentThickness = static_cast<int>(m_lineWidth * thicknessMultiplier);
        
        // Calculate segment start and end points
        int segStartX = startX + static_cast<int>((endX - startX) * progress);
        int segStartY = startY + static_cast<int>((endY - startY) * progress);
        int segEndX = startX + static_cast<int>((endX - startX) * nextProgress);
        int segEndY = startY + static_cast<int>((endY - startY) * nextProgress);
        
        // Set pen with current thickness
        QPen pen(m_color);
        pen.setWidth(currentThickness);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);
        
        // Draw segment
        painter.drawLine(segStartX, segStartY, segEndX, segEndY);
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