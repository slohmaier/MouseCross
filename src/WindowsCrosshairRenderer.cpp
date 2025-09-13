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
    
    drawGradientLine(painter, x - m_settings.offsetFromCursor, y, m_screenGeometry.left(), y, distToLeft);
    drawGradientLine(painter, x + m_settings.offsetFromCursor, y, m_screenGeometry.right(), y, distToRight);
    drawGradientLine(painter, x, y - m_settings.offsetFromCursor, x, m_screenGeometry.top(), distToTop);
    drawGradientLine(painter, x, y + m_settings.offsetFromCursor, x, m_screenGeometry.bottom(), distToBottom);
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
        drawCircles(painter, startX, startY, endX, endY, totalDistance);
    }
}

void WindowsCrosshairRenderer::drawCircles(QPainter &painter, int startX, int startY, int endX, int endY, int totalDistance)
{
    double deltaX = startX - endX;
    double deltaY = startY - endY;
    double length = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    
    if (length == 0) return;
    
    painter.setBrush(QBrush(m_settings.color));
    painter.setPen(Qt::NoPen);
    
    // Collect all circle positions and spacings first (from edge towards center)
    std::vector<double> circlePositions;
    int baseThickness = getScaledLineWidth();
    double baseSpacing = baseThickness * 2;
    double maxSpacing = baseThickness * 6;
    double currentDistance = baseSpacing;
    double spacingMultiplier = 1.12; // Increase spacing by 12% each time moving toward center
    double currentSpacing = baseSpacing;
    
    // Build from edge towards center
    while (currentDistance < totalDistance - baseSpacing) {
        circlePositions.push_back(currentDistance);
        
        // Increase spacing as we move toward center (away from edge)
        currentSpacing = std::min(currentSpacing * spacingMultiplier, maxSpacing);
        currentDistance += currentSpacing;
    }
    
    // Draw circles from edge to center
    for (double dist : circlePositions) {
        // Progress from edge (0.0) to center (1.0) - reversed for circle sizing
        double progress = 1.0 - (dist / totalDistance);
        int circleX = endX + static_cast<int>((startX - endX) * progress);
        int circleY = endY + static_cast<int>((startY - endY) * progress);
        
        // Reverse the progress for circle sizing: small at center (progress=1.0), large at edge (progress=0.0)
        double sizeProgress = dist / totalDistance;
        double thicknessMultiplier = 1.0 + (m_settings.thicknessMultiplier - 1.0) * sizeProgress;
        int currentThickness = static_cast<int>(baseThickness * thicknessMultiplier);
        
        // Circle radius matches the inner line thickness (half of current thickness)
        int circleRadius = currentThickness / 4;
        
        // Draw filled circle
        painter.drawEllipse(QPoint(circleX, circleY), circleRadius, circleRadius);
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