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
#include <QPainter>
#include <QStyledItemDelegate>

// Custom delegate to draw shape icons in the combobox
class ShapeItemDelegate : public QStyledItemDelegate
{
public:
    explicit ShapeItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        if (!index.isValid()) return;
        
        // Create a modified option with adjusted text rectangle to make room for icon
        QStyleOptionViewItem modifiedOption = option;
        modifiedOption.rect.adjust(32, 0, 0, 0); // Move text 32px to the right
        
        // Paint the background and default styling with adjusted text area
        QStyledItemDelegate::paint(painter, modifiedOption, index);
        
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        
        // Get the shape type from the index
        int shapeType = index.row();
        
        // Calculate icon area (left side of the original item rect)
        QRect iconRect = option.rect;
        iconRect.setWidth(28);
        iconRect.adjust(4, 4, -4, -4);
        
        // Set up drawing for shapes
        painter->setPen(QPen(option.palette.text().color(), 2));
        
        QRect shapeRect = iconRect.adjusted(4, 4, -4, -4);
        
        switch (shapeType) {
            case 0: // Circle - filled like the actual crosshair
                painter->setBrush(QBrush(option.palette.text().color()));
                painter->setPen(Qt::NoPen);
                painter->drawEllipse(shapeRect);
                break;
            case 1: // Arrow pointing to center
            {
                painter->setBrush(QBrush(option.palette.text().color()));
                painter->setPen(Qt::NoPen);
                
                int cx = shapeRect.center().x();
                int cy = shapeRect.center().y();
                int size = shapeRect.width() / 3;
                int thickness = 4; // Match cross thickness
                
                // Create a thicker arrow pointing inward (left)
                QPolygon arrow;
                arrow << QPoint(cx + size, cy - thickness)      // Top of base
                      << QPoint(cx, cy)                         // Tip
                      << QPoint(cx + size, cy + thickness)      // Bottom of base
                      << QPoint(cx + size - thickness/2, cy);   // Inner base point
                
                painter->drawPolygon(arrow);
                break;
            }
            case 2: // Cross - properly proportioned for icon display
            {
                painter->setBrush(QBrush(option.palette.text().color()));
                painter->setPen(Qt::NoPen);
                
                int cx = shapeRect.center().x();
                int cy = shapeRect.center().y();
                int size = shapeRect.width() * 2 / 5; // Make arms longer for better cross appearance
                int thickness = 3; // Thinner for icon representation
                
                // Horizontal line
                QRect hLine(cx - size, cy - thickness/2, size * 2, thickness);
                painter->drawRect(hLine);
                
                // Vertical line
                QRect vLine(cx - thickness/2, cy - size, thickness, size * 2);
                painter->drawRect(vLine);
                break;
            }
            case 3: // Raute (Diamond)
            {
                painter->setBrush(QBrush(option.palette.text().color()));
                painter->setPen(Qt::NoPen);
                
                int cx = shapeRect.center().x();
                int cy = shapeRect.center().y();
                int size = shapeRect.width() / 3;
                
                // Create diamond shape
                QPolygon diamond;
                diamond << QPoint(cx, cy - size)        // Top
                        << QPoint(cx + size, cy)        // Right  
                        << QPoint(cx, cy + size)        // Bottom
                        << QPoint(cx - size, cy);       // Left
                
                painter->drawPolygon(diamond);
                break;
            }
        }
        
        painter->restore();
    }
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(qMax(size.height(), 28)); // Ensure minimum height for icons
        size.setWidth(size.width() + 32); // Add space for the icon
        return size;
    }
};

