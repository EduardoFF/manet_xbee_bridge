#include "gps_driver.h"
#include <sys/time.h>
#define IT(c) __typeof((c).begin())
#define FOREACH(i,c) for(__typeof((c).begin()) i=(c).begin();i!=(c).end();++i)



GPSDriver::GPSDriver(const char * url,const string &channel,bool autorun):
  m_uselcm(true)
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

GPSDriver::GPSDriver(bool autorun):
  m_uselcm(false)
{
  pthread_mutex_init(&m_mutex, NULL);
  if( m_gpsdClient.start())
    {
      fprintf(stderr, "gpsd client could not start!\n");
      m_gpsdOk=false;
    }
  else
    m_gpsdOk = true;
    
  if( autorun )
    run();
}

bool
GPSDriver::run()
{
  int status = pthread_create(&m_thread, NULL, internalThreadEntryFunc, this);
  return (status == 0);
}

void
GPSDriver::handleMessage(const lcm::ReceiveBuffer* rbuf,
             const std::string& chan,
             const pose_t* msg)
{
  uint64_t tt = getTime();
  //  uint8_t  rid = msg->robotid;
  printf("GPSDriver: [%lud] got config \n", tt);

}

/// lock, copy, unlock, return
TimestampedGPSData
GPSDriver::data()
{
  pthread_mutex_lock(&m_mutex);
  TimestampedGPSData ret( m_latestData );
  pthread_mutex_unlock(&m_mutex);
  return ret;
}

/// returns time in milliseconds
uint64_t
GPSDriver::getTime()
{
  struct timeval timestamp;
  gettimeofday(&timestamp, NULL);

  uint64_t ms1 = (uint64_t) timestamp.tv_sec;
  ms1*=1000;

  uint64_t ms2 = (uint64_t) timestamp.tv_usec;
  ms2/=1000;

  return (ms1+ms2);
}

std::string
GPSDriver::getTimeStr()
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

inline bool
GPSDriver::isLCMReady()
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

inline void
GPSDriver::subscribeToChannel(const string & channel)
{
  printf("Listening to channel %s\n", channel.c_str());
  m_lcm.subscribe(channel, &GPSDriver::handleMessage, this);
}

void
GPSDriver::internalThreadEntry()
{
  while (true)
    {
      
    if( m_uselcm )
      {
	m_lcm.handle();
      }
    else
      {
	if( m_gpsdOk )
	  {
	    printf("doing gpsd ...\n");
	    m_gpsdClient.step();
	  }
	/// or gpsd client
      }
  }
}
