#ifndef XBEE_APP_DATA_H
#define XBEE_APP_DATA_H
/*
    xbee_app_data.{cc,h} - application data sent through XBee


    Author:		        Eduardo Feo Flushing
                    Dalle Molle Institute for Artificial Intelligence
                IDSIA - Manno - Lugano - Switzerland
                (eduardo <at> idsia.ch)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.	If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <unistd.h>
#include "route2_table_t.hpp"

//#include "tree.h"
namespace xbee_app_data
{
typedef struct Packet Packet;

static const char XBEEDATA_ENDNODEINFO = 1;
static const char XBEEDATA_ROUTING = 2;
static const char XBEEDATA_PLAN = 3;

struct __attribute__((__packed__)) EndNodeInfo
{
    float latitude;
    float longitude;
    float altitude;
    int dataRate;
};

struct __attribute__((__packed__)) Routing
{
    int tabId;
    int fragNb;
    int nbOfFrag;
    int nbBytes;

};

struct __attribute__((__packed__)) Plan
{
    int plan;
};

struct __attribute__((__packed__)) Header
{
    char type;
};
};

std::ostream &operator<<(std::ostream &os, const xbee_app_data::EndNodeInfo &);
std::ostream &operator<<(std::ostream &os, const xbee_app_data::Routing &);
std::ostream &operator<<(std::ostream &os, const xbee_app_data::Plan &);
std::ostream &operator<<(std::ostream &os, const xbee_app_data::Header &);

#endif
