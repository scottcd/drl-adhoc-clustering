#ifndef TCPCLUSTERINGAPP_H
#define TCPCLUSTERINGAPP_H

#include <omnetpp.h>
#include "OptimizationMsg_m.h"

using namespace omnetpp;

class TCPClusteringApp : public cSimpleModule
{
private:
    int number;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

public:
    TCPClusteringApp();
    virtual ~TCPClusteringApp();
    int getNumber();
};

#endif /* TCPCLUSTERINGAPP_H */
