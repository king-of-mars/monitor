#include "MainWindow.h"

MainWindow::MainWindow()
{
    //Lookup the IP adress of the client:
    LastAmountData = 0;

    setWindowTitle(tr("Qt Network Monitor"));
    resize(420, 240);

    //Sets up the widgets
    LCDDownloadKB = new QLCDNumber(this);
    LCDDownloadKB->setSegmentStyle(QLCDNumber::Filled);

    LCDUploadKB = new QLCDNumber(this);
    LCDUploadKB->setSegmentStyle(QLCDNumber::Filled);

    DownloadUploadGB = new QGroupBox(tr("Download/Upload Stats"));
    QHBoxLayout *layoutH1 = new QHBoxLayout;

    layoutH1->addWidget(LCDDownloadKB);
    layoutH1->addWidget(LCDUploadKB);

    DownloadUploadGB->setLayout(layoutH1);

    Console = new QConsole(this);

    DownloadScope = new Scope(this);

    //QDockWidget * dw = new QDockWidget;
    //dw->setWidget(LCDDownloadKB);
    //addDockWidget(Qt::LeftDockWidgetArea, dw);

    //Sets-up the layout
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(DownloadUploadGB);

    mainLayout->addWidget(DownloadScope, Qt::AlignCenter);
    mainLayout->addWidget(Console, Qt::AlignBottom);

    setLayout(mainLayout);

    //Set a clock to update the display
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(250);

    //Keep track on the speed of the data received
    QTimer *timer_data = new QTimer(this);
    connect(timer_data, SIGNAL(timeout()), this, SLOT(updateKBPS()));
    timer_data->start(1000);

    ThreadL = new ThreadListener(&PCHandler);

    OpenDevice();
    StartCapture();

    DownloadSpeedHist = vector<float>(256,0);
}

void MainWindow::OpenDevice()
{
    PCHandler.FindAvailDevices();
    PCHandler.openDevice(2);
    PCHandler.Print_messages();

    Console->Display_Messages( PCHandler.get_messages() );

}

void MainWindow::StartCapture()
{
    ThreadL->start();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    LCDDownloadKB->display( (int) (PCHandler.get_TotalDataTransferred_bytes()/ 1024.0) );
    LCDUploadKB->display( (int) (PCHandler.get_TotalDataTransferred_bytes()/ (1024.0*1024.0) ) );
}

void MainWindow::updateKBPS()
{
    float DataTransferedSinceLastCall =
    (PCHandler.get_TotalDataTransferred_bytes()/ 1024.0)-LastAmountData;

    LastAmountData = (PCHandler.get_TotalDataTransferred_bytes()/ 1024.0);

    //Push one in, remove one
    DownloadSpeedHist.push_back( DataTransferedSinceLastCall );
    DownloadSpeedHist.erase( DownloadSpeedHist.begin(), DownloadSpeedHist.begin()+1);

    DownloadScope->Set_Data(DownloadSpeedHist);
}

