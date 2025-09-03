#include "MouseCrossApp.h"
#include "CrosshairOverlay.h"
#include "WelcomeDialog.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"
#include "SettingsManager.h"
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QMessageBox>
#include <QIcon>

MouseCrossApp::MouseCrossApp(QObject *parent)
    : QObject(parent)
    , m_crosshairActive(false)
{
    m_settings = std::make_unique<SettingsManager>();
    m_crosshair = std::make_unique<CrosshairOverlay>();
    
    connect(m_settings.get(), &SettingsManager::settingsChanged,
            this, &MouseCrossApp::updateCrosshairFromSettings);
}

MouseCrossApp::~MouseCrossApp() = default;

bool MouseCrossApp::init()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, tr("MouseCross"),
                            tr("System tray is not available on this system"));
        return false;
    }
    
    createActions();
    createTrayIcon();
    setupHotkey();
    showWelcomeIfFirstRun();
    
    updateCrosshairFromSettings();
    
    if (m_settings->activateOnStart()) {
        toggleCrosshair();
    }
    
    m_trayIcon->show();
    
    return true;
}

void MouseCrossApp::createActions()
{
    m_toggleAction = new QAction(tr("&Toggle Crosshair"), this);
    connect(m_toggleAction, &QAction::triggered, this, &MouseCrossApp::toggleCrosshair);
    
    m_settingsAction = new QAction(tr("&Settings..."), this);
    connect(m_settingsAction, &QAction::triggered, this, &MouseCrossApp::showSettings);
    
    m_aboutAction = new QAction(tr("&About..."), this);
    connect(m_aboutAction, &QAction::triggered, this, &MouseCrossApp::showAbout);
    
    m_quitAction = new QAction(tr("&Quit"), this);
    connect(m_quitAction, &QAction::triggered, this, &MouseCrossApp::quit);
}

void MouseCrossApp::createTrayIcon()
{
    m_trayMenu = std::make_unique<QMenu>();
    m_trayMenu->addAction(m_toggleAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_settingsAction);
    m_trayMenu->addAction(m_aboutAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);
    
    m_trayIcon = std::make_unique<QSystemTrayIcon>(this);
    m_trayIcon->setContextMenu(m_trayMenu.get());
    
    QIcon icon(":/icons/app_icon.png");
    if (icon.isNull()) {
        icon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    }
    m_trayIcon->setIcon(icon);
    
    m_trayIcon->setToolTip(tr("MouseCross - Visual Mouse Locator"));
    
    connect(m_trayIcon.get(), &QSystemTrayIcon::activated,
            this, &MouseCrossApp::trayIconActivated);
}

void MouseCrossApp::setupHotkey()
{
    // Platform-specific hotkey implementation would go here
    // For now, we'll rely on the settings dialog to configure this
}

void MouseCrossApp::showWelcomeIfFirstRun()
{
    if (m_settings->isFirstRun()) {
        WelcomeDialog welcome;
        welcome.exec();
        m_settings->setFirstRun(false);
    }
}

void MouseCrossApp::showSettings()
{
    SettingsDialog dialog(m_settings.get());
    if (dialog.exec() == QDialog::Accepted) {
        dialog.saveSettings();
        updateCrosshairFromSettings();
    }
}

void MouseCrossApp::showAbout()
{
    AboutDialog about;
    about.exec();
}

void MouseCrossApp::toggleCrosshair()
{
    m_crosshairActive = !m_crosshairActive;
    
    if (m_crosshairActive) {
        m_crosshair->show();
        m_toggleAction->setText(tr("&Hide Crosshair"));
        m_trayIcon->showMessage(tr("MouseCross"), 
                              tr("Crosshair activated"), 
                              QSystemTrayIcon::Information, 1000);
    } else {
        m_crosshair->hide();
        m_toggleAction->setText(tr("&Show Crosshair"));
        m_trayIcon->showMessage(tr("MouseCross"), 
                              tr("Crosshair deactivated"), 
                              QSystemTrayIcon::Information, 1000);
    }
}

void MouseCrossApp::quit()
{
    QApplication::quit();
}

void MouseCrossApp::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        toggleCrosshair();
        break;
    default:
        break;
    }
}

void MouseCrossApp::onHotkeyPressed()
{
    toggleCrosshair();
}

void MouseCrossApp::updateCrosshairFromSettings()
{
    m_crosshair->updateFromSettings(m_settings.get());
}