// Custom combobox that shows icons in both closed and open states
class ShapeComboBox : public QComboBox
{
public:
    explicit ShapeComboBox(QWidget *parent = nullptr) : QComboBox(parent) {
        // Add left padding to the text to make room for the icon
        setStyleSheet("QComboBox { padding-left: 32px; }");
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        // Paint the normal combobox background
        QComboBox::paintEvent(event);
        
        // If we have a valid current selection, draw the icon
        if (currentIndex() >= 0 && currentIndex() <= 3) {
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);
            
            // Calculate icon area (left side)
            QRect iconRect = rect();
            iconRect.setWidth(28);
            iconRect.adjust(4, 4, -4, -4);
            
            QRect shapeRect = iconRect.adjusted(4, 4, -4, -4);
            
            // Set up drawing
            painter.setBrush(QBrush(palette().text().color()));
            painter.setPen(Qt::NoPen);
            
            switch (currentIndex()) {
                case 0: // Circle
                    painter.drawEllipse(shapeRect);
                    break;
                case 1: // Arrow
                {
                    int cx = shapeRect.center().x();
                    int cy = shapeRect.center().y();
                    int size = shapeRect.width() / 3;
                    int thickness = 4;
                    
                    QPolygon arrow;
                    arrow << QPoint(cx + size, cy - thickness)
                          << QPoint(cx, cy)
                          << QPoint(cx + size, cy + thickness)
                          << QPoint(cx + size - thickness/2, cy);
                    
                    painter.drawPolygon(arrow);
                    break;
                }
                case 2: // Cross
                {
                    int cx = shapeRect.center().x();
                    int cy = shapeRect.center().y();
                    int size = shapeRect.width() * 2 / 5; // Make arms longer for better cross appearance
                    int thickness = 3; // Thinner for icon representation
                    
                    // Horizontal line
                    QRect hLine(cx - size, cy - thickness/2, size * 2, thickness);
                    painter.drawRect(hLine);
                    
                    // Vertical line
                    QRect vLine(cx - thickness/2, cy - size, thickness, size * 2);
                    painter.drawRect(vLine);
                    break;
                }
                case 3: // Raute (Diamond)
                {
                    int cx = shapeRect.center().x();
                    int cy = shapeRect.center().y();
                    int size = shapeRect.width() / 3;
                    
                    // Create diamond shape
                    QPolygon diamond;
                    diamond << QPoint(cx, cy - size)        // Top
                            << QPoint(cx + size, cy)        // Right  
                            << QPoint(cx, cy + size)        // Bottom
                            << QPoint(cx - size, cy);       // Left
                    
                    painter.drawPolygon(diamond);
                    break;
                }
            }
        }
    }
};

SettingsDialog::SettingsDialog(SettingsManager* settings, QWidget *parent)
    : QDialog(parent)
    , m_settings(settings)
    , m_currentColor(Qt::red)
{
    setupUI();
    loadSettings();
    setWindowTitle(tr("MouseCross Settings"));
    setFixedSize(450, 500);
    setWindowIcon(QIcon(":/icons/icons/app_icon.png"));
    
    // Accessibility improvements
    setAccessibleName(tr("MouseCross Settings Dialog"));
    setAccessibleDescription(tr("Configure MouseCross crosshair appearance, behavior and keyboard shortcuts"));
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
    m_restoreDefaultsButton->setAccessibleName(tr("Restore Default Settings"));
    m_restoreDefaultsButton->setAccessibleDescription(tr("Reset all settings to their original default values"));
    m_restoreDefaultsButton->setToolTip(tr("Click to reset all crosshair settings to default values"));
    connect(m_restoreDefaultsButton, &QPushButton::clicked, this, &SettingsDialog::onRestoreDefaults);
    
    m_closeButton = new QPushButton(tr("Close"), this);
    m_closeButton->setDefault(true);
    m_closeButton->setAccessibleName(tr("Close Settings Dialog"));
    m_closeButton->setAccessibleDescription(tr("Close the settings dialog and save current settings"));
    m_closeButton->setToolTip(tr("Close settings dialog - changes are saved automatically"));
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_restoreDefaultsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Set up proper tab order for keyboard navigation
    setTabOrder(m_lineWidthSpinBox, m_offsetSpinBox);
    setTabOrder(m_offsetSpinBox, m_thicknessSlider);
    setTabOrder(m_thicknessSlider, m_opacitySlider);
    setTabOrder(m_opacitySlider, m_colorButton);
    setTabOrder(m_colorButton, m_showArrowsCheckBox);
    setTabOrder(m_showArrowsCheckBox, m_shapeSpacingSlider);
    setTabOrder(m_shapeSpacingSlider, m_directionShapeCombo);
    setTabOrder(m_directionShapeCombo, m_autoStartCheckBox);
    setTabOrder(m_autoStartCheckBox, m_activateOnStartCheckBox);
    setTabOrder(m_activateOnStartCheckBox, m_hotkeyEdit);
    setTabOrder(m_hotkeyEdit, m_restoreDefaultsButton);
    setTabOrder(m_restoreDefaultsButton, m_closeButton);
}

