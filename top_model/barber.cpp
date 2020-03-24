//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/checkhair.hpp"
#include "../atomics/cuthair.hpp"
#include "../atomics/reception.hpp"

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/

struct inp_top : public cadmium::in_port<string>{};
struct inp_couple : public cadmium::in_port<string>{};
struct inp_newcust : public cadmium::in_port<string>{};

/***** Define output ports for coupled model *****/
struct out_top : public cadmium::in_port<string>{};
struct outp_out1 : public cadmium::out_port<string>{};
struct outp_out2 : public cadmium::out_port<string>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<string,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<string,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));
    /****** Cuthair atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> cuthair1 = dynamic::translate::make_dynamic_atomic_model<Cuthair, TIME>("cuthair1");
    /****** Checkhair atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> checkhair1 = dynamic::translate::make_dynamic_atomic_model<Checkhair, TIME>("checkhair1");
    /****** Reception atomic models instantiation *******************/
    shared_ptr<dynamic::modeling::model> reception1 = dynamic::translate::make_dynamic_atomic_model<Reception, TIME>("reception1");
    
	/*******BARBER COUPLED MODEL********/
    dynamic::modeling::Ports iports_Barber = {typeid(inp_couple)};
    dynamic::modeling::Ports oports_Barber = {typeid(outp_out1),typeid(outp_out2)};
    dynamic::modeling::Models submodels_Barber = {cuthair1, checkhair1};
    dynamic::modeling::EICs eics_Barber = {
		cadmium::dynamic::translate::make_EIC<inp_couple, Checkhair_defs::cust>("checkhair1")
	};
    dynamic::modeling::EOCs eocs_Barber = {
        dynamic::translate::make_EOC<Checkhair_defs::finished,outp_out1>("checkhair1"),
		dynamic::translate::make_EOC<Checkhair_defs::final_finished,outp_out2>("checkhair1")
    };
    dynamic::modeling::ICs ics_Barber = {
		dynamic::translate::make_IC<Checkhair_defs::cutcontinue, Cuthair_defs::in>("checkhair1","cuthair1"),
			dynamic::translate::make_IC<Cuthair_defs::out, Checkhair_defs::progress>("cuthair1","checkhair1")
	};
    shared_ptr<dynamic::modeling::coupled<TIME>> BARBER;
    BARBER = make_shared<dynamic::modeling::coupled<TIME>>(
        "BARBER", submodels_Barber, iports_Barber, oports_Barber, eics_Barber, eocs_Barber, ics_Barber 
    );
	  /*******BARBERSHOP SIMULATOR COUPLED MODEL********/
    dynamic::modeling::Ports iports_BARBERSHOP = {typeid(inp_newcust)};
    dynamic::modeling::Ports oports_BARBERSHOP = {typeid(outp_out2)};
    dynamic::modeling::Models submodels_BARBERSHOP = {reception1, BARBER};
    dynamic::modeling::EICs eics_BARBERSHOP = {
        cadmium::dynamic::translate::make_EIC<inp_newcust, Reception_defs::newcust>("reception1")
    };
    dynamic::modeling::EOCs eocs_BARBERSHOP = {
        dynamic::translate::make_EOC<outp_out2,outp_out2>("BARBER")
    };
    dynamic::modeling::ICs ics_BARBERSHOP = {
        dynamic::translate::make_IC<Reception_defs::cust, inp_couple>("reception1","BARBER"),
        dynamic::translate::make_IC<outp_out1, Reception_defs::next>("BARBER","reception1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> BARBERSHOP;
    BARBERSHOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "BARBERSHOP", submodels_BARBERSHOP, iports_BARBERSHOP, oports_BARBERSHOP, eics_BARBERSHOP, eocs_BARBERSHOP, ics_BARBERSHOP 
    );
	
	 /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(outp_out2)};
    dynamic::modeling::Models submodels_TOP = {input_reader, BARBERSHOP};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<outp_out2,outp_out2>("BARBERSHOP")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<string>::out, inp_newcust>("input_reader","BARBERSHOP")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/barber_1st_couple_model_message.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/barber_1st_couple_model_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until_passivate();
    return 0;
}