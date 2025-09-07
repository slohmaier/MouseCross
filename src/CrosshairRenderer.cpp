#include "CrosshairRenderer.h"

#ifdef Q_OS_MAC
#include "MacOSCrosshairRenderer.h"
#elif defined(Q_OS_WIN)
#include "WindowsCrosshairRenderer.h"
#else
// Linux/X11 implementation can use the Windows renderer as a fallback
#include "WindowsCrosshairRenderer.h"
#endif

std::unique_ptr<CrosshairRenderer> CrosshairRenderer::createPlatformRenderer()
{
#ifdef Q_OS_MAC
    return std::make_unique<MacOSCrosshairRenderer>();
#elif defined(Q_OS_WIN)
    return std::make_unique<WindowsCrosshairRenderer>();
#else
    // Linux/X11 can use the same approach as Windows
    return std::make_unique<WindowsCrosshairRenderer>();
#endif
}