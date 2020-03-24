/**
* Shyam Bhuptani
*
* Checkhair:
* Cadmium implementation of CD++ Checkhair atomic model
*/

#ifndef __CHECKHAIR_HPP__
#define __CHECKHAIR_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <stdlib.h>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
    struct Checkhair_defs{
        struct cutcontinue : public out_port<string> { };
		struct finished : public out_port<string> { };
		struct final_finished : public out_port<string> { };
        struct cust : public in_port<string> { };
        struct progress : public in_port<string> { };
    };
   
    template<typename TIME>
    class Checkhair{
        public:
			TIME preparationTime;
			TIME preparationTime_for_next;
            // default constructor
            Checkhair() noexcept{
				preparationTime  = TIME("00:00:07:00");
				preparationTime_for_next  = TIME("00:00:03:00");
				state.processed		= false;
				state.sending_for_cut = false;
				state.model_active     = false;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
				state.xl = 0;
            }
            
            // state definition
            struct state_type{
				bool model_active;
				bool processed;
				string message;
				bool sending_for_cut;
			  TIME next_internal;
			  int xl;
            }; 
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename Checkhair_defs::cust, typename Checkhair_defs::progress>;
            using output_ports=std::tuple<typename Checkhair_defs::cutcontinue, typename Checkhair_defs::finished,typename Checkhair_defs::final_finished>;

            // internal transition
            void internal_transition() {
            if (state.processed){
                state.processed = false;
                state.sending_for_cut = true;
                state.model_active = true; 
                state.next_internal = preparationTime_for_next;  
				state.xl =1;
                } else {
                    state.model_active = false;
                    state.next_internal = std::numeric_limits<TIME>::infinity();
					state.xl =2;
                }
             
                if (state.sending_for_cut){
                    state.sending_for_cut = false;
                    state.model_active = true;
                    state.next_internal = std::numeric_limits<TIME>::infinity();
					state.xl =3;					
                }
					else {
					state.sending_for_cut = true;
                    state.model_active = true;
                    state.next_internal = preparationTime;
					state.xl =4;	
					}
            
	}			

            // external transition
		void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
			for(const auto &x : get_messages<typename Checkhair_defs::cust>(mbs)){
         state.message = x;
		 if(state.message == "newcust"){
		 state.processed = false;
         state.sending_for_cut = true;
         state.model_active = true;  
		state.next_internal = preparationTime;	
		state.xl =5;
	  }
		}	
		for(const auto &x : get_messages<typename Checkhair_defs::progress>(mbs))
				if(state.model_active == true) 
				state.message = x;{ 
							if (state.message == "finished") {
								state.processed = true;
								state.sending_for_cut = false;
								state.next_internal = preparationTime_for_next;
								state.xl =9;
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
			string new1,new2;
			
			if (state.sending_for_cut){
                new1 = "newcust";
				get_messages<typename Checkhair_defs::cutcontinue>(bags).push_back(new1);
            }else{
                if (state.processed){
				new1 = "next";
				new2 = "finished";
				
                   get_messages<typename Checkhair_defs::finished>(bags).push_back(new1);
				   get_messages<typename Checkhair_defs::final_finished>(bags).push_back(new2);
                }
            }  

              return bags;
            }

            // time_advance function
            TIME time_advance() const {   
              return state.next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Checkhair<TIME>::state_type& i) {
                os << "Customer: " << i.message  ; 
            return os;
            }
        };     
  

#endif // __CHECKHAIR_HPP__