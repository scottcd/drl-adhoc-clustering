#include "TCPAdhocClientApp.h"
#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/physicallayer/wireless/unitdisk/UnitDiskRadio.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/mobility/single/RandomWaypointMobility.h"


#define MSGKIND_CONNECT    0
#define MSGKIND_SEND       1

Define_Module(TCPAdhocClientApp);

TCPAdhocClientApp::~TCPAdhocClientApp()
{
    cancelAndDelete(timeoutMsg);
}


void TCPAdhocClientApp::initialize(int stage)
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

void TCPAdhocClientApp::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t now = simTime();
    simtime_t start = std::max(startTime, now);
    if (timeoutMsg && ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime))) {
        timeoutMsg->setKind(MSGKIND_CONNECT);
        scheduleAt(start, timeoutMsg);
    }
}

void TCPAdhocClientApp::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (socket.getState() == TcpSocket::CONNECTED || socket.getState() == TcpSocket::CONNECTING || socket.getState() == TcpSocket::PEER_CLOSED)
        close();
}

void TCPAdhocClientApp::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(timeoutMsg);
    if (operation->getRootModule() != getContainingNode(this))
        socket.destroy();
}

void TCPAdhocClientApp::sendRequest()
{
    std::cout << "sending request" << endl;

    long requestLength = par("requestLength");
    long replyLength = par("replyLength");
    if (requestLength < 1)
        requestLength = 1;
    if (replyLength < 1)
        replyLength = 1;

    const auto& payload = makeShared<GenericAppMsg>();
    Packet *packet = new Packet("data");
    payload->setChunkLength(B(requestLength));
    payload->setExpectedReplyLength(B(replyLength));
    payload->setServerClose(false);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);

    EV_INFO << "sending request with " << requestLength << " bytes, expected reply length " << replyLength << " bytes,"
            << "remaining " << numRequestsToSend - 1 << " request\n";

    sendPacket(packet);
}

void TCPAdhocClientApp::handleTimer(cMessage *msg)
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

void TCPAdhocClientApp::connect()
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

void TCPAdhocClientApp::socketEstablished(TcpSocket *socket)
{
    std::cout << "socket established" << endl;
    TcpAppBase::socketEstablished(socket);

    // determine number of requests in this session
    numRequestsToSend = par("numRequestsPerSession");
    if (numRequestsToSend < 1)
        numRequestsToSend = 1;

    // perform first request
    sendRequest();

    numRequestsToSend--;
}

void TCPAdhocClientApp::rescheduleAfterOrDeleteTimer(simtime_t d, short int msgKind)
{
    if (stopTime < SIMTIME_ZERO || d < stopTime) {
        timeoutMsg->setKind(msgKind);
        rescheduleAfter(d, timeoutMsg);
    }
    else {
        cancelAndDelete(timeoutMsg);
        timeoutMsg = nullptr;
    }
}

void TCPAdhocClientApp::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
{
    std::cout << "socket data arrived" << endl;
    TcpAppBase::socketDataArrived(socket, msg, urgent);

    if (numRequestsToSend > 0) {
        EV << "reply arrived\n";

        if (timeoutMsg) {
            simtime_t d = par("thinkTime");
            rescheduleAfterOrDeleteTimer(d, MSGKIND_SEND);
        }
    }
    else if (socket->getState() != TcpSocket::LOCALLY_CLOSED) {
        EV_INFO << "reply to last request arrived, closing session\n";
        close();
    }
}

void TCPAdhocClientApp::close()
{
    TcpAppBase::close();
    cancelEvent(timeoutMsg);
}

void TCPAdhocClientApp::socketClosed(TcpSocket *socket)
{
    TcpAppBase::socketClosed(socket);

    // start another session after a delay
    if (timeoutMsg) {
        simtime_t d = par("idleInterval");
        rescheduleAfterOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}

void TCPAdhocClientApp::socketFailure(TcpSocket *socket, int code)
{
    TcpAppBase::socketFailure(socket, code);

    // reconnect after a delay
    if (timeoutMsg) {
        simtime_t d = par("reconnectInterval");
        rescheduleAfterOrDeleteTimer(d, MSGKIND_CONNECT);
    }
}
