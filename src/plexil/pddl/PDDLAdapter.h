#include <cstdio>
#include <vector>
#include <list>
#include <string>
#include "InterfaceAdapter.hh"
#include "LabelStr.hh"
#include "AdapterFactory.hh"
#include "CoreExpressions.hh"
#include "AdapterExecInterface.hh"
#include "Expression.hh"
#include "Debug.hh"
#include "BooleanVariable.hh"
#include "pugixml.hpp"
#include "ThreadSpawn.hh"
#include "Planner.h"
#include "OgateServerClient.h"
#include "OgateCommon.h"

#define PLX_SPACE '~'
#define REPLAN "REPLAN"
/** Define string to separate X and Y position from square definition: CXX'char'YY. */
#define POS_DELIMITER "_"
#define DEBUG 1

namespace PLEXIL
{
    class PDDLAdapter: public InterfaceAdapter
    {
        public:
            PDDLAdapter(AdapterExecInterface& execInterface, const pugi::xml_node& xml);
        
            virtual ~PDDLAdapter();

            /**
             * @brief Initializes the adapter, possibly using its configuration data.
             * @return true if successful, false otherwise.
             */
            bool initialize();
            /**
             * @brief Starts the adapter, possibly using its configuration data.
             * @return true if successful, false otherwise.
             */
            bool start();
            /**
             * @brief Stops the adapter.
             * @return true if successful, false otherwise.
             */
            bool stop();

            /**
             * @brief Resets the adapter.
             * @return true if successful, false otherwise.
             */
            bool reset();
            /**
             * @brief Shuts down the adapter, releasing any of its resources.
             * @return true if successful, false otherwise.
             */
            bool shutdown();

            void suscribe(const State& state);

            void unsuscribe(const State& state);

            void replacePlexilSpace(char* str);

            void getXYposition(const LabelStr square, int *px, int *py,bool *success);

            void executeCommand(const LabelStr& name, const std::list<double>& args, ExpressionId dest, ExpressionId ack);

            bool add_obstacle(char* ruta_mapa, int angleRobot, int objx,int objy, int bumper);

        private:
            static void* ogateClientThread(void* this_as_void_ptr);
            // OGATE client thread
            pthread_t ogClientThread;
            OgateServerClient ogatePlanner;
            std::string m_Name;
            std::vector<Planner*> planificador;
            std::vector<stpredicado*> ultimaacc;
            double goalProcessingTime;
            double stateProcessingTime;
            double operationalTime;
            int running;
            bool tcreplan;
    };
    
// PLEXIL namespace
}

extern "C"
{
 void initPDDLAdapter();
}

