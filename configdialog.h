#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

class KConfigDialogManager;

class Settings;

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(Settings* settings, QWidget *parent=Q_NULLPTR);
    ~ConfigDialog();

    void saveSettings();

private:
    Ui::ConfigDialog *ui;

    KConfigDialogManager *mManager;
    Settings *mSettings;
};

#endif // CONFIGDIALOG_H
