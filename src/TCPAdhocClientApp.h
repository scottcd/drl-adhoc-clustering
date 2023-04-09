#ifndef TCPADHOCCLIENTAPP_H
#define TCPADHOCCLIENTAPP_H

#include <omnetpp.h>
#include <string.h>
#include "OptimizationMsg_m.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/applications/tcpapp/TcpAppBase.h"


using namespace omnetpp;
using namespace inet;

class TCPAdhocClientApp : public TcpAppBase
{
protected:
    int number;
    cMessage *timeoutMsg = nullptr;
    cMessage *connectionTimeoutMsg = nullptr;
    int numRequestsToSend = 0; // requests to send in this session
    simtime_t startTime;
    simtime_t stopTime;

    virtual void sendRequest();
    virtual void rescheduleAfterOrDeleteTimer(simtime_t d, short int msgKind);

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleTimer(cMessage *msg) override;

    virtual void connect() override;

    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void close() override;

public:
    TCPAdhocClientApp() {}
    virtual ~TCPAdhocClientApp();
};

#endif /* TCPADHOCCLIENTAPP */
