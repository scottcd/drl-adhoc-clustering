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
// initializeComs = par("initiate");
    initializeComs = true;
    receivedMessages = 0;
    sentMessages = 0;

    WATCH(receivedMessages);
    WATCH(sentMessages);


    //cGate *gate = gate("port$o");

    //EV << "MSG  " << gate->getName() << omnetpp::endl;

}

/*
    Event when node receives a message. If ding, return 'Howdy', else, return 'Hey'.
*/
void BaseNode::handleMessage(cMessage *msg)
{
    bubble("Received a message!");
    // receive, digest, then delete message
    EV << "Received message: " <<  msg->getFullName() << omnetpp::endl;
    receivedMessages++;
    delete msg;
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
