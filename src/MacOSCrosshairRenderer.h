#ifndef MACOSCROSSHAIRRENDERER_H
#define MACOSCROSSHAIRRENDERER_H

#include "CrosshairRenderer.h"
#include <QTimer>
#include <memory>

#ifdef __OBJC__
@class CrosshairView;
#else
typedef struct objc_object CrosshairView;
#endif

class MacOSCrosshairRenderer : public QObject, public CrosshairRenderer
{
    Q_OBJECT
    
public:
    MacOSCrosshairRenderer();
    ~MacOSCrosshairRenderer() override;
    
    bool initialize() override;
    void cleanup() override;
    
    void startRendering() override;
    void stopRendering() override;
    
    void updateSettings(const Settings& settings) override;
    void updateMousePosition(const QPoint& pos) override;
    
    bool isRendering() const override { return m_isRendering; }
    
private slots:
    void updateCrosshair();
    
private:
    CrosshairView* m_view;
    QTimer* m_updateTimer;
    void* m_window; // NSWindow*
};

#endif // MACOSCROSSHAIRRENDERER_H