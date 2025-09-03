#ifndef MOUSECROSSAPP_H
#define MOUSECROSSAPP_H

#include <QObject>
#include <QSystemTrayIcon>
#include <memory>

class QMenu;
class QAction;
class CrosshairOverlay;
class WelcomeDialog;
class SettingsDialog;
class AboutDialog;
class SettingsManager;

class MouseCrossApp : public QObject
{
    Q_OBJECT

public:
    MouseCrossApp(QObject *parent = nullptr);
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
    
    std::unique_ptr<QSystemTrayIcon> m_trayIcon;
    std::unique_ptr<QMenu> m_trayMenu;
    std::unique_ptr<CrosshairOverlay> m_crosshair;
    std::unique_ptr<SettingsManager> m_settings;
    
    QAction* m_toggleAction;
    QAction* m_settingsAction;
    QAction* m_aboutAction;
    QAction* m_quitAction;
    
    bool m_crosshairActive;
};

#endif // MOUSECROSSAPP_H