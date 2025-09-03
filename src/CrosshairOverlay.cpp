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
    // Set up pen for crosshair lines
    QPen pen(m_color);
    pen.setWidth(m_lineWidth);
    pen.setCapStyle(Qt::FlatCap);
    painter.setPen(pen);
    
    // Get current mouse position
    int x = m_mousePos.x();
    int y = m_mousePos.y();
    
    // Draw lines from cursor to screen edges with offset
    // Horizontal line to left edge
    painter.drawLine(m_screenGeometry.left(), y, x - m_offsetFromCursor, y);
    
    // Horizontal line to right edge
    painter.drawLine(x + m_offsetFromCursor, y, m_screenGeometry.right(), y);
    
    // Vertical line to top edge
    painter.drawLine(x, m_screenGeometry.top(), x, y - m_offsetFromCursor);
    
    // Vertical line to bottom edge
    painter.drawLine(x, y + m_offsetFromCursor, x, m_screenGeometry.bottom());
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