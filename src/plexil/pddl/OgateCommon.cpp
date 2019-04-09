/* 
 * File:   OgateCommon.cpp
 * Author: Pablo Muñoz
 *
 * Created on 2 de June de 2015
 */

#include "OgateCommon.h"

bool sendMetricToOgate(OgateServerClient* ogate, int tick, std::string metric, double value, bool sendZeroMetric)
{
    std::stringstream metricdata;
    std::stringstream timestamp;
    if((tick-1) >=0 && sendZeroMetric)
    {
        timestamp << (tick-1);
        metricdata << metric << SC << "0";
        ogate->send(DATA, timestamp.str(), metricdata.str());
        metricdata.str("");
        timestamp.str("");
    }
    timestamp << tick;
    metricdata << metric << SC << value;
    int ret = ogate->send(DATA, timestamp.str(), metricdata.str());
    if(ret > 0 && sendZeroMetric)
    {
        metricdata.str("");
        timestamp.str("");
        timestamp << (tick+1);
        metricdata << metric << SC << "0";
        ogate->send(DATA, timestamp.str(), metricdata.str());
    }
    return ret > 0;
}

bool sendStatusToOgate(OgateServerClient* ogate, int tick, int state)
{
    std::stringstream status;
    std::stringstream timestamp;
    timestamp << tick;
    status << state;
    int ret = ogate->send(CONTROL, timestamp.str(), status.str());
    return ret > 0;
}

bool sendTMstatusToOgate(OgateServerClient* ogate, int tick, bool goal, std::string tm, std::string pred, ...)
{
   std::stringstream xmldata;
   std::stringstream timestamp;
   timestamp << tick;
   std::string type("Observation");
   if(goal)
       type = std::string("Goal");

   xmldata << "<" << type << " on=\"" << tm << "\" pred=\"" << pred << "\">";
   va_list p; 
   va_start(p, pred); 
   char *vname;
   int vvalue;
   while((vname = va_arg(p, char*)))
   {
      vvalue = va_arg(p, int);
      xmldata << "<Variable name=\""<<vname<<"\"><int min=\""<<vvalue<<"\" max=\""<<vvalue<<"\"/></Variable>";
   }
   va_end(p);
   xmldata << "</" << type << ">";

   int ret = ogate->send(DATA, timestamp.str(), xmldata.str());
   return ret > 0;
}

