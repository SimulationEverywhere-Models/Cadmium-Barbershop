/**
* Shyam Bhuptani
*
* Cuthair:
* Cadmium implementation of CD++ Cuthair atomic model
*/

#ifndef _CUTHAIR_HPP__
#define _CUTHAIR_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Cuthair_defs{
    struct out : public out_port<string> {};
    struct in : public in_port<string> {};
};

template<typename TIME> class Cuthair{
    public:
		TIME   preparationTime;
		// default constructor
    Cuthair() {
		state.inp = "";
        state.model_active = false;
		preparationTime  = TIME("00:00:20:00");
    } 
    using input_ports=tuple<typename Cuthair_defs::in>;
    using output_ports=tuple<typename Cuthair_defs::out>;
    // state definition
    struct state_type{
		string inp;
        bool model_active;
		TIME next_internal;
    }; 
    state_type state;    
        
    // internal transition
    void internal_transition() {
                state.model_active = false; 
            }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
        vector<string> bag_port_in;
        bag_port_in = get_messages<typename Cuthair_defs::in>(mbs);
		
        if(bag_port_in.size()>1) assert(false && "One message at a time");                
                      
		state.inp = bag_port_in[0]; 
		state.model_active = true;  		
                                      
    }
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }
    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
			string new1 = "finished";
		 get_messages<typename Cuthair_defs::out>(bags).push_back(new1);
            return bags;
    }
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
		if (state.model_active) {            
            next_internal = preparationTime;
        }else {
            next_internal = numeric_limits<TIME>::infinity();
        }  	
        return next_internal;
    }

    friend ostringstream& operator<<(ostringstream& os, const typename Cuthair<TIME>::state_type& i) {
        os << "Hair Cut done of " << i.inp ; 
        return os;
    }
};    
#endif // _CUTHAIR_HPP__