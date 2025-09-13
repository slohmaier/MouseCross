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
    
    // Center the icon label in the layout
    mainLayout->addWidget(m_iconLabel, 0, Qt::AlignCenter);
    
    // Title
    m_titleLabel = new QLabel(tr("MouseCross"), this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_titleLabel);
    
    // Version
    m_versionLabel = new QLabel(tr("Version 0.1.0"), this);
    m_versionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_versionLabel);
    
    // Description
    m_descriptionLabel = new QLabel(
        tr("A crosshair overlay application designed to help "
           "visually impaired users with tunnel vision locate "
           "their mouse cursor on the screen."), this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setMaximumWidth(400);  // Limit width for better wrapping
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
    QFont copyrightFont = m_copyrightLabel->font();
    copyrightFont.setPointSize(10);  // Larger for better readability in bigger window
    m_copyrightLabel->setFont(copyrightFont);
    mainLayout->addWidget(m_copyrightLabel);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    
    m_welcomeButton = new QPushButton(tr("Show Welcome"), this);
    connect(m_welcomeButton, &QPushButton::clicked, this, &AboutDialog::onShowWelcome);
    
    m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setDefault(true);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_welcomeButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    
    mainLayout->addLayout(buttonLayout);
}

void AboutDialog::onShowWelcome()
{
    emit showWelcomeRequested();
    accept(); // Close the about dialog
}