#include "TcpAdhocClientApp.h"

#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/physicallayer/wireless/unitdisk/UnitDiskRadio.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/mobility/single/RandomWaypointMobility.h"


#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

Define_Module(TcpAdhocClientApp);

TcpAdhocClientApp::~TcpAdhocClientApp()
{
    cancelAndDelete(timeoutMsg);
    cancelAndDelete(connectionTimeoutMsg);
}


void TcpAdhocClientApp::initialize(int stage)
{
    EV << "online" << endl;;
    TcpAppBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numRequestsToSend = 1;
        WATCH(numRequestsToSend);
        WATCH(number);
        startTime = par("startTime");
        stopTime = par("stopTime");

        timeoutMsg = new cMessage("timer");
        connectionTimeoutMsg = new cMessage("Try for Connections Timer");
        connectionTimeoutMsg->setKind(MSGKIND_CONNECT);
    }
}


void TcpAdhocClientApp::handleTimer(cMessage *msg)
{
    EV << "hi" << endl;
    switch (msg->getKind()) {
        case MSGKIND_CONNECT:
            connect();

            // schedule next check for connections
            cancelEvent(connectionTimeoutMsg);
            scheduleAt(simTime() + 1, connectionTimeoutMsg);
            break;

        case MSGKIND_SEND:
            sendRequest();
            numRequestsToSend--;
            break;

        default:
            throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}

void TcpAdhocClientApp::connect()
{
    cModule* transmitter = getContainingNode(this)->getSubmodule("wlan", 0)->getSubmodule("radio")->getSubmodule("transmitter");
    double range = transmitter->par("communicationRange").doubleValue();

    cModule* mobMpdule = getContainingNode(this)->getSubmodule("mobility");
    RandomWaypointMobility* mob = check_and_cast<RandomWaypointMobility *>(mobMpdule);
    const Coord myPosition = mob->getCurrentPosition();

    std::vector<cModule*> nodesInRange;
    int n = par("numNodes").intValue();                                           /* number of nodes */;
    int me = getContainingNode(this)->getIndex();

    for (int i = 0; i < n; i++) {
        if (i == me)
        {
            continue;
        }
        std::string node_name = "node[" + std::to_string(i) + "]";
        const char* n = node_name.c_str();

        cModule* node = getModuleByPath(n);
        if (node != nullptr) {
            RandomWaypointMobility* nodeMobility = check_and_cast<RandomWaypointMobility *>(node->getSubmodule("mobility"));
            const Coord nodePosition  = nodeMobility->getCurrentPosition();

            double distance = (myPosition - nodePosition).length();
            if (distance <= range)
            {
                nodesInRange.push_back(node);
            }
        }
    }

    L3Address destination;
    int connectPort = par("connectPort");
    const char *localAddress = par("localAddress");
    int localPort = par("localPort");
    int timeToLive = par("timeToLive");
    int dscp = par("dscp");
    int tos = par("tos");


    for (auto& node : nodesInRange) {
        EV << "Connecting to " << node->getFullName() << endl;

        socket.renewSocket();
        socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);

        if (timeToLive != -1)
            socket.setTimeToLive(timeToLive);

        if (dscp != -1)
            socket.setDscp(dscp);

        if (tos != -1)
            socket.setTos(tos);

        L3AddressResolver().tryResolve(node->getFullName(), destination);
        socket.connect(destination, connectPort);
        numSessions++;
        emit(connectSignal, 1L);
        std::cout << "Connecting to " << node->getFullName() << endl;
    }
}

