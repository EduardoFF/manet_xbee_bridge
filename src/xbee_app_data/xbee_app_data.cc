#include "xbee_app_data.h"

using namespace xbee_app_data;
using namespace std;

ostream &operator<<(ostream &os, const EndNodeInfo &info)
{
  os << " lat: " << info.latitude
     << " lon: " << info.longitude
     << " altitude " << info.altitude
     << " datarate " << info.dataRate;
  return os;
}

ostream &operator<<(ostream &os, const Header &hdr)
{
  os << "type " << hdr.type;
  os << " src " << hdr.src;
  return os;
}
