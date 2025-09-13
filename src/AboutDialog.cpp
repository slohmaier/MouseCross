#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QPixmap>
#include <QIcon>
#include <QApplication>
#include <QStyle>
#include <QDebug>
#include <QPainter>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle(tr("About MouseCross"));
    // Let Qt automatically size the dialog based on content
    resize(sizeHint());
    setWindowIcon(QIcon(":/icons/icons/app_icon.png"));  // Fixed resource path
    
    // Accessibility improvements
    setAccessibleName(tr("About MouseCross Dialog"));
    setAccessibleDescription(tr("Information about MouseCross application including version, description and website link"));
}

void AboutDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);  // Reduced spacing to prevent overlap
    mainLayout->setContentsMargins(50, 40, 50, 40);  // Slightly reduced margins
    
    // Icon
    m_iconLabel = new QLabel(this);
    // Let the icon size itself based on content
    
    // Load the application icon
    QPixmap iconPixmap;
    iconPixmap.load(":/icons/icons/app_icon_hires.png");
    if (iconPixmap.isNull()) {
        iconPixmap.load(":/icons/icons/app_icon.png");
    }
    
    if (!iconPixmap.isNull()) {
        // Scale to a reasonable size for the dialog
        int targetSize = 96;  // Good balance between visibility and space
        iconPixmap = iconPixmap.scaled(targetSize, targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        // Last resort: use system icon
        iconPixmap = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon).pixmap(96, 96);
    }
    
    m_iconLabel->setPixmap(iconPixmap);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setScaledContents(false);  // Prevent automatic scaling that might clip
    
    // Accessibility for icon
    m_iconLabel->setAccessibleName(tr("MouseCross Application Icon"));
    m_iconLabel->setAccessibleDescription(tr("MouseCross crosshair logo - a crosshair with circle in center"));
    
    // Center the icon label in the layout
    mainLayout->addWidget(m_iconLabel, 0, Qt::AlignCenter);
    
    // Title
    m_titleLabel = new QLabel(tr("MouseCross"), this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setAccessibleName(tr("Application Name"));
    m_titleLabel->setAccessibleDescription(tr("MouseCross - The name of this application"));
    mainLayout->addWidget(m_titleLabel);
    
    // Version
    m_versionLabel = new QLabel(tr("Version 0.1.0"), this);
    m_versionLabel->setAlignment(Qt::AlignCenter);
    m_versionLabel->setAccessibleName(tr("Version Information"));
    m_versionLabel->setAccessibleDescription(tr("Current version of MouseCross application"));
    mainLayout->addWidget(m_versionLabel);
    
    // Description
    m_descriptionLabel = new QLabel(
        tr("A crosshair overlay application designed to help "
           "visually impaired users with tunnel vision locate "
           "their mouse cursor on the screen."), this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setMaximumWidth(400);  // Limit width for better wrapping
    m_descriptionLabel->setAccessibleName(tr("Application Description"));
    m_descriptionLabel->setAccessibleDescription(tr("Detailed description of MouseCross functionality and target users"));
    mainLayout->addWidget(m_descriptionLabel);
    
    mainLayout->addStretch();
    
    // Copyright and website
    m_copyrightLabel = new QLabel(
        tr("© 2024 MouseCross\n"
           "Built with Qt %1\n\n"
           "Visit: <a href=\"https://slohmaier.de/MouseCross\">https://slohmaier.de/MouseCross</a>").arg(QT_VERSION_STR), this);
    m_copyrightLabel->setAlignment(Qt::AlignCenter);
    m_copyrightLabel->setTextFormat(Qt::RichText);  // Enable rich text formatting for HTML links
    m_copyrightLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    m_copyrightLabel->setOpenExternalLinks(true);
    m_copyrightLabel->setAccessibleName(tr("Copyright and Website Information"));
    m_copyrightLabel->setAccessibleDescription(tr("Copyright notice, Qt version information, and website link to MouseCross project page"));
    m_copyrightLabel->setToolTip(tr("Click the website link to visit the MouseCross project page"));
    QFont copyrightFont = m_copyrightLabel->font();
    copyrightFont.setPointSize(10);  // Larger for better readability in bigger window
    m_copyrightLabel->setFont(copyrightFont);
    mainLayout->addWidget(m_copyrightLabel);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    
    m_welcomeButton = new QPushButton(tr("Show Welcome"), this);
    m_welcomeButton->setAccessibleName(tr("Show Welcome Dialog"));
    m_welcomeButton->setAccessibleDescription(tr("Opens the welcome dialog with application introduction and quick setup"));
    m_welcomeButton->setToolTip(tr("Click to show the welcome dialog with application overview"));
    connect(m_welcomeButton, &QPushButton::clicked, this, &AboutDialog::onShowWelcome);
    
    m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setDefault(true);
    m_okButton->setAccessibleName(tr("Close Dialog"));
    m_okButton->setAccessibleDescription(tr("Closes the About dialog and returns to the application"));
    m_okButton->setToolTip(tr("Close this dialog"));
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_welcomeButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Set up proper tab order for keyboard navigation
    setTabOrder(m_welcomeButton, m_okButton);
}

void AboutDialog::onShowWelcome()
{
    emit showWelcomeRequested();
    accept(); // Close the about dialog
}