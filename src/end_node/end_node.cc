#include <iostream>
#include <stdlib.h>
#include <GetPot.hpp>
#include "gps_driver.h"
#include "gps_manager/gps_manager.h"
#include "routing_driver.h"
#include "planning_driver/planning_driver.h"
#include "xbee_interface.h"
#include "xbee_app_data.h"
#include "flow_notifier/flow_notifier.h"
#include "timer.h"
#include <csignal>
#include <glog/logging.h>
#define FOREACH(i,c) for(__typeof((c).begin()) i=(c).begin();i!=(c).end();++i)
using namespace std;

//#define NO_XBEE_TEST 0

#ifndef NO_XBEE_TEST
XbeeInterface *g_xbee;
#endif

uint8_t g_nodeId;

PLANNINGDriver *g_planningDriver;
ROUTINGDriver *g_routingDriver;
FlowNotifier *g_flowNotifier;
GPSDriver *g_gpsDriver;
Timer *g_endNodeInfoTimer;
Timer *g_flowInfoTimer;
Timer *g_resetXbeeTimer;
char g_outBuf[130];
bool g_abort;

uint16_t g_lastRoutingTableRcv = 0;
std::set<uint8_t> g_lastRoutingFragments;

uint16_t g_lastPlanningTableRcv = 0;
std::set<uint8_t> g_lastPlanningFragments;

/// Function to print Help if need be
void print_help(const string Application)
{
    exit(0);
}

/// mutex for sending one packet at a time
pthread_mutex_t g_sendMutex;

std::string
getNodeDesc(uint8_t nid)
{
    if( !nid )
        return "SINK";
    ostringstream ss;
    ss << "10.42.43." << +nid;
    return ss.str();
}


void
stopTimer(Timer *timer)
{
    if( timer )
    {
        timer->stop();
	int cnt=10;
        while( timer->isRunning() && cnt--)
        {
            sleep(1);
            timer->stop();
        }
    }
}

void killTimer( Timer *timer)
{
  if( !timer )
    return;
  stopTimer(timer);
  delete timer;
}

///// here we send the node info packet /////
void
flowInfoTimerCB(void *arg)
{
  if( g_abort )
    return;

  DLOG(INFO) << "flowInfoTimer triggered";


  /// lock the mutex first
   using namespace xbee_app_data;

  //// compose node info packet
  Header hdr;

  /// make header
  hdr.src  = g_nodeId;
  hdr.type = XBEEDATA_FLOWINFO;
  memcpy(g_outBuf, &hdr, sizeof(Header));

  /// make payload
  FlowInfoEntry fInfo;
  FlowList fList = g_flowNotifier->getFlows(2000);
  if( !fList.size()  )
    {
      LOG(INFO) << "No flows to notify";
      return;
    }
  LOG(INFO) << "Got some " << fList.size() << " flows to notify";

  FlowInfoHdr *fInfoHdr = (FlowInfoHdr *) (g_outBuf+sizeof(Header));
  fInfoHdr->nEntries=0;
  
  char *ptr = (char *) (g_outBuf+sizeof(Header)+sizeof(FlowInfoHdr));
  size_t buflen = sizeof(Header)+sizeof(FlowInfoHdr);
      
  FOREACH(it, fList )
    {
      fInfo.nodeId =g_flowNotifier->getIdFromAddressBook(it->src_addr);
      if( fInfo.nodeId <= 0)
	continue;
      fInfo.dataRate = it->data_rate;
      memcpy(ptr, &fInfo, sizeof(FlowInfoEntry));
      ptr += sizeof(FlowInfoEntry);
      buflen += sizeof(FlowInfoEntry);
      fInfoHdr->nEntries+=1;
      LOG(INFO) << "Flow " << fInfo.nodeId << " " <<  fInfo.dataRate;
    }
  if( !fInfoHdr->nEntries )
    {
      LOG(INFO) << "No active flows to notify";
      return;
    }
  else
    {
      LOG(INFO) << "Notifying "
		<< fInfoHdr->nEntries << " flows";
    }

    XbeeInterface::TxInfo txInfo;
    txInfo.reqAck  = false;
    txInfo.readCCA = false;

#ifndef NO_XBEE_TEST
pthread_mutex_lock(&g_sendMutex);
 
    int retval = g_xbee->send(0xffff, txInfo, g_outBuf, buflen);
    if( retval == XbeeInterface::NO_ACK )
    {
      LOG(INFO) << "send failed NOACK";
    }
    else if( retval == XbeeInterface::TX_MAC_BUSY )
    {
      LOG(INFO) << "send failed MACBUSY";
    }
    else
    {
      LOG(INFO) << "send OK";
    }

    pthread_mutex_unlock(&g_sendMutex);
#endif
  
  //TimestampedGPSData gpsData = g_gpsDriver->data();
  //  eInfo.dataRate  = g_dataRateMon->data();
}

