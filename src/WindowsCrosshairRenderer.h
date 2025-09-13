#ifndef WINDOWSCROSSHAIRRENDERER_H
#define WINDOWSCROSSHAIRRENDERER_H

#include "CrosshairRenderer.h"
#include <QWidget>
#include <QTimer>
#include <memory>

class WindowsCrosshairRenderer : public QWidget, public CrosshairRenderer
{
    Q_OBJECT
    
public:
    WindowsCrosshairRenderer();
    ~WindowsCrosshairRenderer() override;
    
    bool initialize() override;
    void cleanup() override;
    
    void startRendering() override;
    void stopRendering() override;
    
    void updateSettings(const Settings& settings) override;
    void updateMousePosition(const QPoint& pos) override;
    
    bool isRendering() const override { return m_isRendering; }
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private slots:
    void updateCrosshair();
    
private:
    void setupWindow();
    void drawCrosshair(QPainter &painter);
    void drawGradientLine(QPainter &painter, int startX, int startY, int endX, int endY, int totalDistance);
    void drawCircles(QPainter &painter, int startX, int startY, int endX, int endY, int totalDistance);
    double getUIScaleFactor() const;
    int getScaledLineWidth() const;
    
    QTimer* m_updateTimer;
};

#endif // WINDOWSCROSSHAIRRENDERER_H