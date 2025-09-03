#include "SettingsDialog.h"
#include "SettingsManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QKeySequenceEdit>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QApplication>
#include <QStyle>

SettingsDialog::SettingsDialog(SettingsManager* settings, QWidget *parent)
    : QDialog(parent)
    , m_settings(settings)
    , m_currentColor(Qt::red)
    , m_currentBackgroundColor(Qt::white)
{
    setupUI();
    loadSettings();
    setWindowTitle(tr("MouseCross Settings"));
    setFixedSize(450, 500);
}

void SettingsDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    
    createAppearanceGroup();
    createBehaviorGroup();
    createHotkeyGroup();
    
    mainLayout->addWidget(m_appearanceGroup);
    mainLayout->addWidget(m_behaviorGroup);
    mainLayout->addWidget(m_hotkeyGroup);
    
    mainLayout->addStretch();
    
    // Dialog buttons
    auto* buttonLayout = new QHBoxLayout();
    
    m_restoreDefaultsButton = new QPushButton(tr("Restore Defaults"), this);
    connect(m_restoreDefaultsButton, &QPushButton::clicked, this, &SettingsDialog::onRestoreDefaults);
    
    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okButton = buttonBox->button(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    buttonLayout->addWidget(m_restoreDefaultsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonBox);
    
    mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::createAppearanceGroup()
{
    m_appearanceGroup = new QGroupBox(tr("Crosshair Appearance"), this);
    auto* layout = new QGridLayout(m_appearanceGroup);
    
    int row = 0;
    
    // Line width
    layout->addWidget(new QLabel(tr("Line Width:"), this), row, 0);
    m_lineWidthSpinBox = new QSpinBox(this);
    m_lineWidthSpinBox->setRange(1, 10);
    m_lineWidthSpinBox->setSuffix(" px");
    layout->addWidget(m_lineWidthSpinBox, row, 1);
    row++;
    
    // Line length
    layout->addWidget(new QLabel(tr("Line Length:"), this), row, 0);
    m_lineLengthSpinBox = new QSpinBox(this);
    m_lineLengthSpinBox->setRange(5, 100);
    m_lineLengthSpinBox->setSuffix(" px");
    layout->addWidget(m_lineLengthSpinBox, row, 1);
    row++;
    
    // Clearance radius
    layout->addWidget(new QLabel(tr("Center Clearance:"), this), row, 0);
    m_clearanceSpinBox = new QSpinBox(this);
    m_clearanceSpinBox->setRange(0, 50);
    m_clearanceSpinBox->setSuffix(" px");
    layout->addWidget(m_clearanceSpinBox, row, 1);
    row++;
    
    // Opacity
    layout->addWidget(new QLabel(tr("Opacity:"), this), row, 0);
    auto* opacityLayout = new QHBoxLayout();
    m_opacitySlider = new QSlider(Qt::Horizontal, this);
    m_opacitySlider->setRange(10, 100);
    m_opacityLabel = new QLabel("100%", this);
    opacityLayout->addWidget(m_opacitySlider);
    opacityLayout->addWidget(m_opacityLabel);
    layout->addLayout(opacityLayout, row, 1);
    
    connect(m_opacitySlider, &QSlider::valueChanged, this, [this](int value) {
        m_opacityLabel->setText(QString("%1%").arg(value));
    });
    row++;
    
    // Color
    layout->addWidget(new QLabel(tr("Color:"), this), row, 0);
    m_colorButton = new QPushButton(this);
    m_colorButton->setFixedSize(60, 30);
    connect(m_colorButton, &QPushButton::clicked, this, &SettingsDialog::onColorButtonClicked);
    layout->addWidget(m_colorButton, row, 1);
    row++;
    
    // Background color
    layout->addWidget(new QLabel(tr("Background Color:"), this), row, 0);
    m_backgroundColorButton = new QPushButton(this);
    m_backgroundColorButton->setFixedSize(60, 30);
    connect(m_backgroundColorButton, &QPushButton::clicked, this, &SettingsDialog::onBackgroundColorButtonClicked);
    layout->addWidget(m_backgroundColorButton, row, 1);
    row++;
    
    // Show background
    m_showBackgroundCheckBox = new QCheckBox(tr("Show background circle"), this);
    layout->addWidget(m_showBackgroundCheckBox, row, 0, 1, 2);
    row++;
    
    // Inverted mode
    m_invertedModeCheckBox = new QCheckBox(tr("Inverted mode (visible on any background)"), this);
    layout->addWidget(m_invertedModeCheckBox, row, 0, 1, 2);
}

void SettingsDialog::createBehaviorGroup()
{
    m_behaviorGroup = new QGroupBox(tr("Behavior"), this);
    auto* layout = new QVBoxLayout(m_behaviorGroup);
    
    m_autoStartCheckBox = new QCheckBox(tr("Start with Windows/macOS"), this);
    layout->addWidget(m_autoStartCheckBox);
    
    m_activateOnStartCheckBox = new QCheckBox(tr("Activate crosshair on application start"), this);
    layout->addWidget(m_activateOnStartCheckBox);
}

void SettingsDialog::createHotkeyGroup()
{
    m_hotkeyGroup = new QGroupBox(tr("Hotkey"), this);
    auto* layout = new QHBoxLayout(m_hotkeyGroup);
    
    layout->addWidget(new QLabel(tr("Toggle hotkey:"), this));
    m_hotkeyEdit = new QKeySequenceEdit(this);
    layout->addWidget(m_hotkeyEdit);
}

void SettingsDialog::loadSettings()
{
    m_lineWidthSpinBox->setValue(m_settings->crosshairLineWidth());
    m_lineLengthSpinBox->setValue(m_settings->crosshairLineLength());
    m_clearanceSpinBox->setValue(m_settings->crosshairClearanceRadius());
    m_opacitySlider->setValue(static_cast<int>(m_settings->crosshairOpacity() * 100));
    
    m_currentColor = m_settings->crosshairColor();
    m_currentBackgroundColor = m_settings->crosshairBackgroundColor();
    updateColorButton();
    updateBackgroundColorButton();
    
    m_showBackgroundCheckBox->setChecked(m_settings->showCrosshairBackground());
    m_invertedModeCheckBox->setChecked(m_settings->invertedMode());
    
    m_autoStartCheckBox->setChecked(m_settings->autoStart());
    m_activateOnStartCheckBox->setChecked(m_settings->activateOnStart());
    
    m_hotkeyEdit->setKeySequence(QKeySequence(m_settings->toggleHotkey()));
}

void SettingsDialog::saveSettings()
{
    m_settings->setCrosshairLineWidth(m_lineWidthSpinBox->value());
    m_settings->setCrosshairLineLength(m_lineLengthSpinBox->value());
    m_settings->setCrosshairClearanceRadius(m_clearanceSpinBox->value());
    m_settings->setCrosshairOpacity(m_opacitySlider->value() / 100.0);
    
    m_settings->setCrosshairColor(m_currentColor);
    m_settings->setCrosshairBackgroundColor(m_currentBackgroundColor);
    
    m_settings->setShowCrosshairBackground(m_showBackgroundCheckBox->isChecked());
    m_settings->setInvertedMode(m_invertedModeCheckBox->isChecked());
    
    m_settings->setAutoStart(m_autoStartCheckBox->isChecked());
    m_settings->setActivateOnStart(m_activateOnStartCheckBox->isChecked());
    
    m_settings->setToggleHotkey(m_hotkeyEdit->keySequence().toString());
}

void SettingsDialog::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_currentColor, this, tr("Select Crosshair Color"));
    if (color.isValid()) {
        m_currentColor = color;
        updateColorButton();
    }
}

