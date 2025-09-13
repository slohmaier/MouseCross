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
#include <QStyle>
#include <QDebug>
#include <QDir>
#include <QPixmap>
#include <QPainter>
#include <QPen>

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
MouseCrossApp* MouseCrossApp::s_instance = nullptr;
#endif

MouseCrossApp::MouseCrossApp(QWidget *parent)
    : QWidget(parent)
    , m_crosshairActive(false)
#ifdef Q_OS_MAC
    , m_hotKeyRef(nullptr)
#endif
{
    // Make this a hidden widget
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DontShowOnScreen);
    setFixedSize(1, 1);
    
#ifdef Q_OS_MAC
    s_instance = this;
#endif
    
    m_settings = std::make_unique<SettingsManager>();
    m_crosshair = std::make_unique<CrosshairOverlay>();
    
    connect(m_settings.get(), &SettingsManager::settingsChanged,
            this, &MouseCrossApp::updateCrosshairFromSettings);
}

MouseCrossApp::~MouseCrossApp() 
{
#ifdef Q_OS_WIN
    unregisterHotkey();
#endif
#ifdef Q_OS_MAC
    unregisterHotkey();
#endif
}

bool MouseCrossApp::init()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, tr("MouseCross"),
                            tr("System tray is not available on this system"));
        return false;
    }
    
    // Show the hidden widget to receive native events
    show();
    
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
    m_toggleAction->setToolTip(tr("Show or hide the crosshair overlay"));
    m_toggleAction->setStatusTip(tr("Toggle crosshair visibility on or off"));
    connect(m_toggleAction, &QAction::triggered, this, &MouseCrossApp::toggleCrosshair);
    
    m_settingsAction = new QAction(tr("&Settings..."), this);
    m_settingsAction->setToolTip(tr("Open settings dialog to configure crosshair appearance and behavior"));
    m_settingsAction->setStatusTip(tr("Configure MouseCross settings and preferences"));
    connect(m_settingsAction, &QAction::triggered, this, &MouseCrossApp::showSettings);
    
    m_aboutAction = new QAction(tr("&About..."), this);
    m_aboutAction->setToolTip(tr("Show information about MouseCross application"));
    m_aboutAction->setStatusTip(tr("Display version and information about MouseCross"));
    connect(m_aboutAction, &QAction::triggered, this, &MouseCrossApp::showAbout);
    
    m_quitAction = new QAction(tr("&Quit"), this);
    m_quitAction->setToolTip(tr("Exit MouseCross application completely"));
    m_quitAction->setStatusTip(tr("Close MouseCross and remove from system tray"));
    connect(m_quitAction, &QAction::triggered, this, &MouseCrossApp::quit);
}

