/*
    Copyright (c) 2016 Stefan Stäglich

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include <kconfigdialogmanager.h>
#include "configdialog.h"
#include "settings.h"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(Settings* settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog),
    mSettings(settings)
{
    // Create window
    setWindowTitle(i18n("Select a tomboy server"));
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    ui->setupUi(mainWidget);

    // KSettings stuff. Works not in the initialization list!
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
    if ( ui->kcfg_ServerURL->text() != mSettings->serverURL() ) {
        mSettings->setRequestToken(QString());
        mSettings->setRequestTokenSecret(QString());
    }

    if(ui->kcfg_collectionName->text().isEmpty()) {
        ui->kcfg_collectionName->setText("Tomboy Notes " + Settings::serverURL());
    }

    mManager->updateSettings();
    mSettings->save();
}
