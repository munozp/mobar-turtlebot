#include <cstdio>
#include <string>
#include <cmath>
#include <sys/time.h>
#include "InterfaceAdapter.hh"
#include "LabelStr.hh"
#include "AdapterFactory.hh"
#include "CoreExpressions.hh"
#include "AdapterExecInterface.hh"
#include "Expression.hh"
#include "Debug.hh"
#include "nodo_comunicacion.h"
#include "BooleanVariable.hh"
#include "Node.hh"
#include "stricmp.h"
#include "pugixml.hpp"
#include "OgateServerClient.h"
#include "OgateCommon.h"
#include "Map.h"
#include <sstream>
#include <unistd.h>

/** Define string to separate X and Y position from square definition: CXX'char'YY. */
#define POS_DELIMITER "_"
#define DEBUG 1

namespace PLEXIL
{

  class TurtlecomAdapter : public InterfaceAdapter
  {
    public:
        // Constructor/Destructor
        TurtlecomAdapter(AdapterExecInterface& execInterface, const pugi::xml_node& xml);
        virtual ~TurtlecomAdapter();

        // InterfaceAdapter API
        bool initialize();
        bool start();
        bool stop();
        bool reset();
        bool shutdown();
        void subscribe(const State& state);
        void unsubscribe(const State& state);

        void getXYposition(const LabelStr square, int *px, int *py, int offsetx, int offsety, bool *success);
        // Executes a command with the given arguments
        void executeCommand(const LabelStr& name, const std::list<double>&args, ExpressionId dest, ExpressionId ack);
        
        float moveBackxmeters(const LabelStr angle, const LabelStr square);
  
    private:
        CLIENT *clnt;
        CLIENT *clnt2;
        /** X e Y pos en cm, angulo en grados. */
        int posx;
        int posy;
        int head;
	    int offsetx;
	    int offsety;
        /** Tamaño de la celda en cm.*/
        int squareSize;
	    /*Distancia acumulada*/
	    double cumulativeDistance;
        /** Tiempo de espera del servidor RPC*/
        timeval tmout;
        timeval tmout2; 
  };
}

extern "C"
{
  void initTurtlecomAdapter();
}