void MouseCrossApp::createTrayIcon()
{
    m_trayMenu = std::make_unique<QMenu>();
    m_trayMenu->setAccessibleName(tr("MouseCross System Tray Menu"));
    m_trayMenu->setAccessibleDescription(tr("Context menu for MouseCross system tray icon with application controls"));
    m_trayMenu->addAction(m_toggleAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_settingsAction);
    m_trayMenu->addAction(m_aboutAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);
    
    m_trayIcon = std::make_unique<QSystemTrayIcon>(this);
    m_trayIcon->setContextMenu(m_trayMenu.get());
    
    // Create a simple programmatic icon as fallback
    QPixmap iconPixmap(32, 32);
    iconPixmap.fill(QColor(32, 45, 64));  // Dark background
    
    QPainter painter(&iconPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // White border
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(1, 1, 30, 30);
    
    // White crosshair
    painter.setPen(QPen(Qt::white, 3));
    // Horizontal line
    painter.drawLine(4, 16, 12, 16);
    painter.drawLine(20, 16, 28, 16);
    // Vertical line  
    painter.drawLine(16, 4, 16, 12);
    painter.drawLine(16, 20, 16, 28);
    
    painter.end();
    
    QIcon icon(iconPixmap);
    
    m_trayIcon->setIcon(icon);
    
    // Also set as application icon for all windows
    QApplication::setWindowIcon(icon);
    
    m_trayIcon->setToolTip(tr("MouseCross - Visual Mouse Locator"));
    
    connect(m_trayIcon.get(), &QSystemTrayIcon::activated,
            this, &MouseCrossApp::trayIconActivated);
}

void MouseCrossApp::setupHotkey()
{
#ifdef Q_OS_WIN
    registerHotkey();
#endif
#ifdef Q_OS_MAC
    registerHotkey();
#endif
}

void MouseCrossApp::showWelcomeIfFirstRun()
{
    if (m_settings->isFirstRun()) {
        WelcomeDialog* welcome = new WelcomeDialog();
        welcome->setAttribute(Qt::WA_DeleteOnClose);
        connect(welcome, &QDialog::finished, this, [this]() {
            m_settings->setFirstRun(false);
        });
        welcome->show();
        welcome->raise();
        welcome->activateWindow();
    }
}

void MouseCrossApp::showSettings()
{
    SettingsDialog dialog(m_settings.get());
    connect(&dialog, &SettingsDialog::settingsChanged, this, &MouseCrossApp::updateCrosshairFromSettings);
    dialog.exec();
}

void MouseCrossApp::showAbout()
{
    AboutDialog about;
    connect(&about, &AboutDialog::showWelcomeRequested, this, [this]() {
        WelcomeDialog* welcome = new WelcomeDialog();
        welcome->setAttribute(Qt::WA_DeleteOnClose);
        welcome->show();
        welcome->raise();
        welcome->activateWindow();
    });
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
#ifdef Q_OS_WIN
    updateHotkey();
#endif
#ifdef Q_OS_MAC
    updateHotkey();
#endif
}

#ifdef Q_OS_WIN
bool MouseCrossApp::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY && msg->wParam == HOTKEY_ID) {
            onHotkeyPressed();
            return true;
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

void MouseCrossApp::registerHotkey()
{
    QString hotkeyString = m_settings->toggleHotkey();
    QKeySequence keySequence(hotkeyString);
    
    if (keySequence.isEmpty()) {
        return;
    }
    
    // Parse the first key combination from the sequence
    int key = keySequence[0];
    
    // Extract modifiers and key code
    UINT modifiers = 0;
    if (key & Qt::ControlModifier) modifiers |= MOD_CONTROL;
    if (key & Qt::AltModifier) modifiers |= MOD_ALT;
    if (key & Qt::ShiftModifier) modifiers |= MOD_SHIFT;
    if (key & Qt::MetaModifier) modifiers |= MOD_WIN;
    
    // Extract the actual key (remove modifiers)
    int virtualKey = key & ~(Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier | Qt::MetaModifier);
    
    // Convert Qt key to Windows virtual key
    UINT winKey = virtualKey;
    if (virtualKey >= Qt::Key_A && virtualKey <= Qt::Key_Z) {
        winKey = virtualKey - Qt::Key_A + 'A';
    }
    
    RegisterHotKey(reinterpret_cast<HWND>(winId()), HOTKEY_ID, modifiers, winKey);
}

void MouseCrossApp::unregisterHotkey()
{
    UnregisterHotKey(reinterpret_cast<HWND>(winId()), HOTKEY_ID);
}

void MouseCrossApp::updateHotkey()
{
    unregisterHotkey();
    registerHotkey();
}
#endif

#ifdef Q_OS_MAC
OSStatus MouseCrossApp::hotKeyHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
    if (s_instance) {
        s_instance->onHotkeyPressed();
    }
    return noErr;
}

void MouseCrossApp::registerHotkey()
{
    QString hotkeyString = m_settings->toggleHotkey();
    if (hotkeyString.isEmpty()) {
        return;
    }
    
    // Parse the default Mac hotkey: Cmd+Option+Shift+C
    EventHotKeyID hotKeyID;
    hotKeyID.signature = 'MCCR';  // MouseCross signature
    hotKeyID.id = 1;
    
    // Define the key combination: Cmd+Option+Shift+C
    UInt32 modifiers = cmdKey | optionKey | shiftKey;
    UInt32 keyCode = kVK_ANSI_C;  // 'C' key
    
    // Install event handler
    EventTypeSpec eventType;
    eventType.eventClass = kEventClassKeyboard;
    eventType.eventKind = kEventHotKeyPressed;
    
    InstallApplicationEventHandler(&hotKeyHandler, 1, &eventType, this, nullptr);
    
    // Register the hotkey
    RegisterEventHotKey(keyCode, modifiers, hotKeyID, GetApplicationEventTarget(), 0, &m_hotKeyRef);
}

void MouseCrossApp::unregisterHotkey()
{
    if (m_hotKeyRef) {
        UnregisterEventHotKey(m_hotKeyRef);
        m_hotKeyRef = nullptr;
    }
}

void MouseCrossApp::updateHotkey()
{
    unregisterHotkey();
    registerHotkey();
}
#endif