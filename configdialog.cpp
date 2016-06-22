#include "configdialog.h"
#include "ui_configdialog.h"
#include "settings.h"

#include <kconfigdialogmanager.h>

ConfigDialog::ConfigDialog(Settings* settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    setWindowTitle(i18n("Select a tomboy server"));
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    ui->setupUi(mainWidget);

    mManager = new KConfigDialogManager(this, settings);
    mManager->updateWidgets();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}
