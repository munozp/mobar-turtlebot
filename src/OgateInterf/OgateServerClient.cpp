/** 
 * @file "OgateServerClient.cpp"
 * @brief Implementation for the OGATE interface
 *
 * Implements main functionality for interfacing with the OGATE server for control/data external components.
 * @note The size of the message is defined in socket.h
 *
 * @author Pablo Muñoz
 * @version 1.0
 */

#include "OgateServerClient.h"

OgateServerClient::OgateServerClient(int ntims)
{
    initialize(std::string(), ntims);
}

OgateServerClient::OgateServerClient(std::string comp, int ntims)
{
    initialize(comp, ntims);
}

void OgateServerClient::initialize(std::string comp, int ntims)
{
    dataif = false;
    controlif = false;
    clientid = -1;
    changeName(comp);
    if(ntims > 0)
        numtimers = ntims;
    else
        numtimers = NUM_TIMERS;
    timers = new double [numtimers];
}

bool OgateServerClient::changeName(std::string comp)
{
    if(comp.empty())
    {
        char aux[100];
        sprintf(aux, "OgateClient_%d", clientid);
        compname = aux;
        return false;
    }
    compname = comp;
    return true;
}

OgateServerClient::~OgateServerClient()
{
    delete[] timers;
    initialize(std::string(), 0);
    closeConnection();
}

void OgateServerClient::closeConnection()
{
    if(ogclient.isConnected())
        close(ogclient.getSocket());
}

bool OgateServerClient::connect(std::string ip, int port)
{
    if(!ogclient.isConnected())
        if(ogclient.open((char*)ip.c_str(), port))
        {
            // Read client id
            char cli[M_LENGTH];
            int j=0; // Try several times to get the id from java
            while(clientid < 0)
            {
                int ret = ogclient.receive(cli);
                if(ret > 0)
                    clientid = atoi(cli);
                j++;
                usleep(DELAY); // Required delay
            }
        }
    if(clientid >= 0)
        return true;
    else
        return false;
}

bool OgateServerClient::isConnected()
{
    return (clientid > -1 && ogclient.isConnected());
}

bool OgateServerClient::hasControlInterface()
{
    return (clientid > -1 && controlif);
}

bool OgateServerClient::hasDataInterface()
{
    return (clientid > -1 && dataif);
}

bool OgateServerClient::registerControlInterface(std::string timestamp)
{
    int ret = send(REGISTER, timestamp, CONTROL_TAG) > 0;
    if(ret < 0)
        return false;
    char res[M_LENGTH];
    int j=0; ret=-1; // Try several times to get the ack from java
    while(ret < 0)
    {
        ret = ogclient.receive(res);
        j++;
        usleep(DELAY); // Required delay
    }
    char aux[M_LENGTH/2];
    sprintf(aux, "%s%c%s", REGISTER_TAG, SC, CONTROL_TAG);
    if(ret > 0 && !strncmp(res, aux, strlen(aux)))
        controlif = true;
    return controlif;
}

bool OgateServerClient::registerDataInterface(std::string timestamp)
{
    int ret = send(REGISTER, timestamp, DATA_TAG) > 0;
    if(ret < 0)
        return false;
    char res[M_LENGTH];
    int j=0; ret=-1; // Try several times to get the ack from java
    while(ret < 0)
    {
        ret = ogclient.receive(res);
        j++;
        usleep(DELAY); // Required delay
    }
    char aux[M_LENGTH/2];
    sprintf(aux, "%s%c%s", REGISTER_TAG, SC, DATA_TAG);
    if(ret > 0 && !strncmp(res, aux, strlen(aux)))
        dataif = true;
    return dataif;
}

// -1 not connected; -2 CONTROL request on non-registered comp; -3 DATA request on non-registered comp
// -4 not allowed null parameter; -5 invalid message type; -6 invalid REGISTER tag; -7 invalid CONTROL operation; 

int OgateServerClient::send(int type, std::string timestamp, std::string data)
{
    if(!ogclient.isConnected())
        return -1;
    if(timestamp.empty() || compname.empty() || data.empty())
        return -4;
    char ctag[10];
    switch(type)
    {
     case REGISTER:
        if(strcmp(data.c_str(), CONTROL_TAG) && strcmp(data.c_str(), DATA_TAG))
            return -6;
        strcpy(ctag, REGISTER_TAG);
        break;
     case CONTROL:
       {
        if(!controlif)
            return -2;
        int conop = atoi(data.c_str());
        if(conop < COMPONENT_FAILURE || conop > COMPONENT_NOIMPOP)
            return -7;
        strcpy(ctag, CONTROL_TAG); 
        break;
       }
     case DATA:
        if(!dataif)
            return -3;
        strcpy(ctag, DATA_TAG); 
        break;
     default: // Invalid type
        return -5;
    }
    char message[M_LENGTH];
    char aux[M_LENGTH-6];
    sprintf(aux, "%d%c%s%c%s%c%s%c%s", clientid, SC, timestamp.c_str(), SC, ctag, SC, compname.c_str(), SC, data.c_str());
    sprintf(message, "%05d%c%s", (int)strlen(aux)+6, SC, aux);

    return ogclient.send(message);
}

// 0 when nothing to read (sure?), -1 if not connected or no interfaces registered, -2 for a malformed message (no separator char) -3 for unknown message type.
int OgateServerClient::receive(int* type, std::string* data)
{
    if(!ogclient.isConnected() || (!dataif && !controlif))
        return -1;
    char message[M_LENGTH];
    int rec = ogclient.receive(message);
    if(rec > 0)
    {
        char sepc[2]; sepc[0]=SC; sepc[1]='\0'; // If use SC in strtok it chrases
        char* temptok = strtok(message, sepc);
        if(temptok == NULL)
            return -2;
        // Get message type
        if(!strcmp(temptok, DATA_TAG))
            *type = DATA;
        else if(!strcmp(temptok, CONTROL_TAG))
            *type = CONTROL;
        else
            return -3; // Unknown type message
        // Return data
        temptok = strtok(NULL, sepc);
        if(temptok == NULL)
            return -2;
        data->clear();
        data->assign(temptok);
        return data->size();
    }
    return -2; // Something wrong
}


int OgateServerClient::clientId()
{
    return clientid;
}


bool OgateServerClient::startTimer(int timer)
{
    if(timer < 0 || timer >= numtimers)
        return false;
    struct timeval t_ini;
    gettimeofday(&t_ini, NULL);
    timers[timer] = (double)(t_ini.tv_sec*1000000 + t_ini.tv_usec);
    return true;
}

double OgateServerClient::stopTimer(int timer)
{
    if(timer < 0 || timer >= numtimers)
        return -1;
    struct timeval t_fin;
    gettimeofday(&t_fin, NULL);
    return (double)(t_fin.tv_sec*1000000 + t_fin.tv_usec) - timers[timer];
}
