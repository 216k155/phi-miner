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

    bool successedFileCreation = createNewScriptFile();
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
    minerLogProcess->start(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)  + "/start.sh");

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
        case 11 ... 19 : {
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
        case 30 ... 37 : {
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
                            case 0x00 ... 0x07 : { // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
                                return parseEscapeSequence(index - 0x00 + 30, i, textCharFormat, defaultTextCharFormat);
                            }
                            case 0x08 ... 0x0F : { // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
                                return parseEscapeSequence(index - 0x08 + 90, i, textCharFormat, defaultTextCharFormat);
                            }
                            case 0x10 ... 0xE7 : { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
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
                            case 0xE8 ... 0xFF : { // 0xE8-0xFF:  grayscale from black to white in 24 steps
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
        case 40 ... 47 : {
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
                            case 0x00 ... 0x07 : { // 0x00-0x07:  standard colors (as in ESC [ 40..47 m)
                                return parseEscapeSequence(index - 0x00 + 40, i, textCharFormat, defaultTextCharFormat);
                            }
                            case 0x08 ... 0x0F : { // 0x08-0x0F:  high intensity colors (as in ESC [ 100..107 m)
                                return parseEscapeSequence(index - 0x08 + 100, i, textCharFormat, defaultTextCharFormat);
                            }
                            case 0x10 ... 0xE7 : { // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
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
                            case 0xE8 ... 0xFF : { // 0xE8-0xFF:  grayscale from black to white in 24 steps
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
        case 90 ... 97 : {
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
        case 100 ... 107 : {
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

