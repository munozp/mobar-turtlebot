/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Version modified by Pablo Muñoz, UAH June 2015.
 */

#include <unistd.h>
// sigh, Android only defines _POSIX_TIMERS as 1
#if defined(_POSIX_TIMERS) && ((_POSIX_TIMERS - 200112L) >= 0L || defined(PLEXIL_ANDROID))

#include "TickTimeAdapter.h"


// === REGISTRAR EL ADAPTADOR ===
extern "C" {
	void initTickTimeAdapter() {
		REGISTER_ADAPTER(PLEXIL::TickTimeAdapter, "TickTimeAdapter");
	}
}
// ==============================

namespace PLEXIL
{
  /**
   * @brief Constructor.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   */
  TickTimeAdapter::TickTimeAdapter(AdapterExecInterface& execInterface)
    : InterfaceAdapter(execInterface)
  {
    initSigevent();
  }

  /**
   * @brief Constructor from configuration XML.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   * @param xml A const reference to the XML element describing this adapter
   * @note The instance maintains a shared pointer to the XML.
   */
  TickTimeAdapter::TickTimeAdapter(AdapterExecInterface& execInterface, 
                                     const pugi::xml_node& xml)
    : InterfaceAdapter(execInterface, xml)
  {
    assertTrue(!xml.empty(), "XML config file not found in TickTimeAdapter constructor");
    debugMsg("TickTimeAdapter", " Using "<<xml.attribute("AdapterType").value());
    initSigevent();
  }

  /**
   * @brief Destructor.
   */
  TickTimeAdapter::~TickTimeAdapter()
  {
  }

  /**
   * @brief Initializes the adapter, possibly using its configuration data.
   * @return true if successful, false otherwise.
   */
  bool TickTimeAdapter::initialize()
  {
    debugMsg("TickTimeAdapter", " Initialize called...");
    // Command register
    m_execInterface.defaultRegisterAdapter(getId());
    // Automatically register self for time and tick
    m_execInterface.registerLookupInterface(LabelStr("time"), getId());
    m_execInterface.registerLookupInterface(LabelStr("tick"), getId());   
    debugMsg("TickTimeAdapter", " done");
    return true;
  }

  /**
   * @brief Starts the adapter, possibly using its configuration data.  
   * @return true if successful, false otherwise.
   */
  bool TickTimeAdapter::start()
  {
    debugMsg("TickTimeAdapter", "Started");
    // Create a timer
    int status = timer_create(CLOCK_REALTIME,
                              &m_sigevent,
                              &m_timer);
    condDebugMsg(0 != status,
		 "TickTimeAdapter:start",
		 " timer_create failed, errno = " << errno);
    threadSpawn(internalClockThread, (void*) this, ptClockThread);
    return (status == 0);
  }


  /**
   * @brief Stops the adapter.  
   * @return true if successful, false otherwise.
   */
  bool TickTimeAdapter::stop()
  {
    finish=true;
    // Disable the timer
    stopTimer();
    // Now delete it
    int status = timer_delete(m_timer);
    condDebugMsg(status != 0,
		 "TickTimeAdapter:stop",
		 " timer_delete failed, errno = " << errno);
    return (status == 0);
  }

  /**
   * @brief Resets the adapter.  
   * @return true if successful, false otherwise.
   */
  bool TickTimeAdapter::reset()
  {
    tick = 0;
    return true;
  }

  /**
   * @brief Shuts down the adapter, releasing any of its resources.
   * @return true if successful, false otherwise.
   */
  bool TickTimeAdapter::shutdown()
  {
    return true;
  }

  /**
   * @brief Perform an immediate lookup of the requested state.
   * @param state The state for this lookup.
   * @return The current value for this lookup.
   */
  double TickTimeAdapter::lookupNow(const State& state)
  {
    LabelStr sts(state.first);
    if(sts.toString() == "tick")
    {
       debugMsg("TickTimeAdapter:getCurrentTick", " returning " << tick);
       return (double)tick;
    }
    assertTrueMsg(state == m_execInterface.getStateCache()->getTimeState(),
                     "TickTimeAdapter only provides lookupNow for \"time\" and \"tick\"");
    return getCurrentTime();
  }

  /**
   * @brief Inform the interface that it should report changes in value of this state.
   * @param state The state.
   */

  void TickTimeAdapter::subscribe(const State& state)
  {
    assertTrueMsg(state == m_execInterface.getStateCache()->getTimeState(),
                  "TickTimeAdapter only suscribes lookups for \"time\"");
  }

