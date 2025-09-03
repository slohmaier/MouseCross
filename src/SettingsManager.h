#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QColor>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);
    
    // First run
    bool isFirstRun() const;
    void setFirstRun(bool firstRun);
    
    // Crosshair appearance
    int crosshairLineWidth() const;
    void setCrosshairLineWidth(int width);
    
    int crosshairOffsetFromCursor() const;
    void setCrosshairOffsetFromCursor(int offset);
    
    double crosshairThicknessMultiplier() const;
    void setCrosshairThicknessMultiplier(double multiplier);
    
    QColor crosshairColor() const;
    void setCrosshairColor(const QColor& color);
    
    bool invertedMode() const;
    void setInvertedMode(bool inverted);
    
    double crosshairOpacity() const;
    void setCrosshairOpacity(double opacity);
    
    // Behavior
    bool autoStart() const;
    void setAutoStart(bool enabled);
    
    bool activateOnStart() const;
    void setActivateOnStart(bool activate);
    
    // Hotkey
    QString toggleHotkey() const;
    void setToggleHotkey(const QString& hotkey);

signals:
    void settingsChanged();

private:
    QSettings m_settings;
    
    void setDefaultValues();
};

#endif // SETTINGSMANAGER_H