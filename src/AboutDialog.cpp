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

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle(tr("About MouseCross"));
    setFixedSize(350, 280);
    setWindowIcon(QIcon(":/icons/app_icon.png"));
}

void AboutDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Icon
    m_iconLabel = new QLabel(this);
    QPixmap iconPixmap(":/icons/app_icon.png");
    if (iconPixmap.isNull()) {
        iconPixmap = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon).pixmap(64, 64);
    } else {
        iconPixmap = iconPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    m_iconLabel->setPixmap(iconPixmap);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_iconLabel);
    
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
    mainLayout->addWidget(m_descriptionLabel);
    
    mainLayout->addStretch();
    
    // Copyright
    m_copyrightLabel = new QLabel(
        tr("© 2024 MouseCross\n"
           "Built with Qt %1\n\n"
           "https://slohmaier.de/MouseCross").arg(QT_VERSION_STR), this);
    m_copyrightLabel->setAlignment(Qt::AlignCenter);
    m_copyrightLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    m_copyrightLabel->setOpenExternalLinks(true);
    QFont copyrightFont = m_copyrightLabel->font();
    copyrightFont.setPointSize(8);
    m_copyrightLabel->setFont(copyrightFont);
    mainLayout->addWidget(m_copyrightLabel);
    
    // OK Button
    auto* buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setDefault(true);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
}