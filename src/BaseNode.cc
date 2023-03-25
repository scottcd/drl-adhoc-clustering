#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class BaseNode : public cSimpleModule
{
  private:
    long receivedMessages;
    long sentMessages;
    bool initializeComs;
  
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

// The module class needs to be registered with OMNeT++
Define_Module(BaseNode);

/*
    Initialize the base node. If node's name is ding, send greeting message.
*/
void BaseNode::initialize()
{
    initializeComs = par("initiate");
    receivedMessages = 0;
    sentMessages = 0;

    WATCH(receivedMessages);
    WATCH(sentMessages);

    int n = gateSize("gate");
    int outputGate = intuniform(0, n-1);
    if (initializeComs == true) {
        send(new cMessage("Greetings"), "gate$o", outputGate);
        sentMessages++;
    }
}

/*
    Event when node receives a message. If ding, return 'Howdy', else, return 'Hey'.
*/
void BaseNode::handleMessage(cMessage *msg)
{
    // receive, digest, then delete message
    EV << "Received message: " <<  msg->getFullName() << omnetpp::endl;
    receivedMessages++;
    delete msg;

    int n = gateSize("gate");
    int outputGate = intuniform(0, n-1);
    // return a message
    if (initializeComs == true) {
        send(new cMessage("Howdy"), "gate$o", outputGate);
        sentMessages++;
    }
    else {
        send(new cMessage("Hey"), "gate$o", outputGate);
        sentMessages++;
    }
}

/*
    Record statistics gathered in data members at the end of simulation.
    Only runs if the simulation finishes successfully.
*/
void BaseNode::finish()
{
    EV << "Some cool data for " << getFullName() << omnetpp::endl;
    EV << "Number of Messages Sent:     " << sentMessages << omnetpp::endl;
    EV << "Number of Messages Received: " << receivedMessages << omnetpp::endl;
}
