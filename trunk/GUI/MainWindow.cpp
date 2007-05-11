#include "MainWindow.h"

MainWindow::MainWindow()
{
    //GUI defaults, have to be loaded from a file
    Default_DeviceNo = 2;

    //-------------------------------------------

    //Lookup the IP adress of the client:
    //MISSING

    //Misc inits:
    LastAmountData_download = 0;
    LastAmountData_upload = 0;
    DataDownloadedSinceLastCall = 0;
    DataUploadedSinceLastCall = 0;

    setWindowTitle(tr("Qt Network Monitor"));
    resize(520, 340);

    //-----------------Sets up the widgets---------------------------
    {//Up/Down Stats
        DownloadKBpS = new QLabel("0",this);
        UploadKBpS = new QLabel("0",this);
        DownloadKBTotal = new QLabel("0",this);
        UploadKBTotal = new QLabel("0",this);

        DownloadKBpS_label = new QLabel("Down.:",this);
        UploadKBpS_label = new QLabel("Up.:",this);
        DownloadKBTotal_label = new QLabel("Down. total:",this);
        UploadKBTotal_label = new QLabel("Up. total:",this);

        DownloadKBpS_units = new QLabel("B/s",this);
        UploadKBpS_units = new QLabel("B/s",this);
        DownloadKBTotal_units = new QLabel("B/s",this);
        UploadKBTotal_units = new QLabel("B/s",this);

        DownloadUploadGB = new QGroupBox(tr("Download/Upload Stats"));
        QHBoxLayout *layoutH1 = new QHBoxLayout;

        layoutH1->addWidget(DownloadKBpS_label);
        layoutH1->addWidget(DownloadKBpS);
        layoutH1->addWidget(DownloadKBpS_units,Qt::AlignLeft);
        layoutH1->addWidget(UploadKBpS_label);
        layoutH1->addWidget(UploadKBpS);
        layoutH1->addWidget(UploadKBpS_units,Qt::AlignLeft);
        layoutH1->addWidget(DownloadKBTotal_label);
        layoutH1->addWidget(DownloadKBTotal);
        layoutH1->addWidget(DownloadKBTotal_units,Qt::AlignLeft);
        layoutH1->addWidget(UploadKBTotal_label);
        layoutH1->addWidget(UploadKBTotal);
        layoutH1->addWidget(UploadKBTotal_units,Qt::AlignLeft);

        DownloadUploadGB->setLayout(layoutH1);
    }

    {//Device choice menu
        DropListDeviceChoice = new QComboBox(this);
        DropListDeviceChoice->setMinimumSize(60,10);
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
    dataScope = new Scope(this);

    {//Sets-up the layout
        QVBoxLayout *mainLayout = new QVBoxLayout;

        mainLayout->addWidget(DropListDeviceGB);
        mainLayout->addWidget(DownloadUploadGB);

        mainLayout->addWidget(dataScope, Qt::AlignCenter);

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

    cout<<"Attempting to open device"<<endl;

    OpenDevice();

    cout<<"Attempting to start capture device"<<endl;

    StartCapture();

    SpeedHist_Download = vector<float>(256,0);
    SpeedHist_Upload = vector<float>(256,0);

    //Tray
    setWindowIcon(QIcon(":/GFX/systray/trash.svg"));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon((QIcon(":/GFX/systray/TrayIconNormal.png")));

    trayIconMenu = new QMenu(this);
    trayIconMenu->setIcon((QIcon(":/GFX/systray/TrayIconNormal.png")));
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    //trayIcon->setVisible(true);

    //Signals/Slots
    connect( PushBDropListSetToCurrent, SIGNAL( clicked() ), this, SLOT(ChangeDevice()) );
    connect( trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason) ),
             this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)) );

    //Set the data offset
    InfoReadWrite Reader;
    Reader.Read("Stats.dat");

    if( Reader.getData().size() >= 2 )
    {
        Download_offset = Reader.getData()[0];
        Upload_offset   = Reader.getData()[1];
    }
    else
    {
        Download_offset=0.0f;
        Upload_offset=0.0f;
        cout<<"There was a problem reading the stats file."<<endl;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    trayIcon->setVisible(true);

    if (trayIcon->isVisible())
    {
    hide();
    event->ignore();
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    //trayIcon->setVisible(false);
    show();

    default:;
    }
}

int MainWindow::OpenDevice(int DeviceNO)
{
    vector<string> * Devices = new vector<string>;
    PCHandler.FindAvailDevices(Devices);

    cout<<"Availables devices"<<endl;

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

    if ( Devices->size() < 1 )
    {
        cout<<"No device available. (Linux-> Sudo?)"<<endl;
        return -1;
    }

    if ( (Device_to_Open < 1) || (Device_to_Open > Devices->size()) )
    {
        cout<<"Invalid device number or default device, trying device #1."<<endl;
        Device_to_Open = 1;
    }

    PCHandler.getDeviceIP(Device_to_Open);
    PCHandler.openDevice(Device_to_Open);

    DropListDeviceChoice->setCurrentIndex(Device_to_Open-1);

    return 0;
}

int MainWindow::StartCapture()
{
    ThreadL->start();
    return 0;
}

