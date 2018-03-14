//
// Created by 216k155 on 2/26/18.
//

#ifndef CMAKE_AND_CUDA_MININGDIALOG_H
#define CMAKE_AND_CUDA_MININGDIALOG_H

#include <QMainWindow>
#include <QProcess>
#include <QTextBrowser>
#include <QString>
#include <QListIterator>
#include <QTextCharFormat>
#include "customlistmodel.h"
#include "settingsdialog.h"

namespace Ui
{
    class MiningDialog;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Dialog showing transaction details. */
class MiningDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit MiningDialog(QWidget* parent = 0);
    int abc=11;
    void closeEvent (QCloseEvent *event);
    void setTextTermFormatting(QTextBrowser * textEdit, QString const & text);
    void parseEscapeSequence(int attribute, QListIterator< QString > & i, QTextCharFormat & textCharFormat, QTextCharFormat const & defaultTextCharFormat);
    ~MiningDialog();
public slots:
    void openSettingsDialog();
private:
    Ui::MiningDialog* ui;
    bool m_NeverShown;
    int m_HistoryIndex;
    QStringList m_History;
    QProcess *minerLogProcess = NULL;
    QProcess *benchmarkLogProcess = NULL;
    CustomListModel* model;
    void createListModelView();
    SettingsDialog *settingsDialog;
    QString algo = "phi";
    int numThreads = 0;
    QString password = "";
private Q_SLOTS:
    void run_benchmark();
    void run_mining();
    void stop_mining();
    void ReadOut();
    void ReadErr();
    void startedBenchmark();
    void finishedBenchmark(int exitCode, QProcess::ExitStatus exitStatus);
    void startedMining();
    bool createNewScriptFile();
    void exitButtonClicked();
    QString receiveWallet();
    QString receivePool();
    QString receiveDevices();
    QString receiveAlgoName();
    QString receivePassword();
    QString receiveNumThreads();
    void finishedMining(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif //CMAKE_AND_CUDA_MININGDIALOG_H
