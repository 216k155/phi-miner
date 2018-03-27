// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdio.h>

#include "miningdialog.h"
//#include "guiutil.h"
#include "ui_miningdialog.h"
#include "device_info.h"
#include "customlistmodel.h"

#include <QModelIndex>
#include <QSettings>
#include <QString>
#include <QProcess>
#include <QCloseEvent>
#include <QMainWindow>
#include <QFontDatabase>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

#include <windows.h>


void MiningDialog::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit Mining",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        if(minerLogProcess) {
            minerLogProcess->deleteLater();
            minerLogProcess = NULL;
        }
        ui->textBrowser->setText("");
        event->accept();

    }
}

MiningDialog::MiningDialog(QWidget* parent) : QMainWindow(parent),
                                              ui(new Ui::MiningDialog),
                                              m_NeverShown(true),
                                              m_HistoryIndex(0)
{
    ui->setupUi(this);
    ui->stopButton->setEnabled(false);

    createListModelView();

    /* Open CSS when configured */
    //this->setStyleSheet(GUIUtil::loadStyleSheet());

    ui->textBrowser->setStyleSheet("background-color: black;");
    // ui->textBr owser->setTextColor( QColor( "red" ) );

    ui->poolComboBox->addItem(tr("bpool.io:3033"));
    ui->poolComboBox->addItem(tr("eu1.altminer.net:6667"));
    ui->poolComboBox->setCurrentIndex(0);

    connect(ui->benchmarkButton, SIGNAL(clicked()), this, SLOT(run_benchmark()));
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(run_mining()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stop_mining()));

    connect(ui->settingsButton, SIGNAL(clicked()), this, SLOT(openSettingsDialog()));

    connect(ui->exitButton, SIGNAL(clicked()), this, SLOT(exitButtonClicked()));

    QString store_prefix = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(store_prefix);
    if(!dir.exists())
    {
        dir.mkpath(store_prefix);
    }

    printf("DIRECTORY %s\n", store_prefix.toStdString().c_str());

    //QString desc = "Tra la la";//idx.data(TransactionTableModel::LongDescriptionRole).toString();
    //ui->detailText->setHtml(desc);
}

void MiningDialog::exitButtonClicked() {
    this->close();
}

void MiningDialog::openSettingsDialog(){

    settingsDialog = new SettingsDialog(this, &algo, &numThreads, &password);
    settingsDialog->show();
}

void MiningDialog::createListModelView(){
    //view = new QListView;
    QStringList colNames;
    colNames << "Video Cards" << "Compute Capability";

    //printDeviceProps();

    model = new CustomListModel(this);

    QTableView* table = ui->gpuListView;
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
//    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    table->setColumnWidth(0, this->width()/3);

    //model->setHeaderData( 1, Qt::Vertical, QObject::tr("Video Cards") );
    /*QStringList strList;
    strList << "monitor" << "mouse" << "keyboard" << "hard disk drive"
            << "graphic card" << "sound card" << "memory" << "motherboard";*/
    QList<QStringList> list;
    std::vector <std::string> props;
    QStringList strList;
    strList << "Video Card" << "Compute Capability";
    list.append(strList);
    strList.clear();
    int deviceCount = getDeviceCount();
    for(int i=0; i < deviceCount; i++) {
        props = getPropsOfIDevice(i);
        strList << props[0].data() << props[1].data();
        list.append(strList);
        strList.clear();
    }

    std::vector<int> minableDevices;
   // minableDevices.push_back(0);
   // minableDevices.push_back(1);
    minableDevices = getMinableDevices();
    model->setSelectableRows(minableDevices);


    /*QStringList strList1;
    strList1 << "monitor" << "2.0";
    list.append(strList1);
    //QStringList strList2;
    strList1.clear();
    strList1 << "keyboard" << "3.0";

    list.append(strList1);*/

    //printDeviceProps();



    model->loadData(list);
    //model->setStringList(strList);
    table->setModel(model);

    //model->getCheckedRows();



}


