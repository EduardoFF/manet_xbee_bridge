#include <iostream>
#include <stdlib.h>
#include <GetPot.hpp>
#include "gps_driver.h"
#include "xbee_interface.h"
#include "xbee_app_data.h"
#include "timer.h"
#include <csignal>

using namespace std;

XbeeInterface *g_xbee;
GPSDriver *g_gpsDriver;
Timer *g_endNodeInfoTimer;
char g_outBuf[130];
bool g_abort;

/// Function to print Help if need be
void print_help(const string Application)
{
    exit(0);
}

/// mutex for sending one packet at a time
pthread_mutex_t g_sendMutex;

///// here we send the node info packet /////
void
endNodeInfoTimerCB(void *arg)
{
    if( g_abort )
        return;

    /// lock the mutex first
    pthread_mutex_lock(&g_sendMutex);
    using namespace xbee_app_data;

    //// compose node info packet
    Header hdr;

    /// make header
    hdr.type = XBEEDATA_ENDNODEINFO;
    memcpy(g_outBuf, &hdr, sizeof(Header));

    // make payload
    EndNodeInfo eInfo;
    TimestampedGPSData gpsData = g_gpsDriver->data();
    eInfo.latitude  = gpsData.lat;
    eInfo.longitude = gpsData.lon;
    eInfo.altitude  = gpsData.alt;
    //  eInfo.dataRate  = g_dataRateMon->data();

    memcpy(g_outBuf+sizeof(Header), &eInfo, sizeof(EndNodeInfo));
    size_t buflen = sizeof(Header) + sizeof(EndNodeInfo);
    XbeeInterface::TxInfo txInfo;
    txInfo.reqAck = true;
    txInfo.readCCA = false;
    int retval = g_xbee->send(1, txInfo, g_outBuf, buflen);
    if( retval == XbeeInterface::NO_ACK )
    {
        printf("send failed NOACK\n");
    }
    else if( retval == XbeeInterface::TX_MAC_BUSY )
    {
        printf("send failed MACBUSY\n");
    }
    else
    {
        printf("send OK\n");
    }
    pthread_mutex_unlock(&g_sendMutex);
}

void
signalHandler( int signum )
{
    g_abort = true;
    printf("ending app...\n");
    /// stop timers
    if( g_endNodeInfoTimer )
    {
        g_endNodeInfoTimer->stop();
        while( g_endNodeInfoTimer->isRunning() )
        {
            sleep(1);
            g_endNodeInfoTimer->stop();
        }
    }
    delete g_endNodeInfoTimer;
    delete g_gpsDriver;
    delete g_xbee;
    printf("done.\n");
    exit(signum);
}

/// Receive the Xbee data and Use it
void
receiveData(uint16_t addr, void *data, char rssi, timespec timestamp, size_t len)
{
    using namespace xbee_app_data;
    cout << "Got data from " << addr
         << " rssi: " << rssi << ") "
         <<  " len: " << len << endl;
    cout << "-----------------------" << endl;
    if (len > sizeof(Header))
    {
        Header header;
        memcpy(&header, data, sizeof(Header));
        cout << "Header: " << header << endl;

        if (header.type == XBEEDATA_ROUTING )
        {
            if(len == sizeof(Header) + sizeof(Routing))
            {
                Routing route;
                memcpy(&route,
                       (unsigned char *)data + sizeof(Header),
                       sizeof(Routing));
            }
        }
    }
    cout << "-----------------------" << endl;
}


/////////////// Beginning of Main program //////////////////
int main(int argc, char * argv[])
{

    g_abort = false;
    /// register signal
    signal(SIGINT, signalHandler);

    // Simple Command line parser
    GetPot   cl(argc, argv);
    if(cl.search(2, "--help", "-h") ) print_help(cl[0]);
    cl.init_multiple_occurrence();
    const string  xbeeDev  = cl.follow("/dev/ttyUSB1", "--dev");
    const int     baudrate    = cl.follow(57600, "--baud");
    const int     nodeId    = cl.follow(2, "--nodeid");
    cl.enable_loop();

    XbeeInterfaceParam xbeePar;
    xbeePar.SourceAddress = nodeId;
    xbeePar.brate = baudrate;
    xbeePar.mode  = "xbee1";
    xbeePar.Device = xbeeDev;
    xbeePar.writeParams = false;

    /// create mutexes
    if (pthread_mutex_init(&g_sendMutex, NULL) != 0)
    {
        fprintf(stderr, "mutex init failed\n");
        fflush(stderr);
        exit(-1);
    }

    /// create xbee connection
    g_xbee = new XbeeInterface(xbeePar);

    /// create gpsDriver connection
    g_gpsDriver = new GPSDriver("udpm://239.255.76.67:7667?ttl=1", "POSE", true);

    g_endNodeInfoTimer = new Timer(TIMER_SECONDS, endNodeInfoTimerCB, NULL);
    g_endNodeInfoTimer->startPeriodic(1);

    /// Sleep
    for(;;)
    {
        sleep(1);
    }

    return 0;
}
