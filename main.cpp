#include <iostream>

#include <QApplication>
#include <QPushButton>

#include "WinPcapFns.h"
#include "GUI/MainWindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    cout<<"App. Begin"<<endl;

    QApplication app(argc, argv);
    MainWindow MainWin;
    MainWin.show();

    int return_v = app.exec();

    cout<<"App. End"<<endl;

    return return_v;
}
