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
    
    // Offset from cursor
    layout->addWidget(new QLabel(tr("Offset from Cursor:"), this), row, 0);
    m_offsetSpinBox = new QSpinBox(this);
    m_offsetSpinBox->setRange(0, 100);
    m_offsetSpinBox->setSuffix(" px");
    m_offsetSpinBox->setToolTip(tr("Distance from mouse cursor where lines start"));
    layout->addWidget(m_offsetSpinBox, row, 1);
    row++;
    
    // Thickness multiplier
    layout->addWidget(new QLabel(tr("Thickness Growth:"), this), row, 0);
    auto* thicknessLayout = new QHBoxLayout();
    m_thicknessSlider = new QSlider(Qt::Horizontal, this);
    m_thicknessSlider->setRange(10, 100); // 1.0x to 10.0x
    m_thicknessLabel = new QLabel("3.0x", this);
    thicknessLayout->addWidget(m_thicknessSlider);
    thicknessLayout->addWidget(m_thicknessLabel);
    layout->addLayout(thicknessLayout, row, 1);
    
    connect(m_thicknessSlider, &QSlider::valueChanged, this, [this](int value) {
        double multiplier = value / 10.0;
        m_thicknessLabel->setText(QString("%1x").arg(multiplier, 0, 'f', 1));
    });
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
    m_offsetSpinBox->setValue(m_settings->crosshairOffsetFromCursor());
    m_thicknessSlider->setValue(static_cast<int>(m_settings->crosshairThicknessMultiplier() * 10));
    m_opacitySlider->setValue(static_cast<int>(m_settings->crosshairOpacity() * 100));
    
    m_currentColor = m_settings->crosshairColor();
    updateColorButton();
    
    m_invertedModeCheckBox->setChecked(m_settings->invertedMode());
    
    m_autoStartCheckBox->setChecked(m_settings->autoStart());
    m_activateOnStartCheckBox->setChecked(m_settings->activateOnStart());
    
    m_hotkeyEdit->setKeySequence(QKeySequence(m_settings->toggleHotkey()));
}

void SettingsDialog::saveSettings()
{
    m_settings->setCrosshairLineWidth(m_lineWidthSpinBox->value());
    m_settings->setCrosshairOffsetFromCursor(m_offsetSpinBox->value());
    m_settings->setCrosshairThicknessMultiplier(m_thicknessSlider->value() / 10.0);
    m_settings->setCrosshairOpacity(m_opacitySlider->value() / 100.0);
    
    m_settings->setCrosshairColor(m_currentColor);
    
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


void SettingsDialog::updateColorButton()
{
    QString styleSheet = QString("QPushButton { background-color: %1; border: 1px solid #333; }")
                        .arg(m_currentColor.name());
    m_colorButton->setStyleSheet(styleSheet);
}


void SettingsDialog::onPreviewModeChanged()
{
    // Future: Could add live preview functionality
}

void SettingsDialog::onRestoreDefaults()
{
    m_lineWidthSpinBox->setValue(2);
    m_offsetSpinBox->setValue(10);
    m_thicknessSlider->setValue(30); // 3.0x
    m_opacitySlider->setValue(80);
    
    m_currentColor = Qt::red;
    updateColorButton();
    
    m_invertedModeCheckBox->setChecked(false);
    
    m_autoStartCheckBox->setChecked(false);
    m_activateOnStartCheckBox->setChecked(false);
    
    m_hotkeyEdit->setKeySequence(QKeySequence("Ctrl+Alt+C"));
}