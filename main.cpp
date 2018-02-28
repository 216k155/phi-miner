//
// Created by 216k155
//
#include "device_info.h"
#include <QApplication>
#include <QPushButton>
#include "miningdialog.h"


int main( int argc, char **argv )
{
    printDeviceProps();
    QApplication a( argc, argv );

    MiningDialog miningDialog;


    miningDialog.show();


    return a.exec();
}
