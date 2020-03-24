/**
* Shyam Bhuptani
*
* Reception:
* Cadmium implementation of CD++ Reception atomic model
*/

#ifndef __RECEPTION_HPP__
#define __RECEPTION_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <list>

using namespace cadmium;
using namespace std;

//Port definition
struct Reception_defs{
    struct newcust : public in_port<string> { };
	struct next : public in_port<string> { };
	struct cust : public out_port<string> { };
};

template<typename TIME> class Reception{
    public:
        //Parameters to be overwriten when instantiating the atomic model
        TIME   preparationTime;
	unsigned int numberofChairs;
        // default constructor
        Reception() noexcept{
          preparationTime  = TIME("00:00:05:00");
	  numberofChairs   = 8;
          state.we_are_full = false;
	  state.cust_is_ready = false;
	  state.elements = 0 ;
          state.model_active     = false;
        }
        
        // state definition
        struct state_type{
			bool we_are_full ;
			bool cust_is_ready ;
			int elements ;
			
			bool model_active;
        }; 
        state_type state;
        // ports definition
        using input_ports=std::tuple<typename Reception_defs::newcust, typename Reception_defs::next>;
        using output_ports=std::tuple<typename Reception_defs::cust>;

        // internal transition
        void internal_transition() {
				state.model_active = false;
            } 

        // external transition
               void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            
            for(const auto &x : get_messages<typename Reception_defs::newcust>(mbs)){
				if(x == "newcust"){
                     if (!(state.elements < numberofChairs)) {
					state.we_are_full = true;
					}else{ 
            state.elements ++;
            state.model_active = true;
      }
   }
			}
   for(const auto &x : get_messages<typename Reception_defs::next>(mbs)){
            if(x == "next"){
            state.elements --;
			if(state.elements){
            state.model_active = false;}
           
   }
			   }
			   }


        // confluence transition
        void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }

        // output function
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bags;
			string new1;
            if (state.elements != 0){
                new1 = "newcust";
                get_messages<typename Reception_defs::cust>(bags).push_back(new1);
            }   
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

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Reception<TIME>::state_type& i) {
            os << "total Customers: " << i.elements ;
        return os;
        }
};     
#endif // __RECEPTION_HPP__
