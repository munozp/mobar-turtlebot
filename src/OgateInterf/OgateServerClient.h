/** 
 * @file "OgateServerClient.h"
 * @brief Definitions for the OGATE interface
 *
 * Defines the interfacing with the OGATE server for control/data external components.
 * This class uses the ClientSelect implementation for the client socket. The message size is defined in Socket.h.
 *
 * @author Pablo Muñoz
 * @version 1.0
 */

#ifndef H_ogateInterface
#define H_ogateInterface

#include <iostream>
#include <cstring>
#include <sys/time.h>
#include "ClientSelect.h"

    // Return values for the component status from Control Interface.
    /** Unrecoverable failure. */
    #define COMPONENT_FAILURE -1
    /** Component stopped, ready to be executed. */
    #define COMPONENT_STOPPED  0
    /** Component is executing. */
    #define COMPONENT_RUNNING  1
    /** Component is paused. */
    #define COMPONENT_PAUSED   2
    /** Component finishes its execution and cannot be restarted. */
    #define COMPONENT_FINISHED 3
    /** Component is in a recoverable failure. */
    #define COMPONENT_FAILING  4
    /** Request is not implemented in the component. */
    #define COMPONENT_NOIMPOP  5

    // Interface commands types
    /** Register the component in the OGATE server. */
    #define REGISTER 1
    /** Interface control command. */
    #define CONTROL  2
    /** Interface data command. */
    #define DATA     3

    // Interfacing tags.
    /** Separator char. */
    #define SC '|'
    /** Register tag. */
    #define REGISTER_TAG "REGISTER"
    /** Control message tag. */
    #define CONTROL_TAG  "CONTROL"
    /** Data message tag. */
    #define DATA_TAG     "DATA"
    // Operations for CONTROL messages (service requested by OGATE).
    #define CONTROL_IF_STATUS "ogateStatus"
    #define CONTROL_IF_START  "ogateStart"
    #define CONTROL_IF_FINISH "ogateFinish"
    #define CONTROL_IF_PAUSE  "ogatePause"
    #define CONTROL_IF_STEP   "ogateStep"
    #define CONTROL_IF_RESUME "ogateResume"
    /** Begin of an OGATE control request. */
    const std::string CONTROL_REQUEST = "ogate";
    
    // Values for configuration files
    /** IP or host name for the OGATE server. */
    #define ATTR_OGATE_SERVER "ogateServer"
    /** Port used by the OGATE server. */
    #define ATTR_OGATE_PORT   "ogatePort"

    /** Number of attempts to connect or register an interface. */
    #define ATTEMPTS 90
    /** Delay in msec between attempts. */
    #define DELAY 20
    /** Number of timers created by default. */
    #define NUM_TIMERS 9

    class OgateServerClient {
        public:
            
            /**
             * Creates a non-named interface with the OGATE server.
             * @param ntims number of timers to create
             */
            OgateServerClient(int ntims = NUM_TIMERS);

            /**
             * Creates interface with the OGATE server using the given name.
             * @param comp a no empty string with the name of the component
             * @param ntims number of timers to create
             */
            OgateServerClient(std::string comp, int ntims = NUM_TIMERS);

            /**
             * Closes the connection with the OGATE server.
             */
            ~OgateServerClient();

            /**
             * Establishes the connection with the OGATE server at the specified IP and PORT number.
             * @ip ip or host in which is located the OGATE server
             * @port port number
             * @return return true if the connection is established and the client id is obtained. False otherwise.
             */
            bool connect(std::string ip, int port);

            /**
             * Closes the connection with the OGATE server.
             */
            void closeConnection();

            /**
             * @return Return true if it is possible to communicate with the OGATE server.
             */
            bool isConnected();

            /**
             * @return Return true if the component is connected and the DATA interface is correctly registered in the OGATE server. False otherwise.
             */
            bool hasDataInterface();

            /**
             * @return Return true if the component is connected and the CONTROL interface is correctly registered in the OGATE server. False otherwise.
             */
            bool hasControlInterface();

            /**
             * Attempt to register the CONTROL interface for the component.
             * @return Return true if the component is connected and the DATA interface is correctly registered in the OGATE server. False otherwise.
             */
            bool registerControlInterface(std::string timestamp);

            /**
             * Attempt to register the DATA interface for the component.
             * @return Return true if the component is connected and the DATA interface is correctly registered in the OGATE server. False otherwise.
             */
            bool registerDataInterface(std::string timestamp);

            /**
             * Change the component name.
             * @return Return true if the name is not empty, false otherwise.
             */
            bool changeName(std::string comp);

            /**
             * Send a message to the OGATE server. This automatically checks the required data, creates a correct message and sends it.
             * @type message type (REGISTER, CONTROL or DATA)
             * @timestamp the time in which the message is send
             * @data the data of the message
             * @return The number of bytes send to the OGATE server, or a failure status:
             * -1 not connected; -2 CONTROL request on non-registered comp; 
             * -3 DATA request on non-registered comp -4 not allowed null parameter; 
             * -5 invalid message type; -6 invalid REGISTER tag; -7 invalid CONTROL operation
             */
            int send(int type, std::string timestamp, std::string data);

            /**
             * Receive data from the OGATE server. It is a non-blocking operation.
             * @type the type of request received: DATA or CONTROL
             * @data a string to store the data received, without any separator character or other information
             * @return the size of the data received, 0 when nothing to read, -1 if not connected or no interfaces registered, -2 for a malformed message (no separator char) -3 for unknown message type.
             */
            int receive(int* type, std::string* data);

            /**
             * @return Return the component id given by the OGATE server.
             */
            int clientId();

            /**
             * Start a timer.
             * @param timer number of timer to initialize
             * @return True if the timer number is valid, false otherwise.
             */
            bool startTimer(int timer);
            /**
             * Finish a timer and return the time passed between startTimer and stopTimer.
             * @param timer number of timer to stop
             * @return A positive value in microsec between startTimer and stopTimer or -1 for invalid timer number.
             */
            double stopTimer(int timer);

        private:
            /**
             * Initialize internal data of the interface.
             */
            void initialize(std::string comp, int ntims);

        private:
            /** Socket connection with the OGATE server. */
            ClientSelect ogclient;
            /** Client id provided by OGATE. */
            int clientid;
            /** Name of the component. */
            std::string compname;
            /** True if the component has registered the CONTROL interface. */
            bool controlif;
            /** True if the component has registered the DATA interface. */
            bool dataif;
            /** Timers for metrics. */
            double* timers;
            /** Number of timers. */
            int numtimers;
    };

#endif