  /**
   * @brief Inform the interface that a lookup should no longer receive updates.
   * @param state The state.
   */
  void TickTimeAdapter::unsubscribe(const State& state)
  {
    assertTrueMsg(state == m_execInterface.getStateCache()->getTimeState(),
                  "TickTimeAdapter only implements lookups for \"time\"");

    // Disable the timer
    stopTimer();
  }

  /**
   * @brief Advise the interface of the current thresholds to use when reporting this state.
   * @param state The state.
   * @param hi The upper threshold, at or above which to report changes.
   * @param lo The lower threshold, at or below which to report changes.
   */
  void TickTimeAdapter::setThresholds(const State& state, double hi, double lo)
  {
    /*LabelStr sts(state.first);
    std::cout<<"First: "<<sts.toString()<<std::endl;
    size_t tam(state.parameterCount();
    //size_t state2.parameterCount();
   //std::cout<<"Second: "<<sts2.toString()<<std::endl;
        /*LabelStr sts3(state.second);
    std::cout<<"Third: "<<sts3.toString()<<std::endl;*/
    
    assertTrueMsg(state == m_execInterface.getStateCache()->getTimeState(),
                  "TickTimeAdapter only implements lookups for \"time\"");

    // Get the current time
    timespec now;
    assertTrueMsg(0 == clock_gettime(CLOCK_REALTIME, &now),
		  "TickTimeAdapter::setThresholds: clock_gettime() failed, errno = " << errno);

    // Set up a timer to go off at the high time
    itimerspec tymrSpec = {{0, 0}, {0, 0}};
        //hi=now.tv_sec+15;
        std::cout<<"Now: "<<now.tv_sec<<" Hi: "<<hi<<" Low: "<<std::endl;
    tymrSpec.it_value = doubleToTimespec(hi) - now;
        std::cout<<"\n\n\n Tiempo que tiene el temporizador: "<<tymrSpec.it_value.tv_sec<<" s ,"<<tymrSpec.it_value.tv_nsec<<" s"<<std::endl;
    if (tymrSpec.it_value.tv_nsec < 0 || tymrSpec.it_value.tv_sec < 0) {
      // Already past the scheduled time, submit wakeup
      debugMsg("TickTimeAdapter:setThresholds",
	       " new value " << Expression::valueToString(hi) << " is in past, waking up Exec");
      timerTimeout();
      return;
    }

    tymrSpec.it_interval.tv_sec = tymrSpec.it_interval.tv_nsec = 0; // no repeats
    assertTrueMsg(0 == timer_settime(m_timer,
				     0, // flags: ~TIMER_ABSTIME
				     &tymrSpec,
				     NULL),
                  "TickTimeAdapter::setThresholds: timer_settime failed, errno = " << errno);
    debugMsg("TickTimeAdapter:setThresholds",
	     " timer set for " << Expression::valueToString(hi)
	     << ", tv_nsec = " << tymrSpec.it_value.tv_nsec);
  }

  //
  // Static member functions
  //

  /**
   * @brief Get the current time from the operating system.
   * @return A double representing the current time.
   */
  double TickTimeAdapter::getCurrentTime()
  {
    timespec ts;
    if (0 != clock_gettime(CLOCK_REALTIME, &ts)) {
      debugMsg("TickTimeAdapter:getCurrentTime",
	       " clock_gettime() failed, errno = " << errno << "; returning UNKNOWN");
      return Expression::UNKNOWN();
    }
    double tym = timespecToDouble(ts);
    debugMsg("TickTimeAdapter:getCurrentTime", " returning " << Expression::valueToString(tym));
    return tym;
  }

  //
  // Internal member functions
  //

  /**
   * @brief Helper for constructor methods.
   */
  void TickTimeAdapter::initSigevent()
  {
    // Pre-fill sigevent fields
    m_sigevent.sigev_notify = SIGEV_THREAD;          // invoke notify function
    m_sigevent.sigev_signo = SIGALRM;                // use alarm clock signal
    m_sigevent.sigev_value.sival_ptr = (void*) this; // parent TickTimeAdapter instance
    m_sigevent.sigev_notify_function = TickTimeAdapter::timerNotifyFunction;
    m_sigevent.sigev_notify_attributes = NULL;
  }

  /**
   * @brief Static member function invoked upon receiving a timer signal
   * @param this_as_sigval Pointer to the parent TickTimeAdapter instance as a sigval.
   */
  void TickTimeAdapter::timerNotifyFunction(sigval this_as_sigval)
  {
    // Simply invoke the timeout method
    TickTimeAdapter* adapter = (TickTimeAdapter*) this_as_sigval.sival_ptr;
    adapter->timerTimeout();
  }

