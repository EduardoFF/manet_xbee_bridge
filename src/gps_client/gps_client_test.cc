#include "gps_client.h"


int main()
{
  GPSDClient client;
  if( !client.start())
    return -1;
  while(1)
    {
      client.step();
    }
  client.stop();
}
