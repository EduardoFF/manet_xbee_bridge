#ifndef _ROUTING_DRIVER_H_
#define _ROUTING_DRIVER_H_
// -*- mode: c++; c-basic-offset: 4 -*-
/*
 * gps_driver.{cc,hh} -- interface to GPS data
 *
 * Author:		        Eduardo Feo Flushing
       				Dalle Molle Institute for Artificial Intelligence
				IDSIA - Manno - Lugano - Switzerland
				(eduardo <at> idsia.ch)

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <lcm/lcm-cpp.hpp>
#include <pthread.h>
#include "pose_t.hpp"
using namespace std;


struct TimestampedROUTINGData
{
  uint64_t timestamp;
  float lat;
  float lon;
  float alt;
TimestampedROUTINGData(uint64_t t, float _lat, float _lon, float _alt):
  timestamp(t),
    lat(_lat),
    lon(_lon),
    alt(_alt)
  {}
TimestampedROUTINGData():
  timestamp(0),lon(0),lat(0),alt(0)
  {}
  
};


class ROUTINGDriver 
{
  public:
    ROUTINGDriver();
    ROUTINGDriver(const char * url, const string &channel, bool autorun);

    bool run();

    void handleMessage(const lcm::ReceiveBuffer* rbuf, 
		       const std::string& chan, 
		       const route2_tree_t* msg);

    TimestampedROUTINGData data();
  private:
    static uint64_t getTime();
    static std::string getTimeStr();
    TimestampedROUTINGData m_latestData;


    const char * m_lcmURL;

    string m_lcmChannel;

    lcm::LCM m_lcm;

    /**
     * Mutex to control the access to member variables
     */
    pthread_mutex_t m_mutex;
    /**
     * Thread
     */
    pthread_t m_thread;
    inline bool isLCMReady();
    inline void subscribeToChannel(const string & channel) ;


    static void * internalThreadEntryFunc(void * ptr) 
    {
      (( GPSDriver *) ptr)->internalThreadEntry();
      return NULL;
    }
    void internalThreadEntry();
};

 #endif 