void MiningDialog::run_benchmark()
{

    std::string devices1 = model->getCheckedRows();
    printf("MY DEV %s\n", devices1.c_str());

    if(!devices1.size()) {
        QMessageBox::StandardButton resBtn = QMessageBox::warning( this, "No cards selected",
                                                                   tr("At least one videcard should be selected for mining\n"),
                                                                   QMessageBox::Ok);
        return;
    }


    if(benchmarkLogProcess) {
        ui->benchmarkButton->setText("Start benchmark");
        benchmarkLogProcess->deleteLater();
        benchmarkLogProcess = NULL;
    }
    else {
        printf("I PRESSED\n");
        ui->benchmarkButton->setText("Stop benchmark");
        ui->benchmarkButton->setEnabled(false);
        ui->settingsButton->setEnabled(false);
        ui->startButton->setEnabled(false);
        ui->helpButton->setEnabled(false);
        benchmarkLogProcess = new QProcess(this);
        connect(benchmarkLogProcess, SIGNAL(readyReadStandardOutput()),
                this, SLOT(ReadOut()));
        connect(benchmarkLogProcess, SIGNAL(readyReadStandardError()),
                this, SLOT(ReadErr()));
        connect(benchmarkLogProcess, SIGNAL(started()),
                this, SLOT(startedBenchmark()));
        connect(benchmarkLogProcess, SIGNAL(finished(int , QProcess::ExitStatus )),
                this, SLOT(finishedBenchmark(int , QProcess::ExitStatus )));
        benchmarkLogProcess->start("ccminer", QStringList() << "--benchmark" << "-d" << devices1.c_str());
    }

    // For debugging: Wait until the process has finished.
    //minerLogProcess->waitForFinished();


    //QProcess sh;
    //sh.start("sh");

    //sh.start("ccminer",QStringList() << "--benchmark");
    //sh.closeWriteChannel();

    //sh.waitForStarted();
    //QByteArray output = sh.readAll();
    //sh.close();
    //connect( &sh, SIGNAL(readyReadStandardOutput()), this, SLOT(ReadOut()) );
    //connect( &sh, SIGNAL(readyReadStandardError()), this, SLOT(ReadErr()) );
    //printf("%s \n", output.data());
}

QString MiningDialog::receiveWallet() {
    return "-u " + ui->walletInput->text();
}

QString MiningDialog::receivePool() {
    return "-o " + ui->poolComboBox->currentText();
}

QString MiningDialog::receiveDevices() {
    std::string devices1 = model->getCheckedRows();
    return "-d " + QString(devices1.c_str());
}

QString MiningDialog::receiveAlgoName() {
    return "-a " + algo;
}

QString MiningDialog::receivePassword() {
    return "-p " + password;
}

QString MiningDialog::receiveNumThreads() {
    return "-t " + QString::number(numThreads);
}

bool MiningDialog::createNewScriptFile() {
    QString filename = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)  + "/start.sh";
	char str[256];
	sprintf_s(str, "It works! - fi*********le: %s \n", osName().toStdString().c_str());
	//printf("my filename \n", filename.toStdString().c_str());
	OutputDebugString(str);
    QFile file(filename);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return false;
    }
    QTextStream out(&file);

    out << "#!/bin/bash\n\n";
    out << "while :\n";
    out << "do\n\n";

    out << "ccminer \\\n";
    out << receiveAlgoName() << " \\\n";
    out << receiveWallet() << " \\\n";
    if(password.length()) {
        out << receivePassword() << " \\\n";
    }
    out << receivePool() << " \\\n";

    if(numThreads > 0) {
        out << receiveNumThreads() << " \\\n";
    }

    out << receiveDevices() << " \n";

    out << "\n\nsleep 5\n";
    out << "done\n";

    file.close();
    file.setPermissions(QFile::ExeGroup | QFile::ExeOther | QFile::ExeOwner | QFile::ExeUser | QFile::ReadGroup | QFile::ReadOther | QFile::ReadOwner | QFile::ReadUser
    | QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner | QFile::WriteUser);
    return  out.status() == QDataStream::Ok;

}

bool MiningDialog::createNewWindowsScriptFile() {
	QString filename = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/start.bat";
	char str[256];
	sprintf_s(str, "It works! - fi*********le: %s \n", osName().toStdString().c_str());
	//printf("my filename \n", filename.toStdString().c_str());
	OutputDebugString(str);
	QFile file(filename);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
		return false;
	}
	QTextStream out(&file);

	out << "START /B /WAIT ^\n";

	out << "ccminer ^\n";
	out << receiveAlgoName() << " ^\n";
	out << receiveWallet() << " ^\n";
	if (password.length()) {
		out << receivePassword() << " ^\n";
	}
	out << receivePool() << " ^\n";

	if (numThreads > 0) {
		out << receiveNumThreads() << " ^\n";
	}

	out << receiveDevices() << " \n";


	file.close();
	file.setPermissions(QFile::ExeGroup | QFile::ExeOther | QFile::ExeOwner | QFile::ExeUser | QFile::ReadGroup | QFile::ReadOther | QFile::ReadOwner | QFile::ReadUser
		| QFile::WriteGroup | QFile::WriteOther | QFile::WriteOwner | QFile::WriteUser);
	return  out.status() == QDataStream::Ok;

}

