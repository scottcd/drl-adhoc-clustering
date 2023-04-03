
#include <string.h>
#include <omnetpp.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 65432


using namespace omnetpp;

class MyApp : public cSimpleModule
{
  private:
    long receivedMessages;
    long sentMessages;
    bool initializeComs;
    int sock;
  
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};


// The module class needs to be registered with OMNeT++
Define_Module(MyApp);

/*
    Initialize the base node. If node's name is ding, send greeting message.
*/
void MyApp::initialize()
{
    // initializeComs = par("initiate");
    initializeComs = true;
    receivedMessages = 0;
    sentMessages = 0;

    WATCH(receivedMessages);
    WATCH(sentMessages);

    EV << "Initializing.." << endl;

    // establish connection to DRL server
    sock = 0;
    struct sockaddr_in serv_addr;

    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return;
    }


    // send greeting to DRL server
    cModule* node = getParentModule();
    const char *nodeName =  node->getFullName();
    ::send(sock, nodeName, strlen(nodeName), 0);
    std::cout << "Initial message sent to DRL server." << std::endl;

    // receive ACK from DRL server
    int valread = read(sock, buffer, 1024);
    std::cout << buffer << std::endl;
}

/*
    Event when node receives a message. If ding, return 'Howdy', else, return 'Hey'.
*/
void MyApp::handleMessage(cMessage *msg)
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
void MyApp::finish()
{
    EV << "Some cool data for " << getFullName() << omnetpp::endl;
    EV << "Number of Messages Sent:     " << sentMessages << omnetpp::endl;
    EV << "Number of Messages Received: " << receivedMessages << omnetpp::endl;

    close(sock);
}
