#include "configdialog.h"
#include "ui_configdialog.h"
#include "settings.h"

#include <kconfigdialogmanager.h>

ConfigDialog::ConfigDialog(Settings* settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    // Create window
    setWindowTitle(i18n("Select a tomboy server"));
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    ui->setupUi(mainWidget);

    // KSettings stuff
    mSettings = settings;
    mManager = new KConfigDialogManager(this, settings);
    mManager->updateWidgets();

    // Set the button actions
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConfigDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ConfigDialog::reject);

    // Load config dialog window size settings
    KConfigGroup group(KSharedConfig::openConfig(), "ConfigDialog");
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
            resize(size);
    }

    ui->kcfg_ServerURL->setReadOnly(!mSettings->requestToken().isEmpty());
    ui->kcfg_username->setReadOnly(!mSettings->requestToken().isEmpty());
}

ConfigDialog::~ConfigDialog()
{
    // Save config dialog window size
    KConfigGroup group(KSharedConfig::openConfig(), "ConfigDialog");
    group.writeEntry("Size", size());
    group.sync();

    delete ui;
}

void ConfigDialog::saveSettings()
{
    if ( (ui->kcfg_ServerURL->text() != mSettings->serverURL()) || (ui->kcfg_username->text() != mSettings->username()) ) {
        mSettings->setRequestToken("");
        mSettings->setRequestTokenSecret("");
    }

    mManager->updateSettings();
    mSettings->save();
}
