#ifndef PINGADHOCAPP_H
#define PINGADHOCAPP_H

#include "DrlServerCommunication.h"
#include "inet/applications/pingapp/PingApp.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/applications/pingapp/PingApp_m.h"
#include "inet/networklayer/contract/ipv4/Ipv4Socket.h"
#include "inet/networklayer/contract/ipv6/Ipv6Socket.h"
#include "inet/networklayer/contract/L3Socket.h"


using namespace inet;
using namespace omnetpp;


class PingAdhocApp : public PingApp, public DrlServerCommunication
{
    protected:
        bool drl;
        bool sending;
        cMessage* timeoutTimer = nullptr;

        virtual void initialize(int stage) override;
        virtual void handleSelfMessage(cMessage *msg) override;
        virtual void socketDataArrived(INetworkSocket *socket, Packet *packet) override;
        virtual void finish() override;

};


#endif
