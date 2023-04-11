#ifndef TCPADHOCSERVERAPP_H
#define TCPADHOCSERVERAPP_H

#include <omnetpp.h>
#include <string.h>
#include "inet/applications/tcpapp/TcpGenericServerApp.h"




using namespace omnetpp;
using namespace inet;

class TcpAdhocServerApp : public TcpGenericServerApp
{
protected:
    cMessage *appMsg = nullptr;

    int number;


protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

public:
    int getNumber();
};

#endif /* TCPADHOCAPP */
