#include <iostream>
#include <stdlib.h>
#include <GetPot.hpp>
#include "gps_driver.h"
#include "routing_driver.h"
#include "xbee_interface.h"
#include "xbee_app_data.h"
#include "timer.h"
#include <csignal>

using namespace std;

#ifndef IT
#define IT(c) __typeof((c).begin())
#endif

#ifndef FOREACH
#define FOREACH(i,c) for(__typeof((c).begin()) i=(c).begin();i!=(c).end();++i)
#endif


//#define NO_XBEE_TEST

XbeeInterface *g_xbee;
ROUTINGDriver *g_routingDriver;
Timer *g_sendRoutingDataTimer;

char g_outBuf[130];
bool g_abort;

TimestampedROUTINGData g_lastRoutingDataSent;
uint16_t g_lastXbeeTableId;
vector< vector<uint8_t> > g_routingXbeeMsgs;

/// mutex for sending one packet at a time
pthread_mutex_t g_sendMutex;

/// Function to print Help if need be
void print_help(const string Application)
{
  exit(0);
}


/// let's assume that desc is either 'SINK' or an IP address
uint8_t getNodeId(std::string desc)
{
  if( desc == "SINK" )
    {
      return 0;
    }
  else
    {
      uint8_t value[4] = {0};
      size_t index = 0;
      FOREACH(ct, desc)
	{
        if (isdigit((unsigned char)*ct))
	  {
	    value[index] *= 10;
	    value[index] += *ct - '0';
	  }
	else
	  {
	    index++;
	  }
      }
      //TODO error checking
      return value[3];
    }
}

///// here we send the node info packet /////
void
sendRoutingDataTimerCB(void *arg)
{
  if( g_abort )
    return;
  /// lock the mutex first
  pthread_mutex_lock(&g_sendMutex);
  using namespace xbee_app_data;


  // make payload
  //structure the data packets for Xbee
  TimestampedROUTINGData routingData = g_routingDriver->data();
  //
  //
  //
  //
  //
  
  /// first, check if the routing data has changed by comparing the
  /// timestamps
  if( routingData.timestamp > g_lastRoutingDataSent.timestamp )
    {
      /// it has changed ...
      /// we need to create new xbee messages
      IT(routingData.route) it = routingData.route.begin();

      g_routingXbeeMsgs.clear();
      for(;;)
	{
	  std::vector<uint8_t> dataVec;
	  dataVec.reserve(XBEE_MAX_PAYLOAD_LENGTH);
	  int dataIx = 0;
	  //// compose node info packet
	  Header hdr;
	  /// make header
	  hdr.type = XBEEDATA_ROUTING;
	  dataVec.resize(dataVec.size() + sizeof(Header) );
	  memcpy(&dataVec[dataVec.size() - sizeof(Header)],
		 &hdr, sizeof(Header));
	  /// --- now the routing header
	  Routing routeHdr;
	  dataVec.resize(dataVec.size() + sizeof(Routing) );
	  memcpy(&dataVec[dataVec.size() - sizeof(Routing)],
		 &routeHdr, sizeof(Routing));
	  
	  /// let's forget for a moment about the fragment numbers
	  int nbytes=0;
	 
	  while(it != routingData.route.end() )
	    {
	      std::string nodedesc = it->first;
	      cout << "pushing table of " << nodedesc << endl;
	      RoutingTable table = it->second;
	      cout << table.size() << " entries " << endl;
	      /// can we fit all entries here ?
	      if( dataVec.size() + sizeof(xbee_app_data::RoutingTableHdr)
		  + sizeof(xbee_app_data::RoutingEntry)*table.size()
		  <= XBEE_MAX_PAYLOAD_LENGTH )
		{
		  RoutingTableHdr rtHdr;
		  /// we need somehow to map node identifiers to numeric ids (0-255)
		  NodeId nid = getNodeId(nodedesc);
		  rtHdr.nodeId = nid;
		  rtHdr.nEntries = static_cast<uint8_t>(table.size());
		  dataVec.resize(dataVec.size() + sizeof(RoutingTableHdr) );
		  memcpy(&dataVec[dataVec.size() - sizeof(RoutingTableHdr)],
			 &rtHdr, sizeof(RoutingTableHdr));		    
		  
		  for(int j=0; j<table.size(); j++)
		    {
		      RoutingEntry rtentry;
		      rtentry.dest = getNodeId(table[j].endNode);
		      rtentry.nextHop = getNodeId(table[j].nextHop);
		      rtentry.weight = table[j].weight;
		      dataVec.resize(dataVec.size() + sizeof(RoutingEntry) );
		      memcpy(&dataVec[dataVec.size() - sizeof(RoutingEntry)],
			     &rtentry, sizeof(RoutingEntry));
		    }
		  cout << "byte count " << dataVec.size() << endl;
		}
	      else
		{
		  ///
		  break;
		}
	      it++;
	    }
	  /// push the message
	  g_routingXbeeMsgs.push_back(dataVec);
	  if( it == routingData.route.end() )
	    {
	      /// we are done
	      break;
	    }
	}
      /// now we care about the fragments
      printf("generated msgs %u\n", g_routingXbeeMsgs.size());
      g_lastXbeeTableId++;
      uint8_t fgn = 0;
      FOREACH(jt, g_routingXbeeMsgs)
	{
	  assert((*jt).size() >= sizeof(Header) + sizeof(Routing));
	  if( (*jt).size() < sizeof(Header) + sizeof(Routing) )
	    {
	      fprintf(stderr, "something wrong here (%u < %u)\n",
		      (*jt).size(), sizeof(Header) + sizeof(Routing));
	      exit(1);
	    }
	  Routing *routeHdr = (Routing *)&(*jt)[sizeof(Header)];
	  routeHdr->tabId = g_lastXbeeTableId;
	  routeHdr->fragNb = fgn++;
	  routeHdr->nbOfFrag = static_cast<uint8_t>(g_routingXbeeMsgs.size());
	  routeHdr->nbBytes = (*jt).size() - sizeof(Header) - sizeof(Routing);
	}
      /// we're done
      g_lastRoutingDataSent = routingData;
    }
  else
    {
      /// we don't need to encode again the xbee payload since the
      /// routing hasn't changed
      printf("routing unchanged ...\n");
    }

  printf("sending %d msgs\n", g_routingXbeeMsgs.size());
  for(int i=0; i< g_routingXbeeMsgs.size(); i++)
    {
      printf("sending msgs %d out of %d with size %u\n", i+1,
	     g_routingXbeeMsgs.size(), g_routingXbeeMsgs[i].size() );
      std::cout << +(g_routingXbeeMsgs[i][0]) << std::endl;
  
      memcpy(g_outBuf, &g_routingXbeeMsgs[i][0], g_routingXbeeMsgs[i].size());
      size_t buflen = g_routingXbeeMsgs[i].size();
      XbeeInterface::TxInfo txInfo;
      txInfo.reqAck = true;
      txInfo.readCCA = false;

#ifndef NO_XBEE_TEST
      int retval = g_xbee->send(2, txInfo, g_outBuf, buflen);
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
#endif
    }

  pthread_mutex_unlock(&g_sendMutex);
}

