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
        m_settings.setValue("crosshair/lineWidth", 4);
    }
    
    if (!m_settings.contains("crosshair/offsetFromCursor")) {
        m_settings.setValue("crosshair/offsetFromCursor", 49);
    }
    
    if (!m_settings.contains("crosshair/thicknessMultiplier")) {
        m_settings.setValue("crosshair/thicknessMultiplier", 3.0);
    }
    
    if (!m_settings.contains("crosshair/color")) {
        m_settings.setValue("crosshair/color", QColor(Qt::white).name());
    }
    
    
    if (!m_settings.contains("crosshair/opacity")) {
        m_settings.setValue("crosshair/opacity", 0.8);
    }
    
    if (!m_settings.contains("crosshair/showArrows")) {
        m_settings.setValue("crosshair/showArrows", true);
    }
    
    if (!m_settings.contains("crosshair/circleSpacingIncrease")) {
        m_settings.setValue("crosshair/circleSpacingIncrease", 5.0);  // Default 5%
    }
    
    if (!m_settings.contains("behavior/autoStart")) {
        m_settings.setValue("behavior/autoStart", false);
    }
    
    if (!m_settings.contains("behavior/activateOnStart")) {
        m_settings.setValue("behavior/activateOnStart", true);
    }
    
    if (!m_settings.contains("hotkey/toggle")) {
#ifdef Q_OS_MAC
        // On Mac, use Cmd+Option+Shift+C (more natural for Mac users)
        m_settings.setValue("hotkey/toggle", "Cmd+Option+Shift+C");
#else
        // On Windows/Linux, use Ctrl+Alt+Shift+C
        m_settings.setValue("hotkey/toggle", "Ctrl+Alt+Shift+C");
#endif
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
    return m_settings.value("crosshair/lineWidth", 4).toInt();
}

void SettingsManager::setCrosshairLineWidth(int width)
{
    m_settings.setValue("crosshair/lineWidth", width);
    emit settingsChanged();
}

int SettingsManager::crosshairOffsetFromCursor() const
{
    return m_settings.value("crosshair/offsetFromCursor", 49).toInt();
}

void SettingsManager::setCrosshairOffsetFromCursor(int offset)
{
    m_settings.setValue("crosshair/offsetFromCursor", offset);
    emit settingsChanged();
}

double SettingsManager::crosshairThicknessMultiplier() const
{
    return m_settings.value("crosshair/thicknessMultiplier", 3.0).toDouble();
}

void SettingsManager::setCrosshairThicknessMultiplier(double multiplier)
{
    m_settings.setValue("crosshair/thicknessMultiplier", multiplier);
    emit settingsChanged();
}

QColor SettingsManager::crosshairColor() const
{
    return QColor(m_settings.value("crosshair/color", QColor(Qt::white).name()).toString());
}

void SettingsManager::setCrosshairColor(const QColor& color)
{
    m_settings.setValue("crosshair/color", color.name());
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

bool SettingsManager::showArrows() const
{
    return m_settings.value("crosshair/showArrows", true).toBool();
}

void SettingsManager::setShowArrows(bool show)
{
    m_settings.setValue("crosshair/showArrows", show);
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
    return m_settings.value("behavior/activateOnStart", true).toBool();
}

void SettingsManager::setActivateOnStart(bool activate)
{
    m_settings.setValue("behavior/activateOnStart", activate);
    emit settingsChanged();
}

QString SettingsManager::toggleHotkey() const
{
#ifdef Q_OS_MAC
    return m_settings.value("hotkey/toggle", "Cmd+Option+Shift+C").toString();
#else
    return m_settings.value("hotkey/toggle", "Ctrl+Alt+Shift+C").toString();
#endif
}

void SettingsManager::setToggleHotkey(const QString& hotkey)
{
    m_settings.setValue("hotkey/toggle", hotkey);
    emit settingsChanged();
}

double SettingsManager::circleSpacingIncrease() const
{
    return m_settings.value("crosshair/circleSpacingIncrease", 5.0).toDouble();
}

void SettingsManager::setCircleSpacingIncrease(double percentage)
{
    // Clamp to valid range: 1% to 10%
    percentage = qBound(1.0, percentage, 10.0);
    m_settings.setValue("crosshair/circleSpacingIncrease", percentage);
    emit settingsChanged();
}