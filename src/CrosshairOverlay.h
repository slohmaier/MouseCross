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

#ifndef CROSSHAIROVERLAY_H
#define CROSSHAIROVERLAY_H

#include <QObject>
#include <memory>

class SettingsManager;
class CrosshairRenderer;

class CrosshairOverlay : public QObject
{
    Q_OBJECT

public:
    explicit CrosshairOverlay(QObject *parent = nullptr);
    ~CrosshairOverlay();
    
    void updateFromSettings(SettingsManager* settings);
    void show();
    void hide();
    bool isVisible() const;

private:
    std::unique_ptr<CrosshairRenderer> m_renderer;
};

#endif // CROSSHAIROVERLAY_H