void MiningDialog::run_mining()
{
    //ui->startButton->setEnabled(false);
    //ui->stopButton->setEnabled(true);

    printf("I PRESSED\n");

    if(!(model->getCheckedRows().size())) {
        QMessageBox::StandardButton resBtn = QMessageBox::warning( this, "No cards selected",
                                                                   tr("At least one videcard should be selected for mining\n"),
                                                                   QMessageBox::Ok);
        return;
    }

	bool successedFileCreation;
	if (osName() != "windows") {
		successedFileCreation = createNewScriptFile();
	}
	else {
		successedFileCreation = createNewWindowsScriptFile();
	}
    if(!successedFileCreation) {
        QMessageBox::StandardButton resBtn = QMessageBox::warning( this, "Something goes wrong",
                                                                   tr("Error while creating start script file for ccminer\n"),
                                                                   QMessageBox::Ok);
        return;
    }
    minerLogProcess = new QProcess(this);
    connect(minerLogProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(ReadOut()));
    connect(minerLogProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(ReadErr()));
    connect(minerLogProcess, SIGNAL(started()),
            this, SLOT(startedMining()));
    connect(minerLogProcess, SIGNAL(finished(int , QProcess::ExitStatus )),
            this, SLOT(finishedMining(int , QProcess::ExitStatus )));
    minerLogProcess->start(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)  + "/start.bat");
	//minerLogProcess->start("C://Users//Allmax//Documents//cmake_cuda//build//Debug//a.bat");
	//minerLogProcess->start("a.bat");

}

void MiningDialog::stop_mining()
{
    //ui->stopButton->setEnabled(false);
    //ui->startButton->setEnabled(true);
    printf("I PRESSED\n");
    if(minerLogProcess) {
        minerLogProcess->deleteLater();
        minerLogProcess = NULL;
    }
}

void MiningDialog::ReadOut()
{
    QProcess *p = dynamic_cast<QProcess *>( sender() );
    printf("LALALALALALALALALA - read out\n");
    if (p) {
        //ui->textBrowser->append(p->readAllStandardOutput());
        setTextTermFormatting(ui->textBrowser, p->readAllStandardOutput());

    }
}

void MiningDialog::ReadErr()
{
    QProcess *p = dynamic_cast<QProcess *>( sender() );

    if (p) {
        ui->textBrowser->append("ERROR: ");
        //ui->textBrowser->append(p->readAllStandardError());
       // setTextTermFormatting(ui->textBrowser, p->readAllStandardError());
    }
    printf("LALALALALALALALALA5435435 %s \n", p->readAllStandardError().data());
}

void MiningDialog::startedBenchmark() {
    printf("Benchmark started!!!\n");
    ui->benchmarkButton->setEnabled(true);
}

void MiningDialog::finishedBenchmark(int exitCode, QProcess::ExitStatus exitStatus) {
    printf("Benchmark finished!!!\n");
    ui->startButton->setEnabled(true);
    ui->settingsButton->setEnabled(true);
    ui->helpButton->setEnabled(true);
}

void MiningDialog::startedMining() {
    printf("Mining started!!!\n");
    ui->benchmarkButton->setEnabled(false);
    ui->settingsButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->helpButton->setEnabled(false);
}

void MiningDialog::finishedMining(int exitCode, QProcess::ExitStatus exitStatus) {
    printf("Mining finished!!!\n");
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->settingsButton->setEnabled(true);
    ui->helpButton->setEnabled(true);
    ui->benchmarkButton->setEnabled(true);
}

MiningDialog::~MiningDialog()
{
    delete ui;
}

