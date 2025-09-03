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
    , m_lineLength(20)
    , m_clearanceRadius(10)
    , m_color(Qt::red)
    , m_backgroundColor(Qt::white)
    , m_showBackground(true)
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
    QRect totalGeometry;
    for (QScreen *screen : QApplication::screens()) {
        totalGeometry = totalGeometry.united(screen->geometry());
    }
    setGeometry(totalGeometry);
}

void CrosshairOverlay::updateFromSettings(SettingsManager* settings)
{
    m_lineWidth = settings->crosshairLineWidth();
    m_lineLength = settings->crosshairLineLength();
    m_clearanceRadius = settings->crosshairClearanceRadius();
    m_color = settings->crosshairColor();
    m_backgroundColor = settings->crosshairBackgroundColor();
    m_showBackground = settings->showCrosshairBackground();
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
    // Draw background circle if enabled
    if (m_showBackground) {
        int bgRadius = m_clearanceRadius + m_lineLength + 5;
        QRect bgRect(m_mousePos.x() - bgRadius, m_mousePos.y() - bgRadius,
                    bgRadius * 2, bgRadius * 2);
        drawBackground(painter, bgRect);
    }
    
    // Set up pen for crosshair lines
    QPen pen(m_color);
    pen.setWidth(m_lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    
    // Calculate line positions
    int x = m_mousePos.x();
    int y = m_mousePos.y();
    int innerRadius = m_clearanceRadius;
    int outerRadius = m_clearanceRadius + m_lineLength;
    
    // Draw horizontal lines
    painter.drawLine(x - outerRadius, y, x - innerRadius, y);
    painter.drawLine(x + innerRadius, y, x + outerRadius, y);
    
    // Draw vertical lines
    painter.drawLine(x, y - outerRadius, x, y - innerRadius);
    painter.drawLine(x, y + innerRadius, x, y + outerRadius);
    
    // Draw optional corner lines for better visibility
    if (m_lineLength > 15) {
        int cornerOffset = innerRadius + 3;
        int cornerLength = 8;
        
        // Top-left
        painter.drawLine(x - cornerOffset - cornerLength, y - cornerOffset,
                        x - cornerOffset, y - cornerOffset);
        painter.drawLine(x - cornerOffset, y - cornerOffset - cornerLength,
                        x - cornerOffset, y - cornerOffset);
        
        // Top-right
        painter.drawLine(x + cornerOffset, y - cornerOffset,
                        x + cornerOffset + cornerLength, y - cornerOffset);
        painter.drawLine(x + cornerOffset, y - cornerOffset - cornerLength,
                        x + cornerOffset, y - cornerOffset);
        
        // Bottom-left
        painter.drawLine(x - cornerOffset - cornerLength, y + cornerOffset,
                        x - cornerOffset, y + cornerOffset);
        painter.drawLine(x - cornerOffset, y + cornerOffset,
                        x - cornerOffset, y + cornerOffset + cornerLength);
        
        // Bottom-right
        painter.drawLine(x + cornerOffset, y + cornerOffset,
                        x + cornerOffset + cornerLength, y + cornerOffset);
        painter.drawLine(x + cornerOffset, y + cornerOffset,
                        x + cornerOffset, y + cornerOffset + cornerLength);
    }
}

void CrosshairOverlay::drawBackground(QPainter &painter, const QRect& rect)
{
    QPen bgPen(m_backgroundColor);
    bgPen.setWidth(1);
    painter.setPen(bgPen);
    
    QBrush bgBrush(m_backgroundColor);
    bgBrush.setColor(QColor(m_backgroundColor.red(), 
                           m_backgroundColor.green(), 
                           m_backgroundColor.blue(), 
                           100)); // Semi-transparent
    painter.setBrush(bgBrush);
    
    painter.drawEllipse(rect);
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