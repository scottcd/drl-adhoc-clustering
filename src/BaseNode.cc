#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class BaseNode : public cSimpleModule
{
  protected:
    // The following redefined virtual function holds the algorithm.
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(BaseNode);

/*
    Initialize the base node. If node's name is ding, send greeting message.
*/
void BaseNode::initialize()
{
    // if I am lead ding, send greeting message
    if (strcmp("ding", getName()) == 0) {
        send(new cMessage("Greetings"), "out");
    }
}

/*
    Event when node receives a message. If ding, return 'Howdy', else, return 'Hey'.
*/
void BaseNode::handleMessage(cMessage *msg)
{
    // receive, digest, then delete message
    EV << "Received message: " << msg->getFullName() << omnetpp::endl;
    delete msg;

    // return a message
    if (strcmp("ding", getName()) == 0) {
        send(new cMessage("Howdy"), "out");
    }
    else {
        send(new cMessage("Hey"), "out");
    }
}
