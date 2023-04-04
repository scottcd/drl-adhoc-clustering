
#include <string.h>
#include <omnetpp.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 65432


using namespace omnetpp;

class DRLApp : public cSimpleModule
{
  private:
    int sock;
  
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};


// The module class needs to be registered with OMNeT++
Define_Module(DRLApp);

/*
    Initialize connection with DRL server and schedule next message.
*/
void DRLApp::initialize()
{
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

    // schedule next message
    scheduleAt(simTime() + 1, new cMessage);
}

/*
    Handle self-scheduled message talking to DRL server
*/
void DRLApp::handleMessage(cMessage *msg)
{
    EV << "Talking to server.." << omnetpp::endl;

    cModule* node = getParentModule();
    EV << node->getSubmodule("wlan", 0) << omnetpp::endl;

    // send message to DRL server
    const char *str =  "Number 5";
    ::send(sock, str, strlen(str), 0);
    std::cout << "Message sent to DRL server." << std::endl;

    // receive ACK from DRL server
    char buffer[1024] = {0};
    int valread = read(sock, buffer, 1024);
    std::cout << buffer << std::endl;

    // schedule next self message and clean up
    scheduleAt(simTime() + 1, new cMessage);
    delete msg;
}

/*
    Record statistics gathered in data members at the end of simulation.
    Only runs if the simulation finishes successfully.
*/
void DRLApp::finish()
{
    EV << "Some cool data for " << getFullName() << omnetpp::endl;
    close(sock);
}
