#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QDialog>

class QLabel;
class QCheckBox;
class QPushButton;

class WelcomeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomeDialog(QWidget *parent = nullptr);

private slots:
    void onOpenSettings();

private:
    void setupUI();
    
    QLabel* m_titleLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_instructionsLabel;
    QCheckBox* m_dontShowAgainCheckBox;
    QPushButton* m_settingsButton;
    QPushButton* m_okButton;
};

#endif // WELCOMEDIALOG_H