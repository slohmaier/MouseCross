#include "CrosshairOverlay.h"
#include "CrosshairRenderer.h"
#include "SettingsManager.h"
#include <QDebug>

CrosshairOverlay::CrosshairOverlay(QObject *parent)
    : QObject(parent)
{
    m_renderer = CrosshairRenderer::createPlatformRenderer();
    if (m_renderer) {
        if (!m_renderer->initialize()) {
            qWarning() << "Failed to initialize crosshair renderer";
            m_renderer.reset();
        }
    } else {
        qWarning() << "Failed to create platform-specific crosshair renderer";
    }
}

CrosshairOverlay::~CrosshairOverlay()
{
    if (m_renderer) {
        m_renderer->cleanup();
    }
}

void CrosshairOverlay::updateFromSettings(SettingsManager* settings)
{
    if (!m_renderer) return;
    
    CrosshairRenderer::Settings rendererSettings;
    rendererSettings.color = settings->crosshairColor();
    rendererSettings.lineWidth = settings->crosshairLineWidth();
    rendererSettings.offsetFromCursor = settings->crosshairOffsetFromCursor();
    rendererSettings.thicknessMultiplier = settings->crosshairThicknessMultiplier();
    rendererSettings.opacity = settings->crosshairOpacity();
    rendererSettings.showArrows = settings->showArrows();
    rendererSettings.inverted = false; // Can be added to settings later if needed
    
    m_renderer->updateSettings(rendererSettings);
}

void CrosshairOverlay::show()
{
    if (m_renderer) {
        m_renderer->startRendering();
    }
}

void CrosshairOverlay::hide()
{
    if (m_renderer) {
        m_renderer->stopRendering();
    }
}

bool CrosshairOverlay::isVisible() const
{
    return m_renderer && m_renderer->isRendering();
}