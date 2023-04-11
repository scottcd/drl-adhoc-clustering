#include "PingAdhocApp.h"


Define_Module(PingAdhocApp);

void PingAdhocApp::initialize(int stage)
{
    PingApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        drl = par("drl");
        sending = false;
        if (drl == true)
        {
            createDrlConnection();
        }
        timeoutTimer = new cMessage("Ping Timeout");
    }
}

void PingAdhocApp::handleSelfMessage(cMessage *msg)
{
    EV << "hello" << endl;
    if (msg->getKind() == PING_FIRST_ADDR) {
        srcAddr = L3AddressResolver().resolve(par("srcAddr"));
        parseDestAddressesPar();
        if (destAddresses.empty()) {
            return;
        }
        destAddrIdx = 0;
        msg->setKind(PING_CHANGE_ADDR);
    }

    if (msg->getKind() == PING_CHANGE_ADDR) {
        if (destAddrIdx >= (int)destAddresses.size())
            return;
        destAddr = destAddresses[destAddrIdx];
        EV_INFO << "Starting up: dest=" << destAddr << "  src=" << srcAddr << "seqNo=" << sendSeqNo << endl;
        ASSERT(!destAddr.isUnspecified());
        const Protocol *networkProtocol = nullptr;
        const char *networkProtocolAsString = par("networkProtocol");
        if (*networkProtocolAsString)
            networkProtocol = Protocol::getProtocol(networkProtocolAsString);
        else {
            switch (destAddr.getType()) {
                case L3Address::IPv4: networkProtocol = &Protocol::ipv4; break;
                case L3Address::IPv6: networkProtocol = &Protocol::ipv6; break;
                case L3Address::MODULEID:
                case L3Address::MODULEPATH: networkProtocol = &Protocol::nextHopForwarding; break;
                default: throw cRuntimeError("unknown address type: %d(%s)", (int)destAddr.getType(), L3Address::getTypeName(destAddr.getType()));
            }
        }
        const Protocol *icmp = l3Echo.at(networkProtocol);

        for (auto socket : socketMap.getMap()) {
            socket.second->close();
        }
        currentSocket = nullptr;
        if (networkProtocol == &Protocol::ipv4)
            currentSocket = new Ipv4Socket(gate("socketOut"));
        else if (networkProtocol == &Protocol::ipv6)
            currentSocket = new Ipv6Socket(gate("socketOut"));
        else
            currentSocket = new L3Socket(networkProtocol, gate("socketOut"));
        socketMap.addSocket(currentSocket);
        currentSocket->bind(icmp, L3Address());
        currentSocket->setCallback(this);
        msg->setKind(PING_SEND);
    }

    ASSERT2(msg->getKind() == PING_SEND, "Unknown kind in self message.");

    cModule* selfNode = getContainingNode(this);
    cModule* destNode = L3AddressResolver().findHostWithAddress(destAddr);


    // check via DRL if we should ping based on location, velocity
    if (drl == true && selfNode->getFullName() != destNode->getFullName())
    {
        EV << "DRL CHECK - only ping if score > 0.5."  << endl;

        RandomWaypointMobility* sourceMobility = check_and_cast<RandomWaypointMobility *>(selfNode->getSubmodule("mobility"));
        RandomWaypointMobility* destMobility = check_and_cast<RandomWaypointMobility *>(destNode->getSubmodule("mobility"));

        const Coord sourcePosition  = sourceMobility->getCurrentPosition();
        const Coord sourceVelocity  = sourceMobility->getCurrentVelocity();
        const Coord destPosition  = destMobility->getCurrentPosition();
        const Coord destVelocity = destMobility->getCurrentVelocity();
        std::string sendString = sourcePosition.str() + " "
                + sourceVelocity.str() + " "
                + destPosition.str() + " "
                + destVelocity.str() + " ";

        // send src and dest nodes' position and velocity
        double score = sendDrlData(sendString.c_str());

        if (score > 0.5) {
            EV << score << " > 0.5. Pinging "  << destAddr << "." << endl;
            sendPingRequest();
            sending = true;
        } else {
            EV << score << " <= 0.5. Not pinging "  << destAddr << "." << endl;
        }
    }
    // else, just send normally
    else if (selfNode->getFullName() != destNode->getFullName())
    {
        sendPingRequest();
        sending = true;
    }
    else
    {
        EV << "Skipping ping addressed to self." << endl;
    }

    if (count > 0 && sendSeqNo % count == 0) {
        // choose next dest address
        destAddrIdx++;
        msg->setKind(PING_CHANGE_ADDR);
        if (destAddrIdx >= (int)destAddresses.size()) {
            if (continuous) {
                destAddrIdx = destAddrIdx % destAddresses.size();
            }
        }
    }

    // then schedule next one if needed
    scheduleNextPingRequest(simTime(), msg->getKind() == PING_CHANGE_ADDR);
}

void PingAdhocApp::socketDataArrived(INetworkSocket *socket, Packet *packet)
{

    if (sending == true)
    {
        const char* name = getContainingNode(this)->getFullName();
        std::string myData = std::string(name) + ": socket data arrived";

        sendDrlData(myData.c_str());
        sending = false;
    }
    PingApp::socketDataArrived(socket, packet);
}

void PingAdhocApp::finish()
{
    PingApp::finish();
    closeDrlConnection();
    std::cout << "done" << endl;
}
