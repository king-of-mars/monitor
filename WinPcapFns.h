#ifndef WINPCAPFNS_H
#define WINPCAPFNS_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>

//Unix
#include "pcap.h"

#ifndef WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
#else
	#include <winsock.h>
#endif

using namespace std;

/*!
Contains the description of the Pcap device and all the functions
that wrap around the WinPcap lib to get stats on the network traffic.
This class was constructed to be as independent as possible from the GUI.
(So far 100% independent).
*/
class PcapHandler
{
    public:

        //!Constructor
        PcapHandler();

        //!Prints the messages queued
        void Print_messages();

        //!Returns the messages and erase them afterwards
        //!\returns vector<string>
        vector<string> get_messages();

        /*!
            Returns available devices.

            Possible errors:
                Under Unix: If no device is found, maybe the program wasn't
                run with sufficient rights, try running as root (sudo ./app).
        */
        int FindAvailDevices(vector<string> * outDevices = NULL);

        /*! Opens the device. */
        int openDevice(int inum);

        /*!
        Starts capture on the currently open device,
        justs outputs to std the length of the packets
        */
        int StartListenOnDevice_countData();

        void instructStop();

        /*!
        Checks if the current device is ready to use
        \returns
        0: No
        1: Yes
        */
        int CheckIfCurrDeviceReady();

        /*!
        Gets the device's IP on the subnet
        */
        string getDeviceIP(int DeviceNo);

        //!Thread-safe function to get TotalDataTransferred_bytes
        //!(Implemented with QMutex)
        float get_TotalDataDownloaded_bytes();
        float get_TotalDataUploaded_bytes();
        void resetMemory();

    private:

        //!Thread-safe function to set TotalDataTransferred_bytes
        //!(Implemented with QMutex)
        void set_TotalDataDownloaded_bytes(float TDTb_in);
        void set_TotalDataUploaded_bytes(float TDTb_in);

        //!The messages reported by the application
        vector<string> messages;

        //!The available devices list
        pcap_if_t *alldevs;
        //!The number of available devices
        int Ndevices;
        /*!
        If the device is ready to be used.
        0  = Not initialised
        1  = Ready
        -1 = Error
        */
        int DeviceReady;

        //!The opened device
        pcap_t *adhandle;

        //!Total data transmitted in bytes
        float TotalDataDownloaded_bytes;
        float TotalDataUploaded_bytes;

        //!The ip address of the current device
        string IPadd;

        bool stopInstructed;
};

/*! 4 bytes IP address */
typedef struct ip_address{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;
}ip_address;

/*! IPv4 header */
typedef struct ip_header{
    u_char  ver_ihl;        // Version (4 bits) + Internet header length (4 bits)
    u_char  tos;            // Type of service
    u_short tlen;           // Total length
    u_short identification; // Identification
    u_short flags_fo;       // Flags (3 bits) + Fragment offset (13 bits)
    u_char  ttl;            // Time to live
    u_char  proto;          // Protocol
    u_short crc;            // Header checksum
    ip_address  saddr;      // Source address
    ip_address  daddr;      // Destination address
    u_int   op_pad;         // Option + Padding
}ip_header;

//Little functions that have nothing to do with network
string int_to_string(int Iin);
string ip_to_String(int a,int b, int c, int d);

#endif
