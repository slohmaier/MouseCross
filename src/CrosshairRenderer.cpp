/*
 * MouseCross - A crosshair overlay application for visually impaired users
 * Copyright (C) 2025 Stefan Lohmaier <stefan@slohmaier.de>
 *
 * This file is part of MouseCross.
 *
 * MouseCross is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MouseCross is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MouseCross. If not, see <https://www.gnu.org/licenses/>.
 *
 * Project website: https://slohmaier.de/mousecross
 */

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