///// here we send the node info packet /////
void
endNodeInfoTimerCB(void *arg)
{
    if( g_abort )
        return;
    LOG(INFO) << "endNodeInfoTimer triggered";

    /// lock the mutex first
    pthread_mutex_lock(&g_sendMutex);
    using namespace xbee_app_data;

    //// compose node info packet
    Header hdr;

    /// make header
    hdr.src = g_nodeId;
    hdr.type = XBEEDATA_ENDNODEINFO;
    memcpy(g_outBuf, &hdr, sizeof(Header));

    /// make payload
    EndNodeInfo eInfo;
    TimestampedGPSData gpsData = g_gpsDriver->data();
    eInfo.latitude  = gpsData.lat;
    eInfo.longitude = gpsData.lon;
    eInfo.altitude  = gpsData.alt;
    LOG(INFO) << "Sending EndNodeInfo "
	      << " " << eInfo.latitude
	      << " " << eInfo.longitude
	      << " " << eInfo.altitude;

    //  eInfo.dataRate  = g_dataRateMon->data();

    memcpy(g_outBuf+sizeof(Header), &eInfo, sizeof(EndNodeInfo));
    size_t buflen = sizeof(Header) + sizeof(EndNodeInfo);
    XbeeInterface::TxInfo txInfo;
    txInfo.reqAck  = false;
    txInfo.readCCA = false;

#ifndef NO_XBEE_TEST
    int retval = g_xbee->send(0xffff, txInfo, g_outBuf, buflen);
    if( retval == XbeeInterface::NO_ACK )
    {
      LOG(INFO) << "send failed NOACK";
    }
    else if( retval == XbeeInterface::TX_MAC_BUSY )
    {
      LOG(INFO) << "send failed MACBUSY";
    }
    else
    {
      LOG(INFO) << "send OK";
    }
#endif
    pthread_mutex_unlock(&g_sendMutex);
}


void
resetXbeeTimerCB(void *arg)
{
    if( g_abort )
        return;
    LOG(INFO) << "Resetting Xbee ...";
    g_xbee->sendReset();
    LOG(INFO) << "Reset done.";
}
void
signalHandler( int signum )
{
    g_abort = true;
    printf("ending app...\n");
    LOG(INFO) << "Ending app";
    /// stop timers
    killTimer(g_endNodeInfoTimer);
    killTimer(g_flowInfoTimer);
    killTimer(g_resetXbeeTimer);
    
    
    delete g_flowNotifier;
    delete g_gpsDriver;
    
#ifndef NO_XBEE_TEST
    delete g_xbee;
#endif
    LOG(INFO) << "Clean exit";

    exit(signum);
}

