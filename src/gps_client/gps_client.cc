#include "gps_client.h"
#include <sys/time.h>

GPSDClient::GPSDClient()
  : m_gps(NULL), m_use_gps_time(true), m_check_fix_by_variance(true) {}

bool 
GPSDClient::start() 
{
  std::string m_host = "localhost";
  int m_port = 2947;
  char m_port_s[12];
  snprintf(m_port_s, 12, "%d", m_port);

  gps_data_t *m_resp = NULL;
  m_gps = new gpsmm(m_host.c_str(), m_port_s);
  m_resp = m_gps->stream(WATCH_ENABLE);

  if (m_resp == NULL)
    {
      printf("Failed to open GPSd");
      return false;
    }
  printf("GPSd opened\n");
  return true;
}

void
GPSDClient::step() 
{
  if (!m_gps->waiting(1e6))
    return;
  gps_data_t *p = m_gps->read();
  process_data(p);
}

void
GPSDClient::stop() 
{
  // gpsmm doesn't have a close method? OK ...
}

float
GPSDClient::getLat() const
{
    return m_latitude;
}

float
GPSDClient::getLong() const
{
    return m_longitude;
}

float
GPSDClient::getAlt() const
{
    return m_altitude;
}

float
GPSDClient::getTime() const
{
    return m_time;
}

void 
GPSDClient::process_data(struct gps_data_t* p)
{
  if (p == NULL)
    return;

  if (!p->online)
    {
      return;

      process_data_gps(p);
      process_data_navsat(p);
    }
}

void 
GPSDClient::process_data_gps(struct gps_data_t* p) 
{
  
}

void 
GPSDClient::process_data_navsat(struct gps_data_t* p)
{
  if (p->status == STATUS_FIX)
    {
      /* gpsd reports status=OK even when there is no current fix,
       * as long as there has been a fix previously. Throw out these
       * fake results, which have NaN variance
       */
      if (std::isnan(p->fix.epx) && m_check_fix_by_variance)
        {
	  return;
        }

      printf("Status: %d\n", p->status);
      /* Latitude, longitude and altitude are printed out as spherical values */
      printf("%f %f %f %f\n",p->fix.time, p->fix.latitude, p->fix.longitude, p->fix.altitude);
      m_time = p->fix.time;
      m_latitude = p->fix.latitude;
      m_longitude = p->fix.longitude;
      m_altitude = p->fix.altitude;
    }
}

