
#include <string.h>
#include <omnetpp.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "OptimizationMsg_m.h"
#include "TCPClusteringApp.h"

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
    EV << "hi" << endl;
    // initialize communication with TCP app
    cGate* tcpAppGate =  getParentModule()->getSubmodule("app", 2)->gate("drlIn");
    gate("out")->connectTo(tcpAppGate);
    OptimizationMsg *msg = new OptimizationMsg("Initialize");
    msg->setNumber(1);
    send(msg, "out");

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
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
    scheduleAt(simTime() + 1, new cMessage("Scheduled DRL Server Ping"));
}

/*
    Handle self-scheduled message talking to DRL server
*/
void DRLApp::handleMessage(cMessage *msg)
{
    EV << "Talking to server.." << omnetpp::endl;

    TCPClusteringApp* m = (TCPClusteringApp*)getParentModule()->getSubmodule("app", 2);
    int number = m->getNumber();
    EV << number << omnetpp::endl;
    //EV << getParentModule()->getSubmodule("app", 2)->getProperties() << omnetpp::endl;

    // send message to DRL server
    const char *str =  "5";
    ::send(sock, str, strlen(str), 0);
    EV << "Message sent to DRL server." << omnetpp::endl;

    // receive ACK from DRL server
    char buffer[1024] = {0};
    int valread = read(sock, buffer, 1024);
    EV << buffer << omnetpp::endl;

    // send data to TCPClusteringApp
    OptimizationMsg *optMsg = new OptimizationMsg("Parameter Optimization");
    optMsg->setNumber(atoi(buffer));
    send(optMsg, "out");

    // schedule next self message and clean up
    scheduleAt(simTime() + 1, new cMessage("Scheduled DRL Server Ping"));
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
