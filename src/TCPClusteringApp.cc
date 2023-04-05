#include <string.h>
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
    int getNumber();
 };


// The module class needs to be registered with OMNeT++
Define_Module(TCPClusteringApp);

/*
    TCP stuff..
*/
void TCPClusteringApp::initialize()
{
    number = par("number");
    WATCH(number);
}

/*
    Handle self-scheduled message talking to DRL server
*/
void TCPClusteringApp::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("drlIn"))
    {
        OptimizationMsg * optMsg = check_and_cast<OptimizationMsg *>(msg);
        EV << "Received parameters from DRL app!" << omnetpp::endl;

        number = optMsg->getNumber();
        EV << "Number set to " << number << omnetpp::endl;
    }
    else
    {
        EV << "Different type of msg.." << omnetpp::endl;
    }

    delete msg;
}

/*
    Record statistics gathered in data members at the end of simulation.
    Only runs if the simulation finishes successfully.
*/
void TCPClusteringApp::finish()
{
    EV << "Some cool data for " << getFullName() << omnetpp::endl;
}

int TCPClusteringApp::getNumber()
{
    return number;
}