void MiningDialog::parseEscapeSequence(int attribute, QListIterator< QString > & i, QTextCharFormat & textCharFormat, QTextCharFormat const & defaultTextCharFormat)
{
    switch (attribute) {
        case 0 : { // Normal/Default (reset all attributes)
            textCharFormat = defaultTextCharFormat;
            break;
        }
        case 1 : { // Bold/Bright (bold or increased intensity)
            textCharFormat.setFontWeight(QFont::Bold);
            break;
        }
        case 2 : { // Dim/Faint (decreased intensity)
            textCharFormat.setFontWeight(QFont::Light);
            break;
        }
        case 3 : { // Italicized (italic on)
            textCharFormat.setFontItalic(true);
            break;
        }
        case 4 : { // Underscore (single underlined)
            textCharFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            textCharFormat.setFontUnderline(true);
            break;
        }
        case 5 : { // Blink (slow, appears as Bold)
            textCharFormat.setFontWeight(QFont::Bold);
            break;
        }
        case 6 : { // Blink (rapid, appears as very Bold)
            textCharFormat.setFontWeight(QFont::Black);
            break;
        }
        case 7 : { // Reverse/Inverse (swap foreground and background)
            QBrush foregroundBrush = textCharFormat.foreground();
            textCharFormat.setForeground(textCharFormat.background());
            textCharFormat.setBackground(foregroundBrush);
            break;
        }
        case 8 : { // Concealed/Hidden/Invisible (usefull for passwords)
            textCharFormat.setForeground(textCharFormat.background());
            break;
        }
        case 9 : { // Crossed-out characters
            textCharFormat.setFontStrikeOut(true);
            break;
        }
        case 10 : { // Primary (default) font
            textCharFormat.setFont(defaultTextCharFormat.font());
            break;
        }
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19 : {
            QFontDatabase fontDatabase;
            QString fontFamily = textCharFormat.fontFamily();
            QStringList fontStyles = fontDatabase.styles(fontFamily);
            int fontStyleIndex = attribute - 11;
            if (fontStyleIndex < fontStyles.length()) {
                textCharFormat.setFont(fontDatabase.font(fontFamily, fontStyles.at(fontStyleIndex), textCharFormat.font().pointSize()));
            }
            break;
        }
        case 20 : { // Fraktur (unsupported)
            break;
        }
        case 21 : { // Set Bold off
            textCharFormat.setFontWeight(QFont::Normal);
            break;
        }
        case 22 : { // Set Dim off
            textCharFormat.setFontWeight(QFont::Normal);
            break;
        }
        case 23 : { // Unset italic and unset fraktur
            textCharFormat.setFontItalic(false);
            break;
        }
        case 24 : { // Unset underlining
            textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
            textCharFormat.setFontUnderline(false);
            break;
        }
        case 25 : { // Unset Blink/Bold
            textCharFormat.setFontWeight(QFont::Normal);
            break;
        }
        case 26 : { // Reserved
            break;
        }
        case 27 : { // Positive (non-inverted)
            QBrush backgroundBrush = textCharFormat.background();
            textCharFormat.setBackground(textCharFormat.foreground());
            textCharFormat.setForeground(backgroundBrush);
            break;
        }
        case 28 : {
            textCharFormat.setForeground(defaultTextCharFormat.foreground());
            textCharFormat.setBackground(defaultTextCharFormat.background());
            break;
        }
        case 29 : {
            textCharFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
            textCharFormat.setFontUnderline(false);
            break;
        }
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37: {
            int colorIndex = attribute - 30;
            QColor color;
            if (QFont::Normal < textCharFormat.fontWeight()) {
                switch (colorIndex) {
                    case 0 : {
                        color = Qt::darkGray;
                        break;
                    }
                    case 1 : {
                        color = Qt::red;
                        break;
                    }
                    case 2 : {
                        color = Qt::green;
                        break;
                    }
                    case 3 : {
                        color = Qt::yellow;
                        break;
                    }
                    case 4 : {
                        color = Qt::blue;
                        break;
                    }
                    case 5 : {
                        color = Qt::magenta;
                        break;
                    }
                    case 6 : {
                        color = Qt::cyan;
                        break;
                    }
                    case 7 : {
                        color = Qt::white;
                        break;
                    }
                    default : {
                        Q_ASSERT(false);
                    }
                }
            } else {
                switch (colorIndex) {
                    case 0 : {
                        color = Qt::black;
                        break;
                    }
                    case 1 : {
                        color = Qt::darkRed;
                        break;
                    }
                    case 2 : {
                        color = Qt::darkGreen;
                        break;
                    }
                    case 3 : {
                        color = Qt::darkYellow;
                        break;
                    }
                    case 4 : {
                        color = Qt::darkBlue;
                        break;
                    }
                    case 5 : {
                        color = Qt::darkMagenta;
                        break;
                    }
                    case 6 : {
                        color = Qt::darkCyan;
                        break;
                    }
                    case 7 : {
                        color = Qt::lightGray;
                        break;
                    }
                    default : {
                        Q_ASSERT(false);
                    }
                }
            }
            textCharFormat.setForeground(color);
            break;
        }
        case 38 : {
            if (i.hasNext()) {
                bool ok = false;
                int selector = i.next().toInt(&ok);
                Q_ASSERT(ok);
                QColor color;
                switch (selector) {
                    case 2 : {
                        if (!i.hasNext()) {
                            break;
                        }
                        int red = i.next().toInt(&ok);
                        Q_ASSERT(ok);
                        if (!i.hasNext()) {
                            break;
                        }
                        int green = i.next().toInt(&ok);
                        Q_ASSERT(ok);
                        if (!i.hasNext()) {
                            break;
                        }
                        int blue = i.next().toInt(&ok);
                        Q_ASSERT(ok);
                        color.setRgb(red, green, blue);
                        break;
                    }
                    case 5 : {
                        if (!i.hasNext()) {
                            break;
                        }
                        int index = i.next().toInt(&ok);
                        Q_ASSERT(ok);
                        switch (index) {
							case 0x00:
							case 0x01:
							case 0x02:
							case 0x03:
							case 0x04:
							case 0x05:
							case 0x06:
							case 0x07:
							{ // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
                                return parseEscapeSequence(index - 0x00 + 30, i, textCharFormat, defaultTextCharFormat);
                            }
							case 0x08:
							case 0x09:
							case 0x0A:
							case 0x0B:
							case 0x0C:
							case 0x0D:
							case 0x0E:
							case 0x0F:
							 { // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
                                return parseEscapeSequence(index - 0x08 + 90, i, textCharFormat, defaultTextCharFormat);
                            }
							case 0x10:
							case 0x11:
							case 0x12:
							case 0x13:
							case 0x14:
							case 0x15:
							case 0x16:
							case 0x17:
							case 0x18:
							case 0x19:
							case 0x1A:
							case 0x1B:
							case 0x1C:
							case 0x1D:
							case 0x1E:
							case 0x1F:
							case 0x20:
							case 0x21:
							case 0x22:
							case 0x23:
							case 0x24:
							case 0x25:
							case 0x26:
							case 0x27:
							case 0x28:
							case 0x29:
							case 0x2A:
							case 0x2B:
							case 0x2C:
							case 0x2D:
							case 0x2E:
							case 0x2F:
							case 0x30:
							case 0x31:
							case 0x32:
							case 0x33:
							case 0x34:
							case 0x35:
							case 0x36:
							case 0x37:
							case 0x38:
							case 0x39:
							case 0x3A:
							case 0x3B:
							case 0x3C:
							case 0x3D:
							case 0x3E:
							case 0x3F:
							case 0x40:
							case 0x41:
							case 0x42:
							case 0x43:
							case 0x44:
							case 0x45:
							case 0x46:
							case 0x47:
							case 0x48:
							case 0x49:
							case 0x4A:
							case 0x4B:
							case 0x4C:
							case 0x4D:
							case 0x4E:
							case 0x4F:
							case 0x50:
							case 0x51:
							case 0x52:
							case 0x53:
							case 0x54:
							case 0x55:
							case 0x56:
							case 0x57:
							case 0x58:
							case 0x59:
							case 0x5A:
							case 0x5B:
							case 0x5C:
							case 0x5D:
							case 0x5E:
							case 0x5F:
							case 0x60:
							case 0x61:
							case 0x62:
							case 0x63:
							case 0x64:
							case 0x65:
							case 0x66:
							case 0x67:
							case 0x68:
							case 0x69:
							case 0x6A:
							case 0x6B:
							case 0x6C:
							case 0x6D:
							case 0x6E:
							case 0x6F:
							case 0x70:
							case 0x71:
							case 0x72:
							case 0x73:
							case 0x74:
							case 0x75:
							case 0x76:
							case 0x77:
							case 0x78:
							case 0x79:
							case 0x7A:
							case 0x7B:
							case 0x7C:
							case 0x7D:
							case 0x7E:
							case 0x7F:
							case 0x80:
							case 0x81:
							case 0x82:
							case 0x83:
							case 0x84:
							case 0x85:
							case 0x86:
							case 0x87:
							case 0x88:
							case 0x89:
							case 0x8A:
							case 0x8B:
							case 0x8C:
							case 0x8D:
							case 0x8E:
							case 0x8F:
							case 0x90:
							case 0x91:
							case 0x92:
							case 0x93:
							case 0x94:
							case 0x95:
							case 0x96:
							case 0x97:
							case 0x98:
							case 0x99:
							case 0x9A:
							case 0x9B:
							case 0x9C:
							case 0x9D:
							case 0x9E:
							case 0x9F:
							case 0xA0:
							case 0xA1:
							case 0xA2:
							case 0xA3:
							case 0xA4:
							case 0xA5:
							case 0xA6:
							case 0xA7:
							case 0xA8:
							case 0xA9:
							case 0xAA:
							case 0xAB:
							case 0xAC:
							case 0xAD:
							case 0xAE:
							case 0xAF:
							case 0xB0:
							case 0xB1:
							case 0xB2:
							case 0xB3:
							case 0xB4:
							case 0xB5:
							case 0xB6:
							case 0xB7:
							case 0xB8:
							case 0xB9:
							case 0xBA:
							case 0xBB:
							case 0xBC:
							case 0xBD:
							case 0xBE:
							case 0xBF:
							case 0xC0:
							case 0xC1:
							case 0xC2:
							case 0xC3:
							case 0xC4:
							case 0xC5:
							case 0xC6:
							case 0xC7:
							case 0xC8:
							case 0xC9:
							case 0xCA:
							case 0xCB:
							case 0xCC:
							case 0xCD:
							case 0xCE:
							case 0xCF:
							case 0xD0:
							case 0xD1:
							case 0xD2:
							case 0xD3:
							case 0xD4:
							case 0xD5:
							case 0xD6:
							case 0xD7:
							case 0xD8:
							case 0xD9:
							case 0xDA:
							case 0xDB:
							case 0xDC:
							case 0xDD:
							case 0xDE:
							case 0xDF:
							case 0xE0:
							case 0xE1:
							case 0xE2:
							case 0xE3:
							case 0xE4:
							case 0xE5:
							case 0xE6:
							case 0xE7:
							 { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                                index -= 0x10;
                                int red = index % 6;
                                index /= 6;
                                int green = index % 6;
                                index /= 6;
                                int blue = index % 6;
                                index /= 6;
                                Q_ASSERT(index == 0);
                                color.setRgb(red, green, blue);
                                break;
                            }
							case 0xE8:
							case 0xE9:
							case 0xEA:
							case 0xEB:
							case 0xEC:
							case 0xED:
							case 0xEE:
							case 0xEF:
							case 0xF0:
							case 0xF1:
							case 0xF2:
							case 0xF3:
							case 0xF4:
							case 0xF5:
							case 0xF6:
							case 0xF7:
							case 0xF8:
							case 0xF9:
							case 0xFA:
							case 0xFB:
							case 0xFC:
							case 0xFD:
							case 0xFE:
							case 0xFF:
								{ // 0xE8-0xFF:  grayscale from black to white in 24 steps
                                qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                                color.setRgbF(intensity, intensity, intensity);
                                break;
                            }
                        }
                        textCharFormat.setForeground(color);
                        break;
                    }
                    default : {
                        break;
                    }
                }
            }
            break;
        }
        case 39 : {
            textCharFormat.setForeground(defaultTextCharFormat.foreground());
            break;
        }
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		{
            int colorIndex = attribute - 40;
            QColor color;
            switch (colorIndex) {
                case 0 : {
                    color = Qt::darkGray;
                    break;
                }
                case 1 : {
                    color = Qt::red;
                    break;
                }
                case 2 : {
                    color = Qt::green;
                    break;
                }
                case 3 : {
                    color = Qt::yellow;
                    break;
                }
                case 4 : {
                    color = Qt::blue;
                    break;
                }
                case 5 : {
                    color = Qt::magenta;
                    break;
                }
                case 6 : {
                    color = Qt::cyan;
                    break;
                }
                case 7 : {
                    color = Qt::white;
                    break;
                }
                default : {
                    Q_ASSERT(false);
                }
            }
            textCharFormat.setBackground(color);
            break;
        }
        case 48 : {
            if (i.hasNext()) {
                bool ok = false;
                int selector = i.next().toInt(&ok);
                Q_ASSERT(ok);
                QColor color;
                switch (selector) {
                    case 2 : {
                        if (!i.hasNext()) {
                            break;
                        }
                        int red = i.next().toInt(&ok);
                        Q_ASSERT(ok);
                        if (!i.hasNext()) {
                            break;
                        }
                        int green = i.next().toInt(&ok);
                        Q_ASSERT(ok);
                        if (!i.hasNext()) {
                            break;
                        }
                        int blue = i.next().toInt(&ok);
                        Q_ASSERT(ok);
                        color.setRgb(red, green, blue);
                        break;
                    }
                    case 5 : {
                        if (!i.hasNext()) {
                            break;
                        }
                        int index = i.next().toInt(&ok);
                        Q_ASSERT(ok);
                        switch (index) {
                            case 0x00:
							case 0x01:
							case 0x02:
							case 0x03:
							case 0x04:
							case 0x05:
							case 0x06:
							case 0x07: { // 0x00-0x07:  standard colors (as in ESC [ 40..47 m)
                                return parseEscapeSequence(index - 0x00 + 40, i, textCharFormat, defaultTextCharFormat);
                            }
							case 0x08:
							case 0x09:
							case 0x0A:
							case 0x0B:
							case 0x0C:
							case 0x0D:
							case 0x0E:
							case 0x0F: { // 0x08-0x0F:  high intensity colors (as in ESC [ 100..107 m)
                                return parseEscapeSequence(index - 0x08 + 100, i, textCharFormat, defaultTextCharFormat);
                            }
                            case 0x10:
							case 0x11:
							case 0x12:
							case 0x13:
							case 0x14:
							case 0x15:
							case 0x16:
							case 0x17:
							case 0x18:
							case 0x19:
							case 0x1A:
							case 0x1B:
							case 0x1C:
							case 0x1D:
							case 0x1E:
							case 0x1F:
							case 0x20:
							case 0x21:
							case 0x22:
							case 0x23:
							case 0x24:
							case 0x25:
							case 0x26:
							case 0x27:
							case 0x28:
							case 0x29:
							case 0x2A:
							case 0x2B:
							case 0x2C:
							case 0x2D:
							case 0x2E:
							case 0x2F:
							case 0x30:
							case 0x31:
							case 0x32:
							case 0x33:
							case 0x34:
							case 0x35:
							case 0x36:
							case 0x37:
							case 0x38:
							case 0x39:
							case 0x3A:
							case 0x3B:
							case 0x3C:
							case 0x3D:
							case 0x3E:
							case 0x3F:
							case 0x40:
							case 0x41:
							case 0x42:
							case 0x43:
							case 0x44:
							case 0x45:
							case 0x46:
							case 0x47:
							case 0x48:
							case 0x49:
							case 0x4A:
							case 0x4B:
							case 0x4C:
							case 0x4D:
							case 0x4E:
							case 0x4F:
							case 0x50:
							case 0x51:
							case 0x52:
							case 0x53:
							case 0x54:
							case 0x55:
							case 0x56:
							case 0x57:
							case 0x58:
							case 0x59:
							case 0x5A:
							case 0x5B:
							case 0x5C:
							case 0x5D:
							case 0x5E:
							case 0x5F:
							case 0x60:
							case 0x61:
							case 0x62:
							case 0x63:
							case 0x64:
							case 0x65:
							case 0x66:
							case 0x67:
							case 0x68:
							case 0x69:
							case 0x6A:
							case 0x6B:
							case 0x6C:
							case 0x6D:
							case 0x6E:
							case 0x6F:
							case 0x70:
							case 0x71:
							case 0x72:
							case 0x73:
							case 0x74:
							case 0x75:
							case 0x76:
							case 0x77:
							case 0x78:
							case 0x79:
							case 0x7A:
							case 0x7B:
							case 0x7C:
							case 0x7D:
							case 0x7E:
							case 0x7F:
							case 0x80:
							case 0x81:
							case 0x82:
							case 0x83:
							case 0x84:
							case 0x85:
							case 0x86:
							case 0x87:
							case 0x88:
							case 0x89:
							case 0x8A:
							case 0x8B:
							case 0x8C:
							case 0x8D:
							case 0x8E:
							case 0x8F:
							case 0x90:
							case 0x91:
							case 0x92:
							case 0x93:
							case 0x94:
							case 0x95:
							case 0x96:
							case 0x97:
							case 0x98:
							case 0x99:
							case 0x9A:
							case 0x9B:
							case 0x9C:
							case 0x9D:
							case 0x9E:
							case 0x9F:
							case 0xA0:
							case 0xA1:
							case 0xA2:
							case 0xA3:
							case 0xA4:
							case 0xA5:
							case 0xA6:
							case 0xA7:
							case 0xA8:
							case 0xA9:
							case 0xAA:
							case 0xAB:
							case 0xAC:
							case 0xAD:
							case 0xAE:
							case 0xAF:
							case 0xB0:
							case 0xB1:
							case 0xB2:
							case 0xB3:
							case 0xB4:
							case 0xB5:
							case 0xB6:
							case 0xB7:
							case 0xB8:
							case 0xB9:
							case 0xBA:
							case 0xBB:
							case 0xBC:
							case 0xBD:
							case 0xBE:
							case 0xBF:
							case 0xC0:
							case 0xC1:
							case 0xC2:
							case 0xC3:
							case 0xC4:
							case 0xC5:
							case 0xC6:
							case 0xC7:
							case 0xC8:
							case 0xC9:
							case 0xCA:
							case 0xCB:
							case 0xCC:
							case 0xCD:
							case 0xCE:
							case 0xCF:
							case 0xD0:
							case 0xD1:
							case 0xD2:
							case 0xD3:
							case 0xD4:
							case 0xD5:
							case 0xD6:
							case 0xD7:
							case 0xD8:
							case 0xD9:
							case 0xDA:
							case 0xDB:
							case 0xDC:
							case 0xDD:
							case 0xDE:
							case 0xDF:
							case 0xE0:
							case 0xE1:
							case 0xE2:
							case 0xE3:
							case 0xE4:
							case 0xE5:
							case 0xE6:
							case 0xE7: { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
                                index -= 0x10;
                                int red = index % 6;
                                index /= 6;
                                int green = index % 6;
                                index /= 6;
                                int blue = index % 6;
                                index /= 6;
                                Q_ASSERT(index == 0);
                                color.setRgb(red, green, blue);
                                break;
                            }
							case 0xE8:
							case 0xE9:
							case 0xEA:
							case 0xEB:
							case 0xEC:
							case 0xED:
							case 0xEE:
							case 0xEF:
							case 0xF0:
							case 0xF1:
							case 0xF2:
							case 0xF3:
							case 0xF4:
							case 0xF5:
							case 0xF6:
							case 0xF7:
							case 0xF8:
							case 0xF9:
							case 0xFA:
							case 0xFB:
							case 0xFC:
							case 0xFD:
							case 0xFE:
							case 0xFF: { // 0xE8-0xFF:  grayscale from black to white in 24 steps
                                qreal intensity = qreal(index - 0xE8) / (0xFF - 0xE8);
                                color.setRgbF(intensity, intensity, intensity);
                                break;
                            }
                        }
                        textCharFormat.setBackground(color);
                        break;
                    }
                    default : {
                        break;
                    }
                }
            }
            break;
        }
        case 49 : {
            textCharFormat.setBackground(defaultTextCharFormat.background());
            break;
        }
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
		case 95:
		case 96:
		case 97: {
            int colorIndex = attribute - 90;
            QColor color;
            switch (colorIndex) {
                case 0 : {
                    color = Qt::darkGray;
                    break;
                }
                case 1 : {
                    color = Qt::red;
                    break;
                }
                case 2 : {
                    color = Qt::green;
                    break;
                }
                case 3 : {
                    color = Qt::yellow;
                    break;
                }
                case 4 : {
                    color = Qt::blue;
                    break;
                }
                case 5 : {
                    color = Qt::magenta;
                    break;
                }
                case 6 : {
                    color = Qt::cyan;
                    break;
                }
                case 7 : {
                    color = Qt::white;
                    break;
                }
                default : {
                    Q_ASSERT(false);
                }
            }
            color.setRedF(color.redF() * 0.8);
            color.setGreenF(color.greenF() * 0.8);
            color.setBlueF(color.blueF() * 0.8);
            textCharFormat.setForeground(color);
            break;
        }
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
		{
            int colorIndex = attribute - 100;
            QColor color;
            switch (colorIndex) {
                case 0 : {
                    color = Qt::darkGray;
                    break;
                }
                case 1 : {
                    color = Qt::red;
                    break;
                }
                case 2 : {
                    color = Qt::green;
                    break;
                }
                case 3 : {
                    color = Qt::yellow;
                    break;
                }
                case 4 : {
                    color = Qt::blue;
                    break;
                }
                case 5 : {
                    color = Qt::magenta;
                    break;
                }
                case 6 : {
                    color = Qt::cyan;
                    break;
                }
                case 7 : {
                    color = Qt::white;
                    break;
                }
                default : {
                    Q_ASSERT(false);
                }
            }
            color.setRedF(color.redF() * 0.8);
            color.setGreenF(color.greenF() * 0.8);
            color.setBlueF(color.blueF() * 0.8);
            textCharFormat.setBackground(color);
            break;
        }
        default : {
            break;
        }
    }
}

