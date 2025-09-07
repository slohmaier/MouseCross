#ifndef CROSSHAIRRENDERER_H
#define CROSSHAIRRENDERER_H

#include <QColor>
#include <QPoint>
#include <QRect>
#include <memory>

class CrosshairRenderer
{
public:
    struct Settings {
        QColor color = Qt::white;
        int lineWidth = 4;
        int offsetFromCursor = 49;
        double thicknessMultiplier = 3.0;
        double opacity = 0.8;
        bool showArrows = true;
        bool inverted = false;
    };

    virtual ~CrosshairRenderer() = default;

    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
    
    virtual void startRendering() = 0;
    virtual void stopRendering() = 0;
    
    virtual void updateSettings(const Settings& settings) = 0;
    virtual void updateMousePosition(const QPoint& pos) = 0;
    
    virtual bool isRendering() const = 0;
    
    static std::unique_ptr<CrosshairRenderer> createPlatformRenderer();
    
protected:
    Settings m_settings;
    QPoint m_mousePos;
    QRect m_screenGeometry;
    bool m_isRendering = false;
};

#endif // CROSSHAIRRENDERER_H