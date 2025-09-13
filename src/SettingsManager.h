#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include "CrosshairRenderer.h"

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
    
    
    double crosshairOpacity() const;
    void setCrosshairOpacity(double opacity);
    
    bool showArrows() const;
    void setShowArrows(bool show);
    
    double circleSpacingIncrease() const;
    void setCircleSpacingIncrease(double percentage);
    
    CrosshairRenderer::DirectionShape directionShape() const;
    void setDirectionShape(CrosshairRenderer::DirectionShape shape);
    
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