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
#include <QApplication>
#include <QStyle>

WelcomeDialog::WelcomeDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle(tr("Welcome to MouseCross"));
    setFixedSize(450, 380); // Increased height to accommodate icon
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
    
    // Icon - matching AboutDialog style
    m_iconLabel = new QLabel(this);
    
    // Load the application icon
    QPixmap iconPixmap;
    iconPixmap.load(":/icons/icons/app_icon_hires.png");
    if (iconPixmap.isNull()) {
        iconPixmap.load(":/icons/icons/app_icon.png");
    }
    
    if (!iconPixmap.isNull()) {
        // Scale to a reasonable size for the welcome dialog
        int targetSize = 80;  // Slightly smaller than AboutDialog
        iconPixmap = iconPixmap.scaled(targetSize, targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        // Last resort: use system icon
        iconPixmap = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon).pixmap(80, 80);
    }
    
    m_iconLabel->setPixmap(iconPixmap);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setScaledContents(false);
    
    // Accessibility for icon
    m_iconLabel->setAccessibleName(tr("MouseCross Application Icon"));
    m_iconLabel->setAccessibleDescription(tr("MouseCross crosshair logo - a crosshair with rounded rectangle"));
    
    // Center the icon label in the layout
    mainLayout->addWidget(m_iconLabel, 0, Qt::AlignCenter);
    
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
        tr("Key features:<br/>"
           "• Always-on-top crosshair overlay<br/>"
           "• Customizable appearance and size<br/>"
           "• Inverted mode for visibility on any background<br/>"
#ifdef Q_OS_MAC
           "• Default hotkey: <b>⌘⌥⇧C</b> (Cmd+Option+Shift+C)<br/>"
#else
           "• Default hotkey: <b>Ctrl+Alt+Shift+C</b><br/>"
#endif
#ifdef Q_OS_WIN
           "• Auto-start with Windows<br/><br/>"
#elif defined(Q_OS_MAC)
           "• Auto-start with macOS<br/><br/>"
#else
           "• Auto-start with system<br/><br/>"
#endif
           "The application runs from the system tray. "
           "Double-click the tray icon to toggle the crosshair.<br/><br/>"
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