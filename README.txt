Barbershop demo was created under cygwin on a windows laptop.
The CYGWIN is installed as per the directions provided in the course material "Cadmium User Manual"

1) Open a cygwin prompt
2) Clone the using command i.e, git clone https://github.com/SimulationEverywhere/Cadmium-Simulation-Environment
3) Change the directory cd Cadmium-Simulation-Environment/DEVS-Models
4) unzip the folder there
5) run command i.e, make clean;make all 
6) change directory using ' cd bin/'

to execute coupled components:
7) execute file ./BARBERSHOP.exe ../input_data/input_barber_0_1st.txt

to execute atomic components:

8) ./RECEPTION.exe  
9) ./CHECKHAIR.exe  
10) ./CUTHAIR.exe 
   
Atomic components are present in atomic folder with names like checkhair.hpp, cuthair.cpp, reception.hpp.
Top model is present in top_model folder with name 

OUTPUTS

the .txt files have been created in simulation_results folder for all models which we have executed.