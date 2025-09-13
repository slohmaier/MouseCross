#include "WelcomeDialog.h"
#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QFont>
#include <QPixmap>
#include <QIcon>

WelcomeDialog::WelcomeDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle(tr("Welcome to MouseCross"));
    setFixedSize(450, 300);
    setWindowIcon(QIcon(":/icons/icons/app_icon.png"));
    
    // Accessibility improvements
    setAccessibleName(tr("MouseCross Welcome Dialog"));
    setAccessibleDescription(tr("Introduction to MouseCross features and initial setup options"));
}

void WelcomeDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Title
    m_titleLabel = new QLabel(tr("Welcome to MouseCross"), this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setAccessibleName(tr("Welcome Title"));
    m_titleLabel->setAccessibleDescription(tr("Welcome message for new MouseCross users"));
    mainLayout->addWidget(m_titleLabel);
    
    // Description
    m_descriptionLabel = new QLabel(
        tr("MouseCross helps visually impaired users locate their mouse cursor "
           "by displaying a customizable crosshair overlay."), this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setAccessibleName(tr("Application Description"));
    m_descriptionLabel->setAccessibleDescription(tr("Brief description of MouseCross purpose and functionality"));
    mainLayout->addWidget(m_descriptionLabel);
    
    // Instructions
    m_instructionsLabel = new QLabel(
        tr("Key features:\n"
           "• Always-on-top crosshair overlay\n"
           "• Customizable appearance and size\n"
           "• Inverted mode for visibility on any background\n"
           "• Configurable hotkey for quick toggle\n"
           "• Auto-start with Windows/macOS\n\n"
           "The application runs from the system tray. "
           "Double-click the tray icon to toggle the crosshair.\n\n"
           "More information: <a href=\"https://slohmaier.de/MouseCross\">https://slohmaier.de/MouseCross</a>"), this);
    m_instructionsLabel->setWordWrap(true);
    m_instructionsLabel->setTextFormat(Qt::RichText);
    m_instructionsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    m_instructionsLabel->setOpenExternalLinks(true);
    m_instructionsLabel->setAccessibleName(tr("Feature List and Instructions"));
    m_instructionsLabel->setAccessibleDescription(tr("Detailed list of MouseCross features and usage instructions"));
    m_instructionsLabel->setToolTip(tr("Overview of key features and how to use MouseCross"));
    mainLayout->addWidget(m_instructionsLabel);
    
    mainLayout->addStretch();
    
    // Don't show again checkbox
    m_dontShowAgainCheckBox = new QCheckBox(tr("Don't show this welcome screen again"), this);
    m_dontShowAgainCheckBox->setChecked(true);
    m_dontShowAgainCheckBox->setAccessibleName(tr("Don't Show Welcome Again"));
    m_dontShowAgainCheckBox->setAccessibleDescription(tr("Skip showing this welcome dialog on future application starts"));
    m_dontShowAgainCheckBox->setToolTip(tr("Check this to skip the welcome screen when MouseCross starts"));
    mainLayout->addWidget(m_dontShowAgainCheckBox);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    
    m_settingsButton = new QPushButton(tr("Open Settings"), this);
    m_settingsButton->setAccessibleName(tr("Open Settings Dialog"));
    m_settingsButton->setAccessibleDescription(tr("Open the settings dialog to configure crosshair appearance and behavior"));
    m_settingsButton->setToolTip(tr("Click to configure MouseCross settings"));
    connect(m_settingsButton, &QPushButton::clicked, this, &WelcomeDialog::onOpenSettings);
    
    m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setDefault(true);
    m_okButton->setAccessibleName(tr("Close Welcome Dialog"));
    m_okButton->setAccessibleDescription(tr("Close the welcome dialog and start using MouseCross"));
    m_okButton->setToolTip(tr("Close welcome dialog and begin using MouseCross"));
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_settingsButton);
    buttonLayout->addWidget(m_okButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Set up proper tab order for keyboard navigation
    setTabOrder(m_dontShowAgainCheckBox, m_settingsButton);
    setTabOrder(m_settingsButton, m_okButton);
}

void WelcomeDialog::onOpenSettings()
{
    // Note: This would ideally open the settings dialog
    // For now, we'll just close the welcome dialog
    // The main application will handle settings opening
    accept();
}