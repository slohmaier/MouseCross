#ifndef MOUSECROSSAPP_H
#define MOUSECROSSAPP_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <memory>

#ifdef Q_OS_WIN
#include <windows.h>
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
};

#endif // MOUSECROSSAPP_H