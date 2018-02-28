//
// Created by 216k155
//
#include "device_info.h"
#include <qapplication.h>
#include <qpushbutton.h>


int main( int argc, char **argv )
{
    printDeviceProps();
    QApplication a( argc, argv );

    QPushButton hello( "Hello world!", 0 );
    hello.resize( 100, 30 );

   // a.setMainWidget( &hello );
    hello.show();
    return a.exec();
}
