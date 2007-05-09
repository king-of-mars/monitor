#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <assert.h> //For Unix

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QDockWidget>
#include <QLCDNumber>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include "../WinPcapFns.h"
#include "../ThreadListener.h"
#include "Scope.h"
#include "Console.h"

class MainWindow : public QWidget
{
    Q_OBJECT

    public:

        MainWindow();

        /*!
        Opens the Network device using the wrapper defined in
        WinPcapFns.h

        Will open the default device specified in the 'Default_DeviceNo'
        if no argument is specified.
        */
        void OpenDevice(int DeviceNO= -1);

        //!Starts capturing packets
        //!Has to be run from inside a thread (or will block the app)
        void StartCapture();

    protected:
       void paintEvent(QPaintEvent *event);

    private slots:
        /*!
        Update kbps info using 'LastAmountData'.
        */
        void updateKBPS();

        /*!
        Change the current device.
        Will read the device selected by 'DropListDeviceChoice'
        */
        void ChangeDevice();

    private:
        //!GUI fns
        string getUnits(float nBits);
        float getDivisor(float nBits);

        //!Pcap wrapper
            PcapHandler PCHandler;

            //Info on the data
            float LastAmountData_download;
            float LastAmountData_upload;
            float DataDownloadedSinceLastCall;
            float DataUploadedSinceLastCall;

            vector<float> SpeedHist_Download;
            vector<float> SpeedHist_Upload;

        //!The threaded object that gets data from the wrapper
            ThreadListener * ThreadL;

            //Display widgets:
            QGroupBox   *   DownloadUploadGB;

            QLabel      *   DownloadKBpS;
            QLabel      *   DownloadKBpS_label;
            QLabel      *   DownloadKBpS_units;

            QLabel      *   UploadKBpS;
            QLabel      *   UploadKBpS_label;
            QLabel      *   UploadKBpS_units;

            QLabel      *   DownloadKBTotal;
            QLabel      *   DownloadKBTotal_label;
            QLabel      *   DownloadKBTotal_units;

            QLabel      *   UploadKBTotal;
            QLabel      *   UploadKBTotal_label;
            QLabel      *   UploadKBTotal_units;

            QComboBox   *   DropListDeviceChoice;
            QGroupBox   *   DropListDeviceGB;
            QPushButton *   PushBDropListSetToCurrent;

            QConsole    *   Console;
            Scope       *   dataScope;

        //!GUI default values

            //!(Starts at 1)
            unsigned int Default_DeviceNo;

};

#endif