void SettingsDialog::createAppearanceGroup()
{
    m_appearanceGroup = new QGroupBox(tr("Crosshair Appearance"), this);
    m_appearanceGroup->setAccessibleName(tr("Crosshair Appearance Settings"));
    m_appearanceGroup->setAccessibleDescription(tr("Configure visual properties of the crosshair overlay"));
    auto* layout = new QGridLayout(m_appearanceGroup);
    
    // Let Qt handle automatic spacing
    layout->setVerticalSpacing(-1); // Use default spacing
    layout->setHorizontalSpacing(-1); // Use default spacing
    
    // Line width
    QLabel* lineWidthLabel = new QLabel(tr("Line Width:"), this);
    layout->addWidget(lineWidthLabel, 0, 0);
    m_lineWidthSpinBox = new QSpinBox(this);
    m_lineWidthSpinBox->setRange(3, 10);
    m_lineWidthSpinBox->setSuffix(" px");
    m_lineWidthSpinBox->setToolTip(tr("Base thickness (automatically scaled for high DPI displays)"));
    m_lineWidthSpinBox->setAccessibleName(tr("Line Width"));
    m_lineWidthSpinBox->setAccessibleDescription(tr("Set the base thickness of crosshair lines in pixels"));
    lineWidthLabel->setBuddy(m_lineWidthSpinBox);
    connect(m_lineWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::onLineWidthChanged);
    layout->addWidget(m_lineWidthSpinBox, 0, 1);
    
    // Offset from cursor
    QLabel* offsetLabel = new QLabel(tr("Offset from Cursor:"), this);
    layout->addWidget(offsetLabel, 1, 0);
    m_offsetSpinBox = new QSpinBox(this);
    m_offsetSpinBox->setRange(0, 100);
    m_offsetSpinBox->setSuffix(" px");
    m_offsetSpinBox->setToolTip(tr("Distance from mouse cursor where lines start"));
    m_offsetSpinBox->setAccessibleName(tr("Cursor Offset"));
    m_offsetSpinBox->setAccessibleDescription(tr("Distance in pixels from mouse cursor where crosshair lines begin"));
    offsetLabel->setBuddy(m_offsetSpinBox);
    connect(m_offsetSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::onOffsetChanged);
    layout->addWidget(m_offsetSpinBox, 1, 1);
    
    // Thickness multiplier
    QLabel* thicknessGrowthLabel = new QLabel(tr("Thickness Growth:"), this);
    layout->addWidget(thicknessGrowthLabel, 2, 0);
    auto* thicknessLayout = new QHBoxLayout();
    m_thicknessSlider = new QSlider(Qt::Horizontal, this);
    m_thicknessSlider->setRange(10, 100); // 1.0x to 10.0x
    m_thicknessSlider->setAccessibleName(tr("Thickness Growth Factor"));
    m_thicknessSlider->setAccessibleDescription(tr("How much thicker the crosshair lines become towards the edges"));
    m_thicknessSlider->setToolTip(tr("Controls how much the crosshair lines thicken from center to edges"));
    thicknessGrowthLabel->setBuddy(m_thicknessSlider);
    m_thicknessLabel = new QLabel("3.0x", this);
    m_thicknessLabel->setAccessibleName(tr("Current Thickness Multiplier"));
    thicknessLayout->addWidget(m_thicknessSlider);
    thicknessLayout->addWidget(m_thicknessLabel);
    layout->addLayout(thicknessLayout, 2, 1);
    
    connect(m_thicknessSlider, &QSlider::valueChanged, this, [this](int value) {
        double multiplier = value / 10.0;
        m_thicknessLabel->setText(QString("%1x").arg(multiplier, 0, 'f', 1));
        onThicknessChanged(value);
    });
    
    // Opacity
    QLabel* opacityLabel = new QLabel(tr("Opacity:"), this);
    layout->addWidget(opacityLabel, 3, 0);
    auto* opacityLayout = new QHBoxLayout();
    m_opacitySlider = new QSlider(Qt::Horizontal, this);
    m_opacitySlider->setRange(10, 100);
    m_opacitySlider->setAccessibleName(tr("Crosshair Opacity"));
    m_opacitySlider->setAccessibleDescription(tr("Transparency level of the crosshair overlay"));
    m_opacitySlider->setToolTip(tr("Adjust how transparent or opaque the crosshair appears"));
    opacityLabel->setBuddy(m_opacitySlider);
    m_opacityLabel = new QLabel("100%", this);
    m_opacityLabel->setAccessibleName(tr("Current Opacity Percentage"));
    opacityLayout->addWidget(m_opacitySlider);
    opacityLayout->addWidget(m_opacityLabel);
    layout->addLayout(opacityLayout, 3, 1);
    
    connect(m_opacitySlider, &QSlider::valueChanged, this, [this](int value) {
        m_opacityLabel->setText(QString("%1%").arg(value));
        onOpacityChanged(value);
    });
    
    // Color
    QLabel* colorLabel = new QLabel(tr("Color:"), this);
    layout->addWidget(colorLabel, 4, 0);
    m_colorButton = new QPushButton(this);
    m_colorButton->setFixedSize(60, 30);
    m_colorButton->setAccessibleName(tr("Crosshair Color"));
    m_colorButton->setAccessibleDescription(tr("Choose the color for the crosshair lines"));
    m_colorButton->setToolTip(tr("Click to open color picker and select crosshair color"));
    colorLabel->setBuddy(m_colorButton);
    connect(m_colorButton, &QPushButton::clicked, this, &SettingsDialog::onColorButtonClicked);
    layout->addWidget(m_colorButton, 4, 1);
    
    // Show direction shapes
    m_showArrowsCheckBox = new QCheckBox(tr("Show direction shapes along crosshair"), this);
    m_showArrowsCheckBox->setAccessibleName(tr("Show Direction Shapes"));
    m_showArrowsCheckBox->setAccessibleDescription(tr("Display direction shapes along the crosshair lines"));
    m_showArrowsCheckBox->setToolTip(tr("Add visual direction indicators along the crosshair lines"));
    connect(m_showArrowsCheckBox, &QCheckBox::toggled, this, &SettingsDialog::onShowArrowsChanged);
    layout->addWidget(m_showArrowsCheckBox, 5, 0, 1, 2);
    
    // Direction shape spacing increase
    QLabel* spacingLabel = new QLabel(tr("Shape Spacing Growth:"), this);
    layout->addWidget(spacingLabel, 6, 0);
    auto* spacingLayout = new QHBoxLayout();
    m_shapeSpacingSlider = new QSlider(Qt::Horizontal, this);
    m_shapeSpacingSlider->setRange(1, 10); // 1% to 10%
    m_shapeSpacingSlider->setAccessibleName(tr("Shape Spacing Growth Rate"));
    m_shapeSpacingSlider->setAccessibleDescription(tr("How much shape spacing increases from center to edges"));
    m_shapeSpacingSlider->setToolTip(tr("Controls the rate at which shape spacing grows from cursor center to screen edges"));
    spacingLabel->setBuddy(m_shapeSpacingSlider);
    m_shapeSpacingLabel = new QLabel("5%", this);
    m_shapeSpacingLabel->setAccessibleName(tr("Current Spacing Growth Percentage"));
    spacingLayout->addWidget(m_shapeSpacingSlider);
    spacingLayout->addWidget(m_shapeSpacingLabel);
    layout->addLayout(spacingLayout, 6, 1);
    
    connect(m_shapeSpacingSlider, &QSlider::valueChanged, this, [this](int value) {
        m_shapeSpacingLabel->setText(QString("%1%").arg(value));
        onShapeSpacingChanged(value);
    });
    
    // Direction shape selection
    QLabel* shapeLabel = new QLabel(tr("Direction Shape:"), this);
    layout->addWidget(shapeLabel, 7, 0);
    m_directionShapeCombo = new ShapeComboBox(this);
    m_directionShapeCombo->addItem(tr("Circle"));
    m_directionShapeCombo->addItem(tr("Arrow (to center)"));
    m_directionShapeCombo->addItem(tr("Cross"));
    m_directionShapeCombo->addItem(tr("Raute"));
    m_directionShapeCombo->setAccessibleName(tr("Direction Shape"));
    m_directionShapeCombo->setAccessibleDescription(tr("Choose the shape drawn along crosshair lines"));
    m_directionShapeCombo->setToolTip(tr("Select the type of shape to display along the crosshair lines"));
    shapeLabel->setBuddy(m_directionShapeCombo);
    
    // Set custom delegate to show icons
    m_directionShapeCombo->setItemDelegate(new ShapeItemDelegate(m_directionShapeCombo));
    
    connect(m_directionShapeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &SettingsDialog::onDirectionShapeChanged);
    layout->addWidget(m_directionShapeCombo, 7, 1);
}

