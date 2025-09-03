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
    void drawBackground(QPainter &painter, const QRect& rect);
    
    QTimer* m_updateTimer;
    QPoint m_mousePos;
    
    // Settings
    int m_lineWidth;
    int m_lineLength;
    int m_clearanceRadius;
    QColor m_color;
    QColor m_backgroundColor;
    bool m_showBackground;
    bool m_invertedMode;
    double m_opacity;
};

#endif // CROSSHAIROVERLAY_H