/// Receive the Xbee data and Use it
void
receiveData(uint16_t addr, void *data,
            char rssi, timespec timestamp, size_t len)
{
    using namespace xbee_app_data;
    LOG(INFO) << "Got data from " << addr
         << " rssi: " << +rssi << ") "
	      <<  " len: " << len;
    if (len > sizeof(Header))
    {
        Header header;
        memcpy(&header, data, sizeof(Header));
        LOG(INFO) << "Header: " << +(header.type);
        if (header.type == XBEEDATA_ROUTING )
        {
            if(len >= sizeof(Header) + sizeof(Routing))
            {
                Routing route;
                memcpy(&route,
                       (unsigned char *)data + sizeof(Header),
                       sizeof(Routing));
                LOG(INFO) << "TabId: " << +(route.tabId);
                if( route.tabId > g_lastRoutingTableRcv )
                {
		  LOG(INFO) << "NEW TREE " <<  route.tabId;
                    /// we clear the routing table,
                    /// annotate the timestamp
                    g_routingDriver->clearData(0);
                    g_lastRoutingFragments.clear();
                    g_lastRoutingTableRcv=route.tabId;
                }

                /// check if we already got this fragment
                if( g_lastRoutingFragments.find(route.fragNb) !=
                        g_lastRoutingFragments.end())
                {
                    /// we got it before, so we dont need to process it again
                    return;
                }
                size_t bcnt = sizeof(Header) + sizeof(Routing);
                if( len != bcnt + route.nbBytes )
                {
		  LOG(INFO) << "Error occurred";
                    fprintf(stderr, "Incorrect buffer length (%lu) should be %lu\n",
                            len, sizeof(Header) + sizeof(Routing) + route.nbBytes);
                    return;
                }
                /// so far so good, not we can read the remaining len-bcnt bytes
                unsigned char *ptr = (unsigned char *)data + bcnt;
                while(bcnt < len)
                {
                    if( len-bcnt < sizeof(RoutingTableHdr) )
                    {
                        fprintf(stderr, "Invalid length for routing msg (routing header)\n");
                        exit(1);
                    }
                    RoutingTableHdr *rtHdr = (RoutingTableHdr*)ptr;
                    std::string nodedesc = getNodeDesc(rtHdr->nodeId);
                    int nentries = rtHdr->nEntries;
                    LOG(INFO) << "TABLE FOR " << nodedesc
                              << " with " << nentries << " entries";
                    ptr+=sizeof(RoutingTableHdr);
                    bcnt+=sizeof(RoutingTableHdr);
                    for(int j=0; j<nentries;j++)
                    {
                        if( len-bcnt < sizeof(RoutingEntry) )
                        {
                            fprintf(stderr, "Invalid length for routing msg (routing header)\n");
                            exit(1);
                        }
                        RoutingEntry *rentry = (RoutingEntry*)ptr;
                        Entry entry;
                        entry.endNode = getNodeDesc(rentry->dest);
                        entry.nextHop = getNodeDesc(rentry->nextHop);
                        entry.weight = rentry->weight;
                        g_routingDriver->addEntry(nodedesc, entry);
                        ptr+=sizeof(RoutingEntry);
                        bcnt+=sizeof(RoutingEntry);
                    }
                }
                if( bcnt == len)
                {
                    printf("Read all bytes :)\n");
                }
                printf("NEW FRAGMENT %d\n", route.fragNb);
                g_lastRoutingFragments.insert(route.fragNb);
                printf("got %ld out of %d\n", g_lastRoutingFragments.size(),
                       route.nbOfFrag);
                g_routingDriver->publishLCM();
                if( g_lastRoutingFragments.size() == route.nbOfFrag )
                {
		  LOG(INFO) << "NEW TABLE " << g_routingDriver->data();
                }
            }
            else
            {
                fprintf(stderr, "Invalid length for routing msg\n");
            }
        }

        if (header.type == XBEEDATA_PLANNING )
        {
            if(len >= sizeof(Header) + sizeof(Planning))
            {
                Planning plan;
                memcpy(&plan,
                       (unsigned char *)data + sizeof(Header),
                       sizeof(Planning));
                LOG(INFO) << "TabId: " << +(plan.tabId);
                if( plan.tabId > g_lastPlanningTableRcv )
                {
                    printf("NEW PLAN %d\n", plan.tabId);
                    /// we clear the routing table,
                    /// annotate the timestamp
                    g_planningDriver->clearData(0);
                    g_lastPlanningFragments.clear();
                    g_lastPlanningTableRcv=plan.tabId;
                }

                /// check if we already got this fragment
                if( g_lastPlanningFragments.find(plan.fragNb) !=
                        g_lastPlanningFragments.end())
                {
                    /// we got it before, so we dont need to process it again
                    return;
                }
                size_t bcnt = sizeof(Header) + sizeof(Planning);
                if( len != bcnt + plan.nbBytes )
                {
                    fprintf(stderr, "Incorrect buffer length (%lu) should be %lu\n",
                            len, sizeof(Header) + sizeof(Planning) + plan.nbBytes);
                    return;
                }
                /// so far so good, not we can read the remaining len-bcnt bytes
                unsigned char *ptr = (unsigned char *)data + bcnt;
                while(bcnt < len)
                {
                    if( len-bcnt < sizeof(PlanningTableHdr) )
                    {
                        fprintf(stderr, "Invalid length for planning msg (planning header)\n");
                        exit(1);
                    }
                    PlanningTableHdr *rtHdr = (PlanningTableHdr*)ptr;
                    std::string nodedesc = getNodeDesc(rtHdr->nodeId);
                    int nentries = rtHdr->nEntries;
                    LOG(INFO) << "TABLE FOR " << nodedesc
                              << " with " << nentries << " entries";
                    ptr+=sizeof(PlanningTableHdr);
                    bcnt+=sizeof(PlanningTableHdr);
                    for(int j=0; j<nentries;j++)
                    {
                        if( len-bcnt < sizeof(PlanningEntry) )
                        {
                            fprintf(stderr, "Invalid length for routing msg (routing header)\n");
                            exit(1);
                        }
                        PlanningEntry *rentry = (PlanningEntry*)ptr;
                        EntryP entry;
                        entry.latitude = rentry->latitude;
                        entry.longitude = rentry->longitude;
                        entry.altitude = rentry->altitude;
                        entry.action = rentry->action;
                        entry.option = rentry->option;
                        entry.timestamp = rentry->timestamp;
                        g_planningDriver->addEntry(nodedesc, entry);
                        ptr+=sizeof(PlanningEntry);
                        bcnt+=sizeof(PlanningEntry);
                    }
                }
                if( bcnt == len)
                {
                    printf("Read all bytes :)\n");
                }
                printf("NEW FRAGMENT %d\n", plan.fragNb);
                g_lastPlanningFragments.insert(plan.fragNb);
                printf("got %ld out of %d\n", g_lastPlanningFragments.size(),
                       plan.nbOfFrag);
                g_planningDriver->publishLCM();
                if( g_lastPlanningFragments.size() == plan.nbOfFrag )
                {
		  LOG(INFO) << "NEW TABLE " << g_planningDriver->data();
                }
            }
            else
            {
                fprintf(stderr, "Invalid length for planning msg\n");
            }
        }
    }
}

