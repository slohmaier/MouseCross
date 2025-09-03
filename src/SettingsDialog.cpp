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
    setWindowIcon(QIcon(":/icons/app_icon.png"));
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
    
    m_closeButton = new QPushButton(tr("Close"), this);
    m_closeButton->setDefault(true);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_restoreDefaultsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);
    
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
    m_lineWidthSpinBox->setRange(3, 10);
    m_lineWidthSpinBox->setSuffix(" px");
    m_lineWidthSpinBox->setToolTip(tr("Base thickness (automatically scaled for high DPI displays)"));
    connect(m_lineWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::onLineWidthChanged);
    layout->addWidget(m_lineWidthSpinBox, row, 1);
    row++;
    
    // Offset from cursor
    layout->addWidget(new QLabel(tr("Offset from Cursor:"), this), row, 0);
    m_offsetSpinBox = new QSpinBox(this);
    m_offsetSpinBox->setRange(0, 100);
    m_offsetSpinBox->setSuffix(" px");
    m_offsetSpinBox->setToolTip(tr("Distance from mouse cursor where lines start"));
    connect(m_offsetSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::onOffsetChanged);
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
        onThicknessChanged(value);
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
        onOpacityChanged(value);
    });
    row++;
    
    // Color
    layout->addWidget(new QLabel(tr("Color:"), this), row, 0);
    m_colorButton = new QPushButton(this);
    m_colorButton->setFixedSize(60, 30);
    connect(m_colorButton, &QPushButton::clicked, this, &SettingsDialog::onColorButtonClicked);
    layout->addWidget(m_colorButton, row, 1);
    row++;
    
    // Show arrows
    m_showArrowsCheckBox = new QCheckBox(tr("Show direction arrows on inner line"), this);
    connect(m_showArrowsCheckBox, &QCheckBox::toggled, this, &SettingsDialog::onShowArrowsChanged);
    layout->addWidget(m_showArrowsCheckBox, row, 0, 1, 2);
}

void SettingsDialog::createBehaviorGroup()
{
    m_behaviorGroup = new QGroupBox(tr("Behavior"), this);
    auto* layout = new QVBoxLayout(m_behaviorGroup);
    
    m_autoStartCheckBox = new QCheckBox(tr("Start with Windows/macOS"), this);
    connect(m_autoStartCheckBox, &QCheckBox::toggled, this, &SettingsDialog::onAutoStartChanged);
    layout->addWidget(m_autoStartCheckBox);
    
    m_activateOnStartCheckBox = new QCheckBox(tr("Activate crosshair on application start"), this);
    connect(m_activateOnStartCheckBox, &QCheckBox::toggled, this, &SettingsDialog::onActivateOnStartChanged);
    layout->addWidget(m_activateOnStartCheckBox);
}

void SettingsDialog::createHotkeyGroup()
{
    m_hotkeyGroup = new QGroupBox(tr("Hotkey"), this);
    auto* layout = new QHBoxLayout(m_hotkeyGroup);
    
    layout->addWidget(new QLabel(tr("Toggle hotkey:"), this));
    m_hotkeyEdit = new QKeySequenceEdit(this);
    connect(m_hotkeyEdit, &QKeySequenceEdit::keySequenceChanged, this, &SettingsDialog::onHotkeyChanged);
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
    
    m_showArrowsCheckBox->setChecked(m_settings->showArrows());
    
    m_autoStartCheckBox->setChecked(m_settings->autoStart());
    m_activateOnStartCheckBox->setChecked(m_settings->activateOnStart());
    
    m_hotkeyEdit->setKeySequence(QKeySequence(m_settings->toggleHotkey()));
}


void SettingsDialog::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_currentColor, this, tr("Select Crosshair Color"));
    if (color.isValid()) {
        m_currentColor = color;
        updateColorButton();
        m_settings->setCrosshairColor(m_currentColor);
        emit settingsChanged();
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
    m_lineWidthSpinBox->setValue(4);
    m_offsetSpinBox->setValue(49);
    m_thicknessSlider->setValue(30); // 3.0x
    m_opacitySlider->setValue(80);
    
    m_currentColor = Qt::white;
    updateColorButton();
    
    m_showArrowsCheckBox->setChecked(true);
    
    m_autoStartCheckBox->setChecked(false);
    m_activateOnStartCheckBox->setChecked(true);
    
    m_hotkeyEdit->setKeySequence(QKeySequence("Ctrl+Alt+Shift+C"));
}

// Immediate settings application slots
void SettingsDialog::onLineWidthChanged(int value)
{
    m_settings->setCrosshairLineWidth(value);
    emit settingsChanged();
}

void SettingsDialog::onOffsetChanged(int value)
{
    m_settings->setCrosshairOffsetFromCursor(value);
    emit settingsChanged();
}

void SettingsDialog::onThicknessChanged(int value)
{
    m_settings->setCrosshairThicknessMultiplier(value / 10.0);
    emit settingsChanged();
}

void SettingsDialog::onOpacityChanged(int value)
{
    m_settings->setCrosshairOpacity(value / 100.0);
    emit settingsChanged();
}

void SettingsDialog::onShowArrowsChanged(bool checked)
{
    m_settings->setShowArrows(checked);
    emit settingsChanged();
}

void SettingsDialog::onAutoStartChanged(bool checked)
{
    m_settings->setAutoStart(checked);
    emit settingsChanged();
}

void SettingsDialog::onActivateOnStartChanged(bool checked)
{
    m_settings->setActivateOnStart(checked);
    emit settingsChanged();
}

void SettingsDialog::onHotkeyChanged(const QKeySequence& keySequence)
{
    m_settings->setToggleHotkey(keySequence.toString());
    emit settingsChanged();
}