#ifndef DRLSERVER_H
#define DRLSERVER_H

#include <string.h>
#include <omnetpp.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "TcpAdhocServerApp.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/mobility/single/RandomWaypointMobility.h"




#define PORT 65432

using namespace inet;
using namespace omnetpp;

class DrlServerCommunication
{
    protected:
        int sock;

        void createDrlConnection(const char* name);
        void sendDrlData(const char* data);
        double receiveDrlData();
        void closeDrlConnection();
};

#endif
