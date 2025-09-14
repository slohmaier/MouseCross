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

#ifndef MOUSECROSSAPP_H
#define MOUSECROSSAPP_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <memory>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

class QMenu;
class QAction;
class CrosshairOverlay;
class WelcomeDialog;
class SettingsDialog;
class AboutDialog;
class SettingsManager;

class MouseCrossApp : public QWidget
{
    Q_OBJECT

public:
    MouseCrossApp(QWidget *parent = nullptr);
    ~MouseCrossApp();
    
    bool init();

private slots:
    void showSettings();
    void showAbout();
    void toggleCrosshair();
    void quit();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onHotkeyPressed();
    void updateCrosshairFromSettings();

private:
    void createTrayIcon();
    void createActions();
    void setupHotkey();
    void showWelcomeIfFirstRun();
    
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void registerHotkey();
    void unregisterHotkey();
    void updateHotkey();
#endif

#ifdef Q_OS_MAC
    void registerHotkey();
    void unregisterHotkey();
    void updateHotkey();
#endif
    
    std::unique_ptr<QSystemTrayIcon> m_trayIcon;
    std::unique_ptr<QMenu> m_trayMenu;
    std::unique_ptr<CrosshairOverlay> m_crosshair;
    std::unique_ptr<SettingsManager> m_settings;
    
    QAction* m_toggleAction;
    QAction* m_settingsAction;
    QAction* m_aboutAction;
    QAction* m_quitAction;
    
    bool m_crosshairActive;
    
#ifdef Q_OS_WIN
    static const int HOTKEY_ID = 1;
#endif

#ifdef Q_OS_MAC
    EventHotKeyRef m_hotKeyRef;
    static MouseCrossApp* s_instance;
    static OSStatus hotKeyHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
#endif
};

#endif // MOUSECROSSAPP_H