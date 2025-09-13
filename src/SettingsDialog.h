/*
 * MouseCross - A crosshair overlay application for visually impaired users
 * Copyright (C) 2025 Stefan Lohmaier <stefan@slohmaier.de>
 *
 * This file is part of MouseCross.
 *
 * MouseCross is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MouseCross is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MouseCross. If not, see <https://www.gnu.org/licenses/>.
 *
 * Project website: https://slohmaier.de/mousecross
 */

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QSpinBox;
class QSlider;
class QCheckBox;
class QComboBox;
class QPushButton;
class QLabel;
class QGroupBox;
class QKeySequenceEdit;
class SettingsManager;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(SettingsManager* settings, QWidget *parent = nullptr);

signals:
    void settingsChanged();

private slots:
    void onColorButtonClicked();
    void onPreviewModeChanged();
    void onRestoreDefaults();
    
    // Immediate settings application slots
    void onLineWidthChanged(int value);
    void onOffsetChanged(int value);
    void onThicknessChanged(int value);
    void onOpacityChanged(int value);
    void onShowArrowsChanged(bool checked);
    void onShapeSpacingChanged(int value);
    void onDirectionShapeChanged(int index);
    void onAutoStartChanged(bool checked);
    void onActivateOnStartChanged(bool checked);
    void onHotkeyChanged(const QKeySequence& keySequence);

private:
    void setupUI();
    void loadSettings();
    void updateColorButton();
    void createAppearanceGroup();
    void createBehaviorGroup();
    void createHotkeyGroup();
    
    SettingsManager* m_settings;
    
    // Appearance controls
    QGroupBox* m_appearanceGroup;
    QSpinBox* m_lineWidthSpinBox;
    QSpinBox* m_offsetSpinBox;
    QSlider* m_thicknessSlider;
    QLabel* m_thicknessLabel;
    QSlider* m_opacitySlider;
    QLabel* m_opacityLabel;
    QPushButton* m_colorButton;
    QCheckBox* m_showArrowsCheckBox;
    QSlider* m_shapeSpacingSlider;
    QLabel* m_shapeSpacingLabel;
    QComboBox* m_directionShapeCombo;
    
    // Behavior controls
    QGroupBox* m_behaviorGroup;
    QCheckBox* m_autoStartCheckBox;
    QCheckBox* m_activateOnStartCheckBox;
    
    // Hotkey controls
    QGroupBox* m_hotkeyGroup;
    QKeySequenceEdit* m_hotkeyEdit;
    
    // Dialog buttons
    QPushButton* m_closeButton;
    QPushButton* m_restoreDefaultsButton;
    
    QColor m_currentColor;
};

#endif // SETTINGSDIALOG_H