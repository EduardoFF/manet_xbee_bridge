#include "routing_driver.h"
#include <sys/time.h>
#define IT(c) __typeof((c).begin())
#define FOREACH(i,c) for(__typeof((c).begin()) i=(c).begin();i!=(c).end();++i)

std::string
ROUTINGDriver::getTimeStr()
{
#ifndef FOOTBOT_LQL_SIM
  char buffer [80];
  timeval curTime;
  gettimeofday(&curTime, NULL);
  int milli = curTime.tv_usec / 1000;
  strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
  char currentTime[84] = "";
  sprintf(currentTime, "%s:%d", buffer, milli);
  std::string ctime_str(currentTime);
  return ctime_str;
#else
  return "mytime";
#endif
}


/// returns time in milliseconds
  uint64_t 
ROUTINGDriver::getTime()
{
  struct timeval timestamp;
  gettimeofday(&timestamp, NULL);

  uint64_t ms1 = (uint64_t) timestamp.tv_sec;
  ms1*=1000;

  uint64_t ms2 = (uint64_t) timestamp.tv_usec;
  ms2/=1000;

  return (ms1+ms2);
}

ROUTINGDriver::ROUTINGDriver()
{
}

ROUTINGDriver::ROUTINGDriver(const char * url, 
		     const string &channel,
		     bool autorun)
{
  /// Create a new LCM instance
  m_lcm = lcm_create(url);
  /// Set LCM URL
  m_lcmURL = url;
  /// Set LCM channel
  m_lcmChannel = channel;
  /// FIXME: better outside?
  if (isLCMReady()) 
    {
      subscribeToChannel(channel);
    }
  pthread_mutex_init(&m_mutex, NULL);
  if( autorun )
    run();
}

bool
ROUTINGDriver::run()
{
  int status = pthread_create(&m_thread, NULL, internalThreadEntryFunc, this);
  return (status == 0);
}

void 
ROUTINGDriver::internalThreadEntry() 
{
  while (true) 
  {
    m_lcm.handle();
  }
}

inline bool 
ROUTINGDriver::isLCMReady() 
{
  if (!m_lcm.good()) 
  {
    printf("LCM is not ready :(");
    return false;
  } 
  else 
  {
    printf("LCM is ready :)");
    return true;
  }
}

/// lock, copy, unlock, return
TimestampedROUTINGData
ROUTINGDriver::data()
{
  pthread_mutex_lock(&m_mutex);
  TimestampedROUTINGData ret( m_latestData );
  pthread_mutex_unlock(&m_mutex);
  return ret;
}

inline void 
ROUTINGDriver::subscribeToChannel(const string & channel)
{
  printf("Listening to channel %s\n", channel.c_str());
  m_lcm.subscribe(channel, &ROUTINGDriver::handleMessage, this);
}

void 
ROUTINGDriver::handleMessage(const lcm::ReceiveBuffer* rbuf, 
			 const std::string& chan, 
			 const route2_tree_t* msg)
{
  uint64_t tt = getTime();
  //  uint8_t  rid = msg->robotid;
  printf("ROUTINGDriver: [%lld] got config \n", tt);
}

