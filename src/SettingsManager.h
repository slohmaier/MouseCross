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
    
    int crosshairLineLength() const;
    void setCrosshairLineLength(int length);
    
    int crosshairClearanceRadius() const;
    void setCrosshairClearanceRadius(int radius);
    
    QColor crosshairColor() const;
    void setCrosshairColor(const QColor& color);
    
    QColor crosshairBackgroundColor() const;
    void setCrosshairBackgroundColor(const QColor& color);
    
    bool showCrosshairBackground() const;
    void setShowCrosshairBackground(bool show);
    
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