  /**
   * @brief Report the current time to the Exec as an asynchronous lookup value.
   */
  void TickTimeAdapter::timerTimeout()
  {
    double time = getCurrentTime();
    debugMsg("TickTimeAdapter:lookupOnChange",
	     " timer timeout at " << Expression::valueToString(time));
    m_execInterface.handleValueChange(m_execInterface.getStateCache()->getTimeState(),
                                      time);
    m_execInterface.notifyOfExternalEvent();
  }

  /**
   * @brief Stop the timer.
   */
  void TickTimeAdapter::stopTimer()
  {
    static itimerspec sl_tymrDisable = {{0, 0}, {0, 0}};
    // tymrSpec.it_interval.tv_sec = tymrSpec.it_interval.tv_nsec = 
    //   tymrSpec.it_value.tv_sec = tymrSpec.it_value.tv_nsec = 0;
    condDebugMsg(0 != timer_settime(m_timer,
				    0, // flags: ~TIMER_ABSTIME
				    &sl_tymrDisable,
				    NULL),
		 "TickTimeAdapter:stopTimer",
		 " timer_settime failed, errno = " << errno);
  }
  
  void TickTimeAdapter::executeCommand(const LabelStr& name, const std::list<double>& args, ExpressionId dest, ExpressionId ack)
  {
    bool success = true;
    std::string nStr = name.toString();
    std::list<double>::const_iterator largs = args.begin();
    debugMsg("TickTimeAdapter", "Request to execute command "<< nStr);
    m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SENT_TO_SYSTEM());
    m_execInterface.notifyOfExternalEvent();
  
    // Only for initialize connection with Locomotion GenoM module and set square size.
    if(nStr == "connect_timer_to_ogate")
    {
        LabelStr compName(*largs);
        LabelStr ogateHost(*(++largs));
        int ogatePort = static_cast<int>(*(++largs));
        if(ogatePort > 0)
        {
            ogate.changeName(compName.toString());
            debugMsg("TickTimeAdapter", "Connecting to OGATE at "<<ogateHost.toString()<<":"<<ogatePort<<" as \""<<compName.toString()<<"\"");
            success = this->ogate.connect(ogateHost.toString(), ogatePort);
            if(success)
            {
                if(ogate.registerDataInterface("0"))
                {
                    debugMsg("TickTimeAdapter", "Registered Data Interface");
                    threadSpawn(InterfaceAdapter::internalClockThread, (void*) this, InterfaceAdapter::ptClockThread);
                }
                else
                {    
                    debugMsg("TickTimeAdapter", "Failed to register Data Interface");
                    success = false;
                }
            }
            else
                debugMsg("TickTimeAdapter", "ERROR: failed to connect to OGATE");
        }
        m_execInterface.handleValueChange(dest, success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
    }
    else if(nStr == "send_metric_to_ogate")
    {
        LabelStr metric(*largs);
        double value = static_cast<double>(*(++largs));
        if(ogate.hasDataInterface())
        {
            debugMsg("TickTimeAdapter", "Sending \""<<metric.toString()<<"\" metric to OGATE, value "<<value<<" at tick "<<tick);
            success = sendMetricToOgate(&ogate, tick, metric.toString(), value, true);
        }
        else
            debugMsg("TickTimeAdapter", "Failed to send metric: DATA interface not registered!");
        m_execInterface.handleValueChange(dest, success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
    }
    else if(nStr == "start_timer")
    {
        int ntimer = static_cast<int>(*largs);
        success = ogate.startTimer(ntimer);
        debugMsg("TickTimeAdapter","Timer "<<ntimer<<(success?" started":" failed to start"));
        m_execInterface.handleValueChange(dest, success? BooleanVariable::TRUE_VALUE():BooleanVariable::FALSE_VALUE());
    }
    else if(nStr == "stop_timer")
    {
        int ntimer = static_cast<int>(*largs);
        double msec = ogate.stopTimer(ntimer)/1000;
        success = msec > 0;
        debugMsg("TickTimeAdapter","Timer "<<ntimer<<" stoped. Time: "<<msec<<" milliseconds");
        m_execInterface.handleValueChange(dest, msec);
    }
    
    if(success)
        m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_SUCCESS());
    else
        m_execInterface.handleValueChange(ack, CommandHandleVariable::COMMAND_FAILED());
    m_execInterface.notifyOfExternalEvent();
  }

}

#endif // defined(_POSIX_TIMERS) && (_POSIX_TIMERS - 200112L) >= 0L

