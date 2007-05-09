#include "WinPcapFns.h"

//-------------Little useful functions-----------
string int_to_string(int Iin)
{
    std::stringstream ss;
    std::string str;
    ss << Iin;
    ss >> str;

    return str;
}

string ip_to_String(int a,int b, int c, int d)
{
    string ip;
    ip.append(int_to_string(a));
    ip.append(".");
    ip.append(int_to_string(b));
    ip.append(".");
    ip.append(int_to_string(c));
    ip.append(".");
    ip.append(int_to_string(d));

    return ip;
}

//-----------------------------------------------

PcapHandler::PcapHandler()
{
    TotalDataTransferred_bytes = 0.0;
    Ndevices = 0;
    DeviceReady = 0;
}

void PcapHandler::Print_messages()
{
    for (unsigned int x=0; x<messages.size(); x++)
    {
        cout<<messages[x]<<endl;
    }
}

vector<string> PcapHandler::get_messages()
{
    vector<string> out_messages = messages;
    messages.clear();
    return out_messages;
}

int PcapHandler::FindAvailDevices(vector<string> * outDevices)
{
    if (outDevices!= NULL)
        outDevices->clear();

	pcap_if_t *d;// Iterator
	char errbuf[PCAP_ERRBUF_SIZE];

	/* Retrieve the device list */
	if(pcap_findalldevs(&alldevs, errbuf) == -1)
	{
	    //Has to be changed to handle the error message in errbuf
		messages.push_back("Error: (in pcap_findalldevs...)");
		pcap_freealldevs(alldevs);
		return(-1);
	}

	/* Print the list */
	int i=0;
	for(d=alldevs; d; d=d->next)
	{
	    if (i==0)
            messages.push_back("Found available devices:");

        string Dname(d->name);
        string Ddesc;

		if (d->description)
		{
            Ddesc = d->description;
		}
		else
		{
            Ddesc = "Error, no description available";
		}

		i++;
		Ndevices = i;

        outDevices->push_back(int_to_string(i) + ": " + Dname + " : " + Ddesc);
		messages.push_back(int_to_string(i) + ": " + Dname + " : " + Ddesc);
	}

	if(i==0)
	{
        messages.push_back("Error: no interfaces found! Make sure WinPcap(Windows) or pcap(Unix) is installed.");
        pcap_freealldevs(alldevs);
		return(-1);
	}

	return 0;
}

int PcapHandler::openDevice(int inum)
{
    pcap_if_t *d; //Iterator
    char errbuf[PCAP_ERRBUF_SIZE];

    if(inum < 1 || inum > Ndevices)
    {
        messages.push_back("Interface number out of range.");

        /* Free the device list */
        DeviceReady = -1;
        pcap_freealldevs(alldevs);
        return -1;
    }

    /* Jump to the selected adapter */
    int i=0;
    for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

    /* Open the device */
    /* Open the adapter */
    if ((adhandle= pcap_open_live(d->name,	// name of the device
                             65536,			// portion of the packet to capture.
                                            // 65536 grants that the whole packet will be captured on all the MACs.
                             1,				// promiscuous mode (nonzero means promiscuous)
                             1000,			// read timeout
                             errbuf			// error buffer
                             )) == NULL)
    {
        string device_name(d->name);
        messages.push_back("Unable to open the adapter, device '" + device_name + "' is not supported by WinPcap");

        /* Free the device list */
        pcap_freealldevs(alldevs);
        return -1;
    }

    /* At this point, we don't need any more the device list. Free it */
    pcap_freealldevs(alldevs);

    DeviceReady = 1;
    return 0;
}

int PcapHandler::CheckIfCurrDeviceReady()
{
    if (DeviceReady==1)
        return 1;
    else
    {
        messages.push_back("Device is not ready to use.");
        return 0;
    }
}

int PcapHandler::StartListenOnDevice_countData()
{
    if (!CheckIfCurrDeviceReady())
        return CheckIfCurrDeviceReady();

    int res=0;
    struct pcap_pkthdr *header;
    const u_char *pkt_data;

    //To retreive the source and dest. ips
    ip_header *ih;

    while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0){

        if(res == 0)
            /* Timeout elapsed */
            continue;

        set_TotalDataTransferred_bytes (get_TotalDataTransferred_bytes() + header->len);

        /* retireve the position of the ip header */
        ih = (ip_header *) (pkt_data + 14);

        /* print ip addresses and udp ports */

        string source_IP = ip_to_String (   ih->saddr.byte1,
                                            ih->saddr.byte2,
                                            ih->saddr.byte3,
                                            ih->saddr.byte4
                                        );
        string dest_IP = ip_to_String   (   ih->daddr.byte1,
                                            ih->daddr.byte2,
                                            ih->daddr.byte3,
                                            ih->daddr.byte4
                                        );

        cout<<source_IP<<"-->"<<dest_IP<<endl;
        //cout<<ih->saddr.byte1<<endl;
        //printf("%d.%d.%d.%d. -> %d.%d.%d.%d.\n",
    }

    if(res == -1)
    {
        string error_rep(pcap_geterr(adhandle));
        messages.push_back("Error reading the packets:" + error_rep);
        return -1;
    }

    return 0;
}

float PcapHandler::get_TotalDataTransferred_bytes()
{
    return TotalDataTransferred_bytes;
}

void PcapHandler::set_TotalDataTransferred_bytes(float TDTb_in)
{
    TotalDataTransferred_bytes = TDTb_in;
}
