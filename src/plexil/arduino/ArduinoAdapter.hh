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
#include "BooleanVariable.hh"
#include "Node.hh"
#include "stricmp.h"
#include "pugixml.hpp"
#include "arduino_comunicacion.h"
#include "OgateServerClient.h"
#include "OgateCommon.h"

/** Define string to separate YAW and PITCH angles from position definition: Pyaw'char'pitch. */
#define POS_DELIMITER "_"
#define RPC_SUCCESS 0
namespace PLEXIL
{

  class ArduinoAdapter : public InterfaceAdapter
  {
    public:
	// Constructor/Destructor
	ArduinoAdapter(AdapterExecInterface& execInterface, const pugi::xml_node& xml);
	virtual ~ArduinoAdapter();

	// InterfaceAdapter API
	bool initialize();
	bool start();
	bool stop();
	bool reset();
	bool shutdown();
	void subscribe(const State& state);
	void unsubscribe(const State& state);

	void getPtuAngles(const LabelStr square, int *yaw, int *pitch, bool *success);
	// Executes a command with the given arguments
	void executeCommand(const LabelStr& name, const std::list<double>&args, ExpressionId dest, ExpressionId ack);
  
    private:
    	CLIENT *clnt;
	timeval tmout;
  };
}

extern "C"
{
  void initArduinoAdapter();
}
