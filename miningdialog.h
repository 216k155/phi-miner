//
// Created by 216k155
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
    void closeEvent (QCloseEvent *event);
    void setTextTermFormatting(QTextBrowser * textEdit, QString const & text);
    void parseEscapeSequence(int attribute, QListIterator< QString > & i, QTextCharFormat & textCharFormat, QTextCharFormat const & defaultTextCharFormat);
    ~MiningDialog();

private:
    Ui::MiningDialog* ui;
    bool m_NeverShown;
    int m_HistoryIndex;
    QStringList m_History;
    QProcess *minerLogProcess = NULL;
    CustomListModel* model;
    void createListModelView();
private Q_SLOTS:
    void run_benchmark();
    void run_mining();
    void stop_mining();
    void ReadOut();
    void ReadErr();
};

#endif //CMAKE_AND_CUDA_MININGDIALOG_H
