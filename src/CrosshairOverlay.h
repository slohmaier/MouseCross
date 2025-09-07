#ifndef CROSSHAIROVERLAY_H
#define CROSSHAIROVERLAY_H

#include <QObject>
#include <memory>

class SettingsManager;
class CrosshairRenderer;

class CrosshairOverlay : public QObject
{
    Q_OBJECT

public:
    explicit CrosshairOverlay(QObject *parent = nullptr);
    ~CrosshairOverlay();
    
    void updateFromSettings(SettingsManager* settings);
    void show();
    void hide();
    bool isVisible() const;

private:
    std::unique_ptr<CrosshairRenderer> m_renderer;
};

#endif // CROSSHAIROVERLAY_H