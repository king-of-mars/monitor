#include "MainWindow.h"

MainWindow::MainWindow()
{
    //GUI defaults, have to be loaded from a file
    Default_DeviceNo = 2;

    //-------------------------------------------

    //Lookup the IP adress of the client:
    //MISSING

    //Misc inits:
    LastAmountData = 0;

    setWindowTitle(tr("Qt Network Monitor"));
    resize(520, 340);

    //-----------------Sets up the widgets---------------------------
    {//Up/Down Stats
        LCDDownloadKB = new QLCDNumber(this);
        LCDDownloadKB->setSegmentStyle(QLCDNumber::Filled);

        LCDUploadKB = new QLCDNumber(this);
        LCDUploadKB->setSegmentStyle(QLCDNumber::Filled);

        DownloadUploadGB = new QGroupBox(tr("Download/Upload Stats"));
        QHBoxLayout *layoutH1 = new QHBoxLayout;

        layoutH1->addWidget(LCDDownloadKB);
        layoutH1->addWidget(LCDUploadKB);

        DownloadUploadGB->setLayout(layoutH1);
    }

    {//Device choice menu
        DropListDeviceChoice = new QComboBox(this);
        PushBDropListSetToCurrent = new QPushButton("Set",this);

        DropListDeviceGB = new QGroupBox(tr("Device to monitor:"));
        QHBoxLayout *layoutHDropListDevice = new QHBoxLayout;
        layoutHDropListDevice->addWidget(DropListDeviceChoice);
        layoutHDropListDevice->addWidget(PushBDropListSetToCurrent);
        DropListDeviceGB->setLayout(layoutHDropListDevice);
    }

    //Console
    Console = new QConsole(this);

    //Speed Scope
    DownloadScope = new Scope(this);

    {//Sets-up the layout
        QVBoxLayout *mainLayout = new QVBoxLayout;

        mainLayout->addWidget(DropListDeviceGB);
        mainLayout->addWidget(DownloadUploadGB);
        mainLayout->addWidget(DownloadScope, Qt::AlignCenter);
        mainLayout->addWidget(Console, Qt::AlignBottom);

        setLayout(mainLayout);
    }
    //---------------------------------------------------------------

    //Set a clock to update the display
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(250);

    //Keep track on the speed of the data received
    QTimer *timer_data = new QTimer(this);
    connect( timer_data, SIGNAL(timeout()), this, SLOT(updateKBPS()) );
    timer_data->start(1000);

    ThreadL = new ThreadListener(&PCHandler);

    OpenDevice();
    StartCapture();

    DownloadSpeedHist = vector<float>(256,0);

    //Signals/Slots
    connect( PushBDropListSetToCurrent, SIGNAL( clicked() ), this, SLOT(ChangeDevice()));
}

void MainWindow::OpenDevice(int DeviceNO)
{
    vector<string> * Devices = new vector<string>;

    PCHandler.FindAvailDevices(Devices);

    DropListDeviceChoice->clear();
    for(int x=0; x< Devices->size();x++)
    {
        QString temp( (*Devices)[x].c_str() );
        DropListDeviceChoice->addItem(temp);
    }

    int Device_to_Open;

    if (DeviceNO != -1)
        Device_to_Open = DeviceNO;
    else
        Device_to_Open = Default_DeviceNo;

    PCHandler.openDevice(Device_to_Open);
    DropListDeviceChoice->setCurrentIndex(Device_to_Open-1);
}

void MainWindow::StartCapture()
{
    ThreadL->start();
}

void MainWindow::ChangeDevice()
{
    int device_No = DropListDeviceChoice->currentIndex();

    ThreadL->terminate();
    ThreadL->wait();

    OpenDevice(device_No+1);
    StartCapture();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    LCDDownloadKB->display( (int) (PCHandler.get_TotalDataTransferred_bytes() / 1024.0) );
    LCDUploadKB->display( (int) (PCHandler.get_TotalDataTransferred_bytes() / (1024.0*1024.0) ) );

    //Update the messages
    Console->Display_Messages( PCHandler.get_messages() );
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

