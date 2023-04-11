#ifndef TCPADHOCCLIENTAPP_H
#define TCPADHOCCLIENTAPP_H

#include <omnetpp.h>
#include <string.h>
#include "inet/applications/tcpapp/TcpBasicClientApp.h"


using namespace omnetpp;
using namespace inet;

class TcpAdhocClientApp : public TcpBasicClientApp
{
protected:
    int number;
    cMessage *connectionTimeoutMsg = nullptr;

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleTimer(cMessage *msg) override;

    virtual void connect() override;

public:
    virtual ~TcpAdhocClientApp();
};

#endif /* TCPADHOCCLIENTAPP */
