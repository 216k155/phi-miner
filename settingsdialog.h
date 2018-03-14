//
// Created by 216k155 on 3/6/18.
//

#ifndef CMAKE_AND_CUDA_SETTINGSDIALOG_H
#define CMAKE_AND_CUDA_SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>


namespace Ui
{
    class SettingsDialog;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Dialog showing transaction details. */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent, QString* algo, int* numThreads, QString* pass);

    ~SettingsDialog();
private:
    Ui::SettingsDialog* ui;
    QString* algoValue;
    int* numThr;
    QString* password;

private Q_SLOTS:
    void on_buttonBox_clicked(QAbstractButton *button);

};


#endif //CMAKE_AND_CUDA_SETTINGSDIALOG_H
