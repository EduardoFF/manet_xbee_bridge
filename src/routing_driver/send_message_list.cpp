// file: send_message.cpp
//
// LCM example program.
//
// compile with:
//  $ g++ -o send_message_list send_message_list.cpp -llcm
//
// On a system with pkg-config, you can also use:
//  $ g++ -o send_message_list send_message_list.cpp `pkg-config --cflags --libs lcm`

#include <lcm/lcm-cpp.hpp>
#include "exlcm/route2_tree_t.hpp"

int main(int argc, char ** argv)
{
    lcm::LCM lcm;
    if(!lcm.good())
        return 1;

    exlcm::route2_tree_t my_data;
    my_data.timestamp = 0;    
    my_data.n = 15; 
    my_data.rtable.resize(my_data.n);
    for(int i = 0; i < my_data.n; i++)
    {
        my_data.rtable[i].node = "test";
	my_data.rtable[i].n = 1;
	my_data.rtable[i].entries.resize(my_data.rtable[i].n);
    	for(int j = 0; j < my_data.rtable[i].n; j++)
	{
		my_data.rtable[i].entries[j].dest = "dest test";
		my_data.rtable[i].entries[j].node = "node test";
		my_data.rtable[i].entries[j].weight = 1;
	}
    }

    lcm.publish("EXAMPLE", &my_data);

    return 0;
}
