#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);

private:
    void setupUI();
    
    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    QLabel* m_versionLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_copyrightLabel;
    QPushButton* m_okButton;
};

#endif // ABOUTDIALOG_H