void SettingsDialog::createBehaviorGroup()
{
    m_behaviorGroup = new QGroupBox(tr("Behavior"), this);
    m_behaviorGroup->setAccessibleName(tr("Application Behavior Settings"));
    m_behaviorGroup->setAccessibleDescription(tr("Configure how MouseCross behaves when starting and running"));
    auto* layout = new QVBoxLayout(m_behaviorGroup);
    
#ifdef Q_OS_WIN
    m_autoStartCheckBox = new QCheckBox(tr("Start with Windows"), this);
#elif defined(Q_OS_MAC)
    m_autoStartCheckBox = new QCheckBox(tr("Start with macOS"), this);
#else
    m_autoStartCheckBox = new QCheckBox(tr("Start with system"), this);
#endif
    m_autoStartCheckBox->setAccessibleName(tr("Auto-start with System"));
    m_autoStartCheckBox->setAccessibleDescription(tr("Automatically start MouseCross when the operating system boots"));
    m_autoStartCheckBox->setToolTip(tr("MouseCross will start automatically when you log in to your computer"));
    connect(m_autoStartCheckBox, &QCheckBox::toggled, this, &SettingsDialog::onAutoStartChanged);
    layout->addWidget(m_autoStartCheckBox);
    
    m_activateOnStartCheckBox = new QCheckBox(tr("Activate crosshair on application start"), this);
    m_activateOnStartCheckBox->setAccessibleName(tr("Activate on Start"));
    m_activateOnStartCheckBox->setAccessibleDescription(tr("Show crosshair immediately when MouseCross application starts"));
    m_activateOnStartCheckBox->setToolTip(tr("The crosshair will be visible as soon as MouseCross starts"));
    connect(m_activateOnStartCheckBox, &QCheckBox::toggled, this, &SettingsDialog::onActivateOnStartChanged);
    layout->addWidget(m_activateOnStartCheckBox);
}

