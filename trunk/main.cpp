#include <iostream>

#include <QApplication>
#include <QPushButton>

#include "WinPcapFns.h"
#include "GUI/MainWindow.h"
#include "GUI/MasterWindow.h"

using namespace std;

int main(int argc, char *argv[])
{

    cout<<"App. Begin"<<endl;

    QApplication app(argc, argv);

    MasterWindow MasterWin;
    MasterWin.show();

    //MainWindow MainWin;
    //MainWin.show();

    int return_v = app.exec();

    cout<<"App. End"<<endl;

    return return_v;

}
