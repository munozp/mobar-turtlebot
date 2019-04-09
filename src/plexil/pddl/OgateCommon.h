/* 
 * File:   OgateCommon.h
 * Author: Pablo Muñoz
 *
 * Created on 2 de June de 2015
 */
 
#ifndef OGATECOMMON_H
#define	OGATECOMMON_H

#include <sstream>
#include <string>
#include <cstdarg>
#include "OgateServerClient.h"

#define EXTIME "operationalTime"
#define DBTIME "deliberationTime"
#define GPTIME "goalProcessingTime"
#define SPTIME "stateProcessingTime"


bool sendMetricToOgate(OgateServerClient* ogate, int tick, std::string metric, double value, bool sendZeroMetric);

bool sendStatusToOgate(OgateServerClient* ogate, int tick, int state);

bool sendTMstatusToOgate(OgateServerClient* ogate, int tick, bool goal, std::string tm, std::string pred, ...);

#endif	/* OGATECOMMON_H */