void SettingsDialog::createHotkeyGroup()
{
    m_hotkeyGroup = new QGroupBox(tr("Hotkey"), this);
    m_hotkeyGroup->setAccessibleName(tr("Keyboard Shortcut Settings"));
    m_hotkeyGroup->setAccessibleDescription(tr("Configure keyboard shortcut to toggle crosshair visibility"));
    auto* layout = new QHBoxLayout(m_hotkeyGroup);
    
    QLabel* hotkeyLabel = new QLabel(tr("Toggle hotkey:"), this);
    layout->addWidget(hotkeyLabel);
    m_hotkeyEdit = new QKeySequenceEdit(this);
    m_hotkeyEdit->setAccessibleName(tr("Toggle Hotkey"));
    m_hotkeyEdit->setAccessibleDescription(tr("Keyboard combination to show or hide the crosshair"));
    m_hotkeyEdit->setToolTip(tr("Press keys to set hotkey combination for toggling crosshair visibility"));
    hotkeyLabel->setBuddy(m_hotkeyEdit);
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
    
    m_shapeSpacingSlider->setValue(static_cast<int>(m_settings->circleSpacingIncrease()));
    
    m_directionShapeCombo->setCurrentIndex(static_cast<int>(m_settings->directionShape()));
    
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
    
    m_shapeSpacingSlider->setValue(5);
    
    m_directionShapeCombo->setCurrentIndex(0); // Circle
    
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

void SettingsDialog::onShapeSpacingChanged(int value)
{
    m_settings->setCircleSpacingIncrease(static_cast<double>(value));
    emit settingsChanged();
}

void SettingsDialog::onDirectionShapeChanged(int index)
{
    m_settings->setDirectionShape(static_cast<CrosshairRenderer::DirectionShape>(index));
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