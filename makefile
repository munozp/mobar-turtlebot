CC=g++
DEV=-g -Wall
OPT=-O2
CXXFLAGS=-I include
LIBSO=-fPIC -shared
#DIRECTORIO RAIZ DE MOBAR
export MOBAR_HOME=$(PWD)
#RUTAS A FUENTES
export MSRC=$(MOBAR_HOME)/src
export MROS=$(MSRC)/ros/src/nodo_comunicacion/src
export ARDU=arduino
export PDDL=pddl
export PLEX=$(MSRC)/plexil
export PROS=$(PLEX)/ros
export UPTA=$(MSRC)/upta
OGAT=$(MSRC)/OgateInterf
#RUTAS A GENERADOS
export MBIN=bin
export MLIB=lib
#PLANES DE PLEXIL
PLEXIL_PLANS := Planner 

all: ogate TimeAdapter ARDUINO_Adapter ROS_Adapter PDDL_Adapter CompilePlexilPlans TimeAdapter up2ta

TimeAdapter: ogate
	cd $(PLEX); make;
	
up2ta:
	cd $(UPTA); $(CC) $(CXXFLAGS) $(OPT) main.cpp Map.cpp GSIsearch.cpp Heuristica.cpp CProblema.cpp CPlan.cpp -o $(MOBAR_HOME)/$(MBIN)/planner/up2ta

TimeAdapter: ogate
	cd $(PLEX); make;

ogate:
	cd $(OGAT); $(CC) $(LIBSO) Socket.cpp ClientSelect.cpp OgateServerClient.cpp -I. -o ../../lib/libOgate.so
	sudo rm -rf /usr/local/lib/libOgate.so
	sudo ln -s $(PWD)/lib/libOgate.so /usr/local/lib/libOgate.so

ARDUINO_server_rpcgen:
	cd $(MSRC)/$(ARDU); rpcgen arduino_comunicacion.x;
	cp $(MSRC)/$(ARDU)/arduino_comunicacion_clnt.c $(MSRC)/$(ARDU)/arduino_comunicacion_xdr.c $(PLEX)/$(ARDU)/;

ARDUINO_server:
	cd $(MSRC)/$(ARDU); make -f Makefile.arduino_comunicacion ; cp arduino_comunicacion_server $(MOBAR_HOME)/$(MBIN)/arduino_rpc_server ;

ARDUINO_Adapter: ARDUINO_server
	cd $(PLEX)/$(ARDU); make

ROS_server_rpcgen:
	#cd $(MROS)/;
	#rm $(MROS)/nodo_comunicacion_clnt.c $(MROS)/nodo_comunicacion_client.c $(MROS)/nodo_comunicacion_xdr.c $(MROS)/nodo_comunicacion.h $(MROS)/nodo_comunicacion_svc.c $(MROS)/nodo_comunicacion_server.c
	#cd $(MROS)/; 
	#rpcgen -a ./nodo_comunicacion.x;
	rm $(MSRC)/ros/src/nodo_comunicacion/include/nodo_comunicacion/nodo_comunicacion.h;
	rm $(PROS)/nodo_comunicacion_clnt.c $(PROS)/nodo_comunicacion_xdr.c;
	cp $(MROS)/nodo_comunicacion_clnt.c $(MROS)/nodo_comunicacion_xdr.c $(PROS)/;
	cp $(MROS)/nodo_comunicacion.h $(MSRC)/ros/src/nodo_comunicacion/include/nodo_comunicacion/;

ROS_server:
	cd $(MSRC)/ros; catkin_make; 

ROS_Adapter: ROS_server
	cd $(PROS); make;

PDDL_Adapter:
	cd $(PLEX)/$(PDDL); make;

StandalonePDDLlib:
	$(CC) -O2 $(LIBSO) $(MSRC)/$(PDDL)/*.cpp -o $(MLIB)/libPDDL.so

CompilePlexilPlans: $(PLEXIL_PLANS)

$(PLEXIL_PLANS):
	cd $(MBIN)/plexil/; plexilc $@.ple
	cd $(MBIN)/plexil/; plexilc dock.ple
	cd $(MBIN)/plexil/; plexilc undock.ple
	cd $(MBIN)/plexil/; plexilc timer.ple
	
installPlexilMod:
	sudo cp $(PLEX)/plexil-mod/InterfaceAdapter.hh $(PLEXIL_HOME)/src/app-framework
	sudo cp $(PLEX)/plexil-mod/InterfaceAdapter.cc $(PLEXIL_HOME)/src/app-framework
	sudo cp $(PLEX)/plexil-mod/Makefile $(PLEXIL_HOME)/src/app-framework
	export MOBAR_HOME=$(PWD)
	cd $(PLEXIL_HOME); make


clean: deltemp
	find -name "*.so" -exec rm {} \;
	find -name "*.o" -exec rm {} \;

deltemp:
	find -name "*~" -exec rm {} \;
