#include "SettingsManager.h"
#include <QColor>
#include <QKeySequence>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings("MouseCross", "MouseCross")
{
    setDefaultValues();
}

void SettingsManager::setDefaultValues()
{
    // Set default values if they don't exist
    if (!m_settings.contains("firstRun")) {
        m_settings.setValue("firstRun", true);
    }
    
    if (!m_settings.contains("crosshair/lineWidth")) {
        m_settings.setValue("crosshair/lineWidth", 2);
    }
    
    if (!m_settings.contains("crosshair/offsetFromCursor")) {
        m_settings.setValue("crosshair/offsetFromCursor", 10);
    }
    
    if (!m_settings.contains("crosshair/color")) {
        m_settings.setValue("crosshair/color", QColor(Qt::red).name());
    }
    
    if (!m_settings.contains("crosshair/invertedMode")) {
        m_settings.setValue("crosshair/invertedMode", false);
    }
    
    if (!m_settings.contains("crosshair/opacity")) {
        m_settings.setValue("crosshair/opacity", 0.8);
    }
    
    if (!m_settings.contains("behavior/autoStart")) {
        m_settings.setValue("behavior/autoStart", false);
    }
    
    if (!m_settings.contains("behavior/activateOnStart")) {
        m_settings.setValue("behavior/activateOnStart", false);
    }
    
    if (!m_settings.contains("hotkey/toggle")) {
        m_settings.setValue("hotkey/toggle", "Ctrl+Alt+C");
    }
}

bool SettingsManager::isFirstRun() const
{
    return m_settings.value("firstRun", true).toBool();
}

void SettingsManager::setFirstRun(bool firstRun)
{
    m_settings.setValue("firstRun", firstRun);
    emit settingsChanged();
}

int SettingsManager::crosshairLineWidth() const
{
    return m_settings.value("crosshair/lineWidth", 2).toInt();
}

void SettingsManager::setCrosshairLineWidth(int width)
{
    m_settings.setValue("crosshair/lineWidth", width);
    emit settingsChanged();
}

int SettingsManager::crosshairOffsetFromCursor() const
{
    return m_settings.value("crosshair/offsetFromCursor", 10).toInt();
}

void SettingsManager::setCrosshairOffsetFromCursor(int offset)
{
    m_settings.setValue("crosshair/offsetFromCursor", offset);
    emit settingsChanged();
}

QColor SettingsManager::crosshairColor() const
{
    return QColor(m_settings.value("crosshair/color", QColor(Qt::red).name()).toString());
}

void SettingsManager::setCrosshairColor(const QColor& color)
{
    m_settings.setValue("crosshair/color", color.name());
    emit settingsChanged();
}

bool SettingsManager::invertedMode() const
{
    return m_settings.value("crosshair/invertedMode", false).toBool();
}

void SettingsManager::setInvertedMode(bool inverted)
{
    m_settings.setValue("crosshair/invertedMode", inverted);
    emit settingsChanged();
}

double SettingsManager::crosshairOpacity() const
{
    return m_settings.value("crosshair/opacity", 0.8).toDouble();
}

void SettingsManager::setCrosshairOpacity(double opacity)
{
    m_settings.setValue("crosshair/opacity", opacity);
    emit settingsChanged();
}

bool SettingsManager::autoStart() const
{
    return m_settings.value("behavior/autoStart", false).toBool();
}

void SettingsManager::setAutoStart(bool enabled)
{
    m_settings.setValue("behavior/autoStart", enabled);
    emit settingsChanged();
}

bool SettingsManager::activateOnStart() const
{
    return m_settings.value("behavior/activateOnStart", false).toBool();
}

void SettingsManager::setActivateOnStart(bool activate)
{
    m_settings.setValue("behavior/activateOnStart", activate);
    emit settingsChanged();
}

QString SettingsManager::toggleHotkey() const
{
    return m_settings.value("hotkey/toggle", "Ctrl+Alt+C").toString();
}

void SettingsManager::setToggleHotkey(const QString& hotkey)
{
    m_settings.setValue("hotkey/toggle", hotkey);
    emit settingsChanged();
}