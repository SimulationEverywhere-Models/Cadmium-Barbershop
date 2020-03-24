CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)

main_barber.o: top_model/barber.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/barber.cpp -o build/main_barber.o

main_checkhair_test.o: test/main_checkhair_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_checkhair_test.cpp -o build/main_checkhair_test.o

main_cuthair_test.o: test/main_cuthair_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_cuthair_test.cpp -o build/main_cuthair_test.o
	
main_reception_test.o: test/main_reception_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_reception_test.cpp -o build/main_reception_test.o

tests: main_checkhair_test.o main_cuthair_test.o main_reception_test.o
		$(CC) -g -o bin/CHECKHAIR_TEST build/main_checkhair_test.o
		$(CC) -g -o bin/CUTHAIR_TEST build/main_cuthair_test.o
		$(CC) -g -o bin/RECEPTION_TEST build/main_reception_test.o

#TARGET TO COMPILE ONLY BARBER SIMULATOR
simulator: main_barber.o
	$(CC) -g -o bin/BARBERSHOP build/main_barber.o
	
#TARGET TO COMPILE EVERYTHING (ABP SIMULATOR + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*