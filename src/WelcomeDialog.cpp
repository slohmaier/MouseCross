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
    setWindowIcon(QIcon(":/icons/app_icon.png"));
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
    mainLayout->addWidget(m_titleLabel);
    
    // Description
    m_descriptionLabel = new QLabel(
        tr("MouseCross helps visually impaired users locate their mouse cursor "
           "by displaying a customizable crosshair overlay."), this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
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
           "Double-click the tray icon to toggle the crosshair."), this);
    m_instructionsLabel->setWordWrap(true);
    mainLayout->addWidget(m_instructionsLabel);
    
    mainLayout->addStretch();
    
    // Don't show again checkbox
    m_dontShowAgainCheckBox = new QCheckBox(tr("Don't show this welcome screen again"), this);
    m_dontShowAgainCheckBox->setChecked(true);
    mainLayout->addWidget(m_dontShowAgainCheckBox);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    
    m_settingsButton = new QPushButton(tr("Open Settings"), this);
    connect(m_settingsButton, &QPushButton::clicked, this, &WelcomeDialog::onOpenSettings);
    
    m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setDefault(true);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_settingsButton);
    buttonLayout->addWidget(m_okButton);
    
    mainLayout->addLayout(buttonLayout);
}

void WelcomeDialog::onOpenSettings()
{
    // Note: This would ideally open the settings dialog
    // For now, we'll just close the welcome dialog
    // The main application will handle settings opening
    accept();
}