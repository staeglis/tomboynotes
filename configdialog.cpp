#include <kconfigdialogmanager.h>
#include "configdialog.h"
#include "settings.h"
#include "ui_configdialog.h"

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
    if ( (ui->kcfg_ServerURL->text() != mSettings->serverURL()) ) {
        mSettings->setRequestToken("");
        mSettings->setRequestTokenSecret("");
    }

    if(ui->kcfg_collectionName->text() == "") {
        ui->kcfg_collectionName->setText("Tomboy Notes " + Settings::serverURL());
    }

    mManager->updateSettings();
    mSettings->save();
}
