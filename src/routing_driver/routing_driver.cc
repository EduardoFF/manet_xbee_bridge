#include "routing_driver.h"
#include <sys/time.h>


#define IT(c) __typeof((c).begin())
#define FOREACH(i,c) for(__typeof((c).begin()) i=(c).begin();i!=(c).end();++i)

ROUTINGDriver::ROUTINGDriver()
{
}

ROUTINGDriver::ROUTINGDriver(const char * url, 
                             const string &channel,
                             bool autorun)
{
    m_lcm = lcm_create(url); /// Create a new LCM instance
    m_lcmURL = url; /// Set LCM URL
    m_lcmChannel = channel; /// Set LCM channel
    if (isLCMReady())/// FIXME: better outside?
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

/// lock, copy, unlock, return
TimestampedROUTINGData
ROUTINGDriver::data()
{
    pthread_mutex_lock(&m_mutex);
    TimestampedROUTINGData ret( m_latestRoutingData );
    pthread_mutex_unlock(&m_mutex);
    return ret;
}

/* Converts LCM data to TimestampedROUTINGData */
void
ROUTINGDriver::handleMessage(const lcm::ReceiveBuffer* rbuf,
                             const std::string& chan,
                             const route2_tree_t* msg)
{
    uint64_t tt = getTime();
    TimestampedROUTINGData routingData;

    printf("ROUTINGDriver: [%lld] got config \n", (long long)tt);
    printf("  Received message on channel \"%s\":\n", chan.c_str());
    printf("  timestamp   = %lld\n", (long long)msg->timestamp);

    routingData.timestamp = msg->timestamp;
    for(int i = 0; i < msg->n; i++)
    { 
        RoutingTable table;
        string nodeID = msg->rtable[i].node;
        int nbEntry = msg->rtable[i].n;

        for(int j = 0; j < nbEntry; j++)
        {
            Entry entry;
            entry.endNode = msg->rtable[i].entries[j].dest;
            entry.nextHop = msg->rtable[i].entries[j].node;
            entry.weight = msg->rtable[i].entries[j].weight;
            table.push_back(entry);
        }
        routingData.route[nodeID] = table;
    }
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

std::string
ROUTINGDriver::getTimeStr()
{
    char buffer [80];
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
    char currentTime[84] = "";
    sprintf(currentTime, "%s:%d", buffer, milli);
    std::string ctime_str(currentTime);
    return ctime_str;
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

inline void 
ROUTINGDriver::subscribeToChannel(const string & channel)
{
    printf("Listening to channel %s\n", channel.c_str());
    m_lcm.subscribe(channel, &ROUTINGDriver::handleMessage, this);
}

void
ROUTINGDriver::internalThreadEntry()
{
    while (true)
    {
        m_lcm.handle();
    }
}