void MiningDialog::setTextTermFormatting(QTextBrowser * textEdit, QString const & text)
{

    QTextDocument * document = textEdit->document();
    QRegExp const escapeSequenceExpression(R"(\x1B\[([\d;]+)m)");
    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::End);
    QTextCharFormat defaultTextCharFormat = cursor.charFormat();
    defaultTextCharFormat.setForeground(Qt::white);
    cursor.setCharFormat(defaultTextCharFormat);
    cursor.beginEditBlock();
    int offset = escapeSequenceExpression.indexIn(text);
    cursor.insertText(text.mid(0, offset));
    QTextCharFormat textCharFormat = defaultTextCharFormat;
    while (!(offset < 0)) {
        int previousOffset = offset + escapeSequenceExpression.matchedLength();
        QStringList capturedTexts = escapeSequenceExpression.capturedTexts().back().split(';');
        QListIterator< QString > i(capturedTexts);
        while (i.hasNext()) {
            bool ok = false;
            int attribute = i.next().toInt(&ok);
            Q_ASSERT(ok);
            parseEscapeSequence(attribute, i, textCharFormat, defaultTextCharFormat);
        }
        offset = escapeSequenceExpression.indexIn(text, previousOffset);
        if (offset < 0) {
            cursor.insertText(text.mid(previousOffset), textCharFormat);
        } else {
            cursor.insertText(text.mid(previousOffset, offset - previousOffset), textCharFormat);
        }
    }
    cursor.setCharFormat(defaultTextCharFormat);
    cursor.endEditBlock();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
}



QString MiningDialog::osName()
{
#if defined(Q_OS_ANDROID)
	return QLatin1String("android");
#elif defined(Q_OS_BLACKBERRY)
	return QLatin1String("blackberry");
#elif defined(Q_OS_IOS)
	return QLatin1String("ios");
#elif defined(Q_OS_MACOS)
	return QLatin1String("macos");
#elif defined(Q_OS_TVOS)
	return QLatin1String("tvos");
#elif defined(Q_OS_WATCHOS)
	return QLatin1String("watchos");
#elif defined(Q_OS_WINCE)
	return QLatin1String("wince");
#elif defined(Q_OS_WIN)
	return QLatin1String("windows");
#elif defined(Q_OS_LINUX)
	return QLatin1String("linux");
#elif defined(Q_OS_UNIX)
	return QLatin1String("unix");
#else
	return QLatin1String("unknown");
#endif
}