void SettingsDialog::onBackgroundColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_currentBackgroundColor, this, tr("Select Background Color"));
    if (color.isValid()) {
        m_currentBackgroundColor = color;
        updateBackgroundColorButton();
    }
}

void SettingsDialog::updateColorButton()
{
    QString styleSheet = QString("QPushButton { background-color: %1; border: 1px solid #333; }")
                        .arg(m_currentColor.name());
    m_colorButton->setStyleSheet(styleSheet);
}

void SettingsDialog::updateBackgroundColorButton()
{
    QString styleSheet = QString("QPushButton { background-color: %1; border: 1px solid #333; }")
                        .arg(m_currentBackgroundColor.name());
    m_backgroundColorButton->setStyleSheet(styleSheet);
}

void SettingsDialog::onPreviewModeChanged()
{
    // Future: Could add live preview functionality
}

void SettingsDialog::onRestoreDefaults()
{
    m_lineWidthSpinBox->setValue(2);
    m_lineLengthSpinBox->setValue(20);
    m_clearanceSpinBox->setValue(10);
    m_opacitySlider->setValue(80);
    
    m_currentColor = Qt::red;
    m_currentBackgroundColor = Qt::white;
    updateColorButton();
    updateBackgroundColorButton();
    
    m_showBackgroundCheckBox->setChecked(true);
    m_invertedModeCheckBox->setChecked(false);
    
    m_autoStartCheckBox->setChecked(false);
    m_activateOnStartCheckBox->setChecked(false);
    
    m_hotkeyEdit->setKeySequence(QKeySequence("Ctrl+Alt+C"));
}