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
  os << "type: ";
  if( hdr.type == XBEEDATA_ROUTING )
    os << "ROUTING";
  else if( hdr.type == XBEEDATA_ENDNODEINFO)
    os << "ENDNODEINFO";
  else if( hdr.type == XBEEDATA_FLOWINFO )
    os << "FLOWINFO";
  else if( hdr.type == XBEEDATA_PLANNING )
    os << "PLANNING"; 
  os << " src " << (int) hdr.src;
  return os;
}