/////////////// Beginning of Main program //////////////////
int main(int argc, char * argv[])
{

    g_abort = false;


    /// register signal
    signal(SIGINT, signalHandler);

    /// Simple Command line parser
    GetPot   cl(argc, argv);
    if(cl.search(2, "--help", "-h") ) print_help(cl[0]);
    cl.init_multiple_occurrence();

    cl.enable_loop();
    const string  xbeeDev   = cl.follow("/dev/ttyUSB0", "--dev");
    const int     baudrate  = cl.follow(57600, "--baud");
    g_nodeId    = cl.follow(2, "--nodeid");
    const bool    use_gpsd  = cl.follow(true, "--use-gpsd");
    const string  addrBook  = cl.follow("none", "--abook");
    const string  myIp   = cl.follow("none", "--ip");
    const string  myMac  = cl.follow("none", "--mac");
    const string  xbeeMode = cl.follow("xbee5", "--mode");
    const string  logDir  = cl.follow("/tmp/", "--logdir");
    const int     infoPeriod  = cl.follow(1, "--info-period");
    const int     flowPeriod  = cl.follow(3, "--flow-period");
    const int     resetPeriod  = cl.follow(300, "--reset-period");

    /// Initialize Log
    google::InitGoogleLogging(argv[0]);
    FLAGS_logbufsecs = 0;
    FLAGS_log_dir = logDir;
    LOG(INFO) << "Logging initialized";

    LOG(INFO) << "using: nodeId " << g_nodeId;
    

    XbeeInterfaceParam xbeePar;
    xbeePar.SourceAddress   = g_nodeId;
    xbeePar.brate           = baudrate;
    xbeePar.mode            = xbeeMode;
    xbeePar.Device          = xbeeDev;
    xbeePar.writeParams     = true;

    /// create mutexes
    if (pthread_mutex_init(&g_sendMutex, NULL) != 0)
    {
        fprintf(stderr, "mutex init failed\n");
        fflush(stderr);
        exit(-1);
    }
#ifndef NO_XBEE_TEST
    /// create xbee connection
    g_xbee = new XbeeInterface(xbeePar);
    g_xbee->registerReceive(&receiveData);
#endif

    if( use_gpsd )
    {
      LOG(INFO)  << "USING GPSD";
      g_gpsDriver = new GPSDriver("udpm://239.255.76.67:7667?ttl=1", "POSE", true, true);
        /// create gpsDriver connection
	//        g_gpsDriver = new GPSDriver(true);
    }
    else
    {
        /// create gpsDriver connection
      g_gpsDriver = new GPSDriver("udpm://239.255.76.67:7667?ttl=1", "POSE", true, false);
    }

    g_flowNotifier = new FlowNotifier("udpm://239.255.76.67:7667?ttl=1", "iflow", true);
    if( addrBook != "none" )
      g_flowNotifier->readAddressBook(addrBook);

    /// create routing Driver -- we don't need to listen LCM
    g_routingDriver = new ROUTINGDriver("udpm://239.255.76.67:7667?ttl=1", "RNP2", false);

    /// create planningDriver connection
    g_planningDriver = new PLANNINGDriver("udpm://239.255.76.67:7667?ttl=1", "PLAN", false);

    if( infoPeriod > 0)
      {
	g_endNodeInfoTimer = new Timer(TIMER_SECONDS, endNodeInfoTimerCB, NULL);
	g_endNodeInfoTimer->startPeriodic(infoPeriod);
      }
    else
      g_endNodeInfoTimer = NULL;


    if( flowPeriod > 0 )
      {
	g_flowInfoTimer = new Timer(TIMER_SECONDS, flowInfoTimerCB, NULL);
	g_flowInfoTimer->startPeriodic(flowPeriod);
      }
    else
      g_flowInfoTimer = NULL;

    if( resetPeriod > 60)
      {
	g_resetXbeeTimer = new Timer(TIMER_SECONDS, resetXbeeTimerCB, NULL);
	g_resetXbeeTimer->startPeriodic(resetPeriod);
      }
    else
      g_resetXbeeTimer = NULL;
    

    /// Sleep
    for(;;)
    {
        sleep(1);
    }

    return 0;
}