void
signalHandler( int signum )
{
  g_abort = true;
  printf("ending app...\n");
  /// stop timers
  if( g_sendRoutingDataTimer )
    {
      g_sendRoutingDataTimer->stop();
      while( g_sendRoutingDataTimer->isRunning() )
        {
	  sleep(1);
	  g_sendRoutingDataTimer->stop();
        }
    }

  delete g_sendRoutingDataTimer;
  delete g_routingDriver;
  delete g_xbee;
  printf("done.\n");
  exit(signum);
}

void
receiveData(uint16_t addr, void *data, char rssi, timespec timestamp, size_t len)
{
  using namespace xbee_app_data;
  cout << "Got data from " << addr
       << " rssi: " << +rssi << ") "
       <<  " len: " << len << endl;
  cout << "-----------------------" << endl;
  if (len > sizeof(Header))
    {
      Header header;
      memcpy(&header, data, sizeof(Header));
      cout << "Header: " << header << endl;

      if (header.type == XBEEDATA_ENDNODEINFO )
        {
	  if(len == sizeof(Header) + sizeof(EndNodeInfo))
            {
	      EndNodeInfo eInfo;
	      memcpy(&eInfo,
		     (unsigned char *)data + sizeof(Header),
		     sizeof(EndNodeInfo));
	      cout << "EndNodeInfo: " << eInfo << endl;
            }
        }
    }
  cout << "-----------------------" << endl;
}

/////////////// Beginning of Main program //////////////////
int main(int argc, char * argv[])
{
  g_abort = false;
  g_lastRoutingDataSent.timestamp = 0;
  /// register signal
  signal(SIGINT, signalHandler);

  // Simple Command line parser
  GetPot   cl(argc, argv);
  if(cl.search(2, "--help", "-h") ) print_help(cl[0]);
  cl.init_multiple_occurrence();
  const string  xbeeDev  = cl.follow("/dev/ttyUSB0", "--dev");
  const int     baudrate = cl.follow(57600, "--baud");
  const int     nodeId   = cl.follow(1, "--nodeid");
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

  printf("Creating xbeeInterface\n");
  fflush(stdout);

#ifndef NO_XBEE_TEST
  /// create Xbee communication
  g_xbee = new XbeeInterface(xbeePar);
  /// Listen for messages
  g_xbee->registerReceive(&receiveData);
#endif

  /// create routing Driver
  g_routingDriver = new ROUTINGDriver("udpm://239.255.76.67:7667?ttl=1", "RNP", true);

  // does it need to be modified???
  g_sendRoutingDataTimer = new Timer(TIMER_SECONDS, sendRoutingDataTimerCB, NULL);
  g_sendRoutingDataTimer->startPeriodic(1);

  /// Sleep
  for(;;)
    {
      sleep(1);
    }

  return 0;
}
