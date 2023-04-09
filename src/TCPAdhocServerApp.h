#ifndef TCPADHOCSERVERAPP_H
#define TCPADHOCSERVERAPP_H

#include <omnetpp.h>
#include <string.h>
#include "OptimizationMsg_m.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"



using namespace omnetpp;
using namespace inet;

class TCPAdhocServerApp : public cSimpleModule
{
protected:
    TcpSocket socket;
    simtime_t delay;
    simtime_t maxMsgDelay;
    std::map<int, ChunkQueue> socketQueue;
    cMessage *appMsg = nullptr;

    int number;
    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;

protected:
    virtual void sendBack(cMessage *msg);
    virtual void sendOrSchedule(cMessage *msg, simtime_t delay);

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

public:
    //TCPAdhocApp();
    //virtual ~TCPAdhocApp();
    int getNumber();
};

#endif /* TCPADHOCAPP */