void MainWindow::ChangeDevice()
{
    int device_No = DropListDeviceChoice->currentIndex();

    ThreadL->instructStop();
    if(!ThreadL->wait(2500))
    {
        //Console.Display_Messages("ERROR: Thread didn't stop properly, process had to be killed.");
        //messages.push_back
        //messages.push_back("Please restart the application");
        cout<<"Error stopping thread"<<endl;

        ThreadL->terminate();//Very dangerous
        ThreadL->wait(500);

        //Memory leak?
        PcapHandler freshPCH;
        PCHandler = freshPCH;
        delete ThreadL;
        ThreadL = new ThreadListener(&PCHandler);
    }

    clearMemory();
    OpenDevice(device_No+1);
    StartCapture();
}

string MainWindow::getUnits(float nBits)
{
    if ( nBits < 1024.0 )
        return "B";
    if ( nBits < 1024.0 * 1024.0 )
        return "Kb";
    if ( nBits < 1024.0 * 1024.0 * 1024.0 )
        return "Mb";
    else
        return "Gb";
}

float MainWindow::getDivisor(float nBits)
{
    if ( nBits < 1024.0 )
        return 1.0f;
    if ( nBits < 1024.0 * 1024.0 )
        return 1024.0f;
    if ( nBits < 1024.0 * 1024.0 * 1024.0 )
        return 1024.0f*1024.0f;
    else
        return 1024.0f*1024.0f*1024.0f;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //Total data downloaded:
    QString DownloadKBTot_S;
    QString UploadKBTot_S;

    float DataDownloadedSoFar = PCHandler.get_TotalDataDownloaded_bytes() + Download_offset;
    float DataUploadedSoFar = PCHandler.get_TotalDataUploaded_bytes() + Upload_offset;

    float divisor = getDivisor( DataDownloadedSoFar );
    DownloadKBTotal->setText( DownloadKBTot_S.setNum( (int) (DataDownloadedSoFar / divisor) ) );
    QString DownloadKBTotal_units_S = QString( getUnits(DataDownloadedSoFar).c_str());
    DownloadKBTotal_units->setText(DownloadKBTotal_units_S);

    divisor = getDivisor( DataUploadedSoFar );
    UploadKBTotal->setText( UploadKBTot_S.setNum( (int) (DataUploadedSoFar / (divisor) ) ) );
    QString UploadKBTotal_units_S = QString( getUnits( DataUploadedSoFar ).c_str());
    UploadKBTotal_units->setText(UploadKBTotal_units_S);

    //kb/s:
    QString DownloadKBpS_S;
    QString UploadKBpS_S;

    divisor = getDivisor( DataDownloadedSinceLastCall );
    DownloadKBpS->setText( DownloadKBpS_S.setNum( (int) (DataDownloadedSinceLastCall/divisor) ));
    QString DownloadKBpS_units_S = QString( getUnits(DataDownloadedSinceLastCall).c_str() + QString("/s")  );
    DownloadKBpS_units->setText(DownloadKBpS_units_S);

    divisor = getDivisor( DataUploadedSinceLastCall );
    UploadKBpS->setText( UploadKBpS_S.setNum((int) (DataUploadedSinceLastCall/divisor) ));
    QString UploadKBpS_units_S = QString( getUnits(DataUploadedSinceLastCall).c_str() + QString("/s") );
    UploadKBpS_units->setText(UploadKBpS_units_S);

    //Update the messages
    Console->Display_Messages( PCHandler.get_messages() );
}

void MainWindow::updateKBPS()
{
    DataDownloadedSinceLastCall =
    (PCHandler.get_TotalDataDownloaded_bytes() + Download_offset)-LastAmountData_download;
    LastAmountData_download = (PCHandler.get_TotalDataDownloaded_bytes() + Download_offset);

    DataUploadedSinceLastCall =
    (PCHandler.get_TotalDataUploaded_bytes() + Upload_offset)-LastAmountData_upload;
    LastAmountData_upload = (PCHandler.get_TotalDataUploaded_bytes() + Upload_offset);

    //Push one in, remove one
    SpeedHist_Download.push_back( DataDownloadedSinceLastCall );
    SpeedHist_Download.erase( SpeedHist_Download.begin(), SpeedHist_Download.begin()+1);

    SpeedHist_Upload.push_back( DataUploadedSinceLastCall );
    SpeedHist_Upload.erase( SpeedHist_Upload.begin(), SpeedHist_Upload.begin()+1);

    dataScope->Set_Data(SpeedHist_Download, 0);
    dataScope->Set_Data(SpeedHist_Upload, 1);

    //Save the data to a file
    InfoReadWrite Writer;
    vector<float> data_out;
    data_out.push_back( PCHandler.get_TotalDataDownloaded_bytes() + Download_offset);
    data_out.push_back( PCHandler.get_TotalDataUploaded_bytes() + Upload_offset);
    Writer.setData(data_out);
    Writer.Write("Stats.dat");
}

void MainWindow::clearMemory()
{
    for (int x=0; x< SpeedHist_Download.size(); x++)
    {
        SpeedHist_Download[x] = 0.0;
    }
    for (int x=0; x< SpeedHist_Upload.size(); x++)
    {
        SpeedHist_Upload[x] = 0.0;
    }

    LastAmountData_download=0.0;
    LastAmountData_upload=0.0;
    DataDownloadedSinceLastCall=0.0;
    DataUploadedSinceLastCall=0.0;

    PCHandler.resetMemory();
}

