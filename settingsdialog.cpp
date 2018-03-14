//
// Created by 216k155 on 3/6/18.
//

#include "settingsdialog.h"

#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent, QString* algo, int* numThreads, QString* pass) : QDialog(parent),
                                                              ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->algoComboBox->addItem(tr(" Blake256-14rounds (SFR)"), QVariant("blake"));
    ui->algoComboBox->addItem(tr(" Blake256-14rounds(DCR)"), QVariant("decred"));
    ui->algoComboBox->addItem(tr(" Blake256-8rounds (BLC)"), QVariant("blakecoin"));
    ui->algoComboBox->addItem(tr(" Blake256-8rounds (XVC)"), QVariant("vcash"));
    ui->algoComboBox->addItem(tr(" Blake2s (NEVA/XVG)"), QVariant("blake2s"));
    ui->algoComboBox->addItem(tr(" keccak256 (Maxcoin)"), QVariant("keccak"));
    ui->algoComboBox->addItem(tr(" X13+SM3 (Hshare)"), QVariant("hsr"));
    ui->algoComboBox->addItem(tr(" (LyraBar)"), QVariant("lyra2"));
    ui->algoComboBox->addItem(tr(" (VertCoin)"), QVariant("lyra2v2"));
    ui->algoComboBox->addItem(tr(" Skein SHA2 (AUR/DGB/SKC)"), QVariant("skein"));
    ui->algoComboBox->addItem(tr(" Double Skein (Woodcoin)"), QVariant("skein2"));
    ui->algoComboBox->addItem(tr(" NIST5 (Talkcoin/Power)"), QVariant("nist5"));
    ui->algoComboBox->addItem(tr(" Quark (Quarkcoin)"), QVariant("quark"));
    ui->algoComboBox->addItem(tr(" Qubit"), QVariant("qubit"));
    ui->algoComboBox->addItem(tr(" X11 (DarkCoin)"), QVariant("x11"));
    ui->algoComboBox->addItem(tr(" C11 (Chaincoin)"), QVariant("c11"));
    ui->algoComboBox->addItem(tr(" X11+gost (Sibcoin)"), QVariant("sib"));
    ui->algoComboBox->addItem(tr(" PHI1612 (LuxCoin)"), QVariant("phi"));
    ui->algoComboBox->addItem(tr(" Permuted x11 (Revolver)"), QVariant("x11evo"));
    ui->algoComboBox->addItem(tr(" X13 (MaruCoin)"), QVariant("x13"));
    ui->algoComboBox->addItem(tr(" X14 (BernCoin)"), QVariant("x14"));
    ui->algoComboBox->addItem(tr(" X15 (Joincoin)"), QVariant("x15"));
    ui->algoComboBox->addItem(tr(" X17 (XVG)"), QVariant("x17"));
    ui->algoComboBox->addItem(tr(" whirlpool (JoinCoin)"), QVariant("whirlpool"));
    ui->algoComboBox->addItem(tr(" Lbry (Library Credits)"), QVariant("lbry"));
    ui->algoComboBox->addItem(tr(" neoscrypt (FTC/PXC/UFO)"), QVariant("neoscrypt"));
    ui->algoComboBox->addItem(tr(" Sia (SIAcoin)"), QVariant("sia"));
    ui->algoComboBox->addItem(tr(" Myriad-Groestl (SFR/AUR/DGB/XVG/MYR)"), QVariant("myr-gr"));
    ui->algoComboBox->addItem(tr(" Thor's Riddle(?) (Veltor)"), QVariant("veltor"));

    algoValue = algo;
    numThr = numThreads;
    password = pass;
    int index = ui->algoComboBox->findData(*algoValue);
    if ( index != -1 ) { // -1 for not found
        ui->algoComboBox->setCurrentIndex(index);
    }

    if(*numThr > 0) {
        ui->gputhreadEdit->setText(QString::number(*numThr));
    }

    if((*password).length()) {
        ui->passEdit->setText(*password);
    }

    connect(ui->buttonBox, SIGNAL(clicked (QAbstractButton*)), this, SLOT(on_buttonBox_clicked(QAbstractButton*)));

}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button) {
    if(ui->buttonBox->standardButton(button) == QDialogButtonBox::Ok){
        printf("OK button pressed\n");
        *password = ui->passEdit->text();
        *numThr = ui->gputhreadEdit->text().toInt();
        *algoValue = ui->algoComboBox->currentData().toString();
        this->close();
    } else {
        this->close();
    }
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
}
