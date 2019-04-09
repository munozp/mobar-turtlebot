#include "ArduinoAdapter.hh"

// Register the ArduinoAdapter
extern "C"
{
  void initArduinoAdapter()
  {
    REGISTER_ADAPTER(PLEXIL::ArduinoAdapter, "ArduinoAdapter");
  }
}

namespace PLEXIL
{
  // Constructor
  ArduinoAdapter::ArduinoAdapter(AdapterExecInterface& execInterface, const pugi::xml_node& xml)
    : InterfaceAdapter(execInterface, xml)
  {
    assertTrue(!xml.empty(), "XML config file not found in ArduinoAdapter constructor");
    debugMsg("ArduinoAdapter", " Using " << xml.attribute("AdapterType").value());
  }

  // Destructor
  ArduinoAdapter::~ArduinoAdapter()
  {
    debugMsg("ArduinoAdapter", " Destructor called");
  }

  // Initialize
  bool ArduinoAdapter::initialize()
  {
    debugMsg("ArduinoAdapter::initialize", " called");
    m_execInterface.defaultRegisterAdapter(getId());
    debugMsg("ArduinoAdapter::initialize", " done");
    return true;
  }

  // Start method
  bool ArduinoAdapter::start()
  {
    tmout.tv_sec = 60;
    tmout.tv_usec = 0;
    debugMsg("ArduinoAdapter::start()", " called");
    return true;
  }

  // Stop method
  bool ArduinoAdapter::stop()
  {
    debugMsg("ArduinoAdapter::stop", " called");
    return true;
  }

  // Reset method
  bool ArduinoAdapter::reset()
  {
    debugMsg("ArduinoAdapter::reset", " called");
    return true;
  }

  // Shutdown method
  bool ArduinoAdapter::shutdown()
  {
    debugMsg("ArduinoAdapter::shutdown", " called");
    clnt_destroy (clnt);
    return true;
  }

  void ArduinoAdapter::subscribe(const State& /* state */)
  {
    debugMsg("ArduinoAdapter::subscribe", " called");
    debugMsg("ExternalInterface:udp", " subscribe called");
  }

  void ArduinoAdapter::unsubscribe(const State& /* state */)
  {
    debugMsg("ArduinoAdapter::unsubscribe", " called");
    debugMsg("ExternalInterface:udp", " unsubscribe called");
  }

/*
  Value ArduinoAdapter::lookupNow(const State& state )
  {
    debugMsg("ArduinoAdapter::lookupNow", " called");
    debugMsg("ExternalInterface:udp", " lookupNow called; returning UNKNOWN");
    return Expression::UNKNOWN();
  }
*/


void ArduinoAdapter::getPtuAngles(const LabelStr square, int *yaw, int *pitch, bool *success)
{
    // Position x,y,z
    char aux[16];
    char* temptok;
    strcpy(aux, square.c_str());
    aux[0] = ' '; // Delete first 'P'
    temptok=strtok(aux, (char*)POS_DELIMITER); // First token, YAW position
    if(temptok == NULL)
        *success = false; // Malformed square
    else
        *yaw = atoi(temptok);
    // Next token
    temptok=strtok(NULL, (char*)POS_DELIMITER); // Second token, PITCH position
    if(temptok == NULL)
        *success = false; // Malformed square
    else
        *pitch = atoi(temptok);
    *success = true;
}

  // Execute a Plexil Command
  void ArduinoAdapter::executeCommand(const LabelStr& name, const std::list<double>&args,  ExpressionId dest, ExpressionId ack)
  {
	//OGATE METRIC
	double executionTime = ogate.stopTimer(9)/1000;
	if(ogate.hasDataInterface())
		sendMetricToOgate(&ogate, tick, std::string(EXTIME), executionTime, true);
	ogate.startTimer(0);
	double goalProcessingTime, stateProcessingTime;

    debugMsg("ArduinoAdapter::executeCommand", " " << name.toString());
    std::list<double>::const_iterator largs = args.begin();
    std::string nStr = name.toString();
    bool retval = false;
    bool_t  *result_2;
    if(nStr == "initPtu")
    {
	// Run rpc client.
   	 LabelStr ip(*largs);
	char host [20];
	strcpy(host, ip.c_str());
   	 clnt = clnt_create (host, ARDUINO_COMUNICACION, ARDUINO_COMUNICACIONVERS, "tcp");
	if(ogate.hasDataInterface())
			sendTMstatusToOgate(&ogate, tick, false, std::string("PanTiltUnit.ptu"),std::string("PointingAt"), (char*)"Pant",90, (char*)"Tilt", 90, NULL);
   	 if (clnt == NULL) 
    	{
		debugMsg("ArduinoAdapter", "FAILED TO CONNECT ARDUINO RPC.");
		exit (1);
    	}
   	clnt_control(clnt, CLSET_TIMEOUT, (char*)&tmout);
    	debugMsg("ArduinoAdapter",nStr << ": ARDUINO RPC CLIENT SUCCESSFULL."<< "\n\r IP SERVER: "<<host);
    }
    if(nStr == "movePtu")
    {
	PTUPOS newpos;
	LabelStr orientation(*largs);
	bool correct;
	getPtuAngles(orientation, &newpos.yaw, &newpos.pitch, &correct);
	//newpos.yaw = static_cast<int> (*largs);
	//newpos.pitch = static_cast<int> (*(++largs));
	debugMsg("ArduinoAdapter", "MOVE PTU TO: "<< newpos.yaw <<","<< newpos.pitch << "º");
	if(ogate.hasDataInterface())
		sendTMstatusToOgate(&ogate, tick, false, std::string("PanTiltUnit.ptu"),  std::string("MovingTo"), (char*)"Pant", (int)newpos.yaw, (char*)"Tilt", (int)newpos.pitch, NULL);
	int result;
	if(ogate.hasDataInterface())
		sendMetricToOgate(&ogate, tick, std::string(GPTIME), goalProcessingTime, true);
	
	 result_2 = moveptu_1(&newpos, clnt);
	
	ogate.startTimer(0);
	if(ogate.hasDataInterface())
		sendTMstatusToOgate(&ogate, tick, false, std::string("PanTiltUnit.ptu"),  std::string("PointingAt"), (char*)"Pant", (int)newpos.yaw, (char*)"Tilt", (int)newpos.pitch, NULL);
	if (result_2 == (bool_t *) NULL) {
		clnt_perror (clnt, "call failed");
		debugMsg("ArduinoAdapter", "Command Failed");
	}
        char position_return[20];
        sprintf(position_return,"p%d_%d", newpos.yaw, newpos.pitch);
	PLEXIL::LabelStr valor_devuelto(position_return);
	m_execInterface.handleValueChange(dest,valor_devuelto);
	debugMsg("ArduinoAdapter", "ret "<<position_return);
	retval = true;
    }
    
    if(retval)
{
    	m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SUCCESS());
}
    else
        m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_FAILED());
    m_execInterface.notifyOfExternalEvent();


	stateProcessingTime = ogate.stopTimer(0)/1000;
	if(ogate.hasDataInterface())
		sendMetricToOgate(&ogate, tick, std::string(SPTIME), stateProcessingTime, true);
	//OGATE METRIC
	ogate.startTimer(9);

  }

}

