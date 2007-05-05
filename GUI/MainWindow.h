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
#include <QHostInfo>
#include <QHostAddress>

#include "../WinPcapFns.h"
#include "../ThreadListener.h"
#include "Scope.h"
#include "Console.h"

class MainWindow : public QWidget
{
    Q_OBJECT

    public:

        MainWindow();

        //!Opens the Network device using the wrapper defined in
        //!WinPcapFns.h
        void OpenDevice();

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

    private:

        //!Pcap wrapper
        PcapHandler PCHandler;

        //Info on the data
        double LastAmountData;
        vector<float> DownloadSpeedHist;

        //!The threaded object that gets data from the wrapper
        ThreadListener * ThreadL;

        //Display widgets:
        QGroupBox   *   DownloadUploadGB;
        QLCDNumber  *   LCDDownloadKB;
        QLCDNumber  *   LCDUploadKB;

        QConsole    *   Console;
        Scope       *   DownloadScope;

};

#endif
