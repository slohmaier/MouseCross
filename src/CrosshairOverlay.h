#ifndef CROSSHAIROVERLAY_H
#define CROSSHAIROVERLAY_H

#include <QWidget>
#include <QTimer>

class SettingsManager;
class QPainter;

class CrosshairOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit CrosshairOverlay(QWidget *parent = nullptr);
    
    void updateFromSettings(SettingsManager* settings);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void updateMousePosition();

private:
    void setupWindow();
    void drawCrosshair(QPainter &painter);
    void drawGradientLine(QPainter &painter, int startX, int startY, int endX, int endY, int totalDistance);
    
    QTimer* m_updateTimer;
    QPoint m_mousePos;
    QRect m_screenGeometry;
    
    // Settings
    int m_lineWidth;
    int m_offsetFromCursor;
    double m_thicknessMultiplier;
    QColor m_color;
    bool m_invertedMode;
    double m_opacity;
};

#endif // CROSSHAIROVERLAY_H