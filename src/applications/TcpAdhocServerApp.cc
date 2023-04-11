
#include "TcpAdhocServerApp.h"

#include "inet/common/TimeTag_m.h"
#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/socket/SocketTag_m.h"

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/lifecycle/NodeStatus.h"


Define_Module(TcpAdhocServerApp);

/*
    TCP stuff..
*/
void TcpAdhocServerApp::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    appMsg = new cMessage("TcpAdhocApp");

    if (stage == INITSTAGE_LOCAL) {
        delay = par("replyDelay");
        maxMsgDelay = 0;
        number = msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        WATCH(number);
        WATCH(msgsRcvd);
        WATCH(msgsSent);
        WATCH(bytesRcvd);
        WATCH(bytesSent);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        const char *localAddress = "";
        int localPort = 5000;
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        socket.listen();


        scheduleAt(simTime()+1, appMsg);
    }
}


/*
    Handle self-scheduled message talking to DRL server
*/
void TcpAdhocServerApp::handleMessage(cMessage *msg)
{
    EV << "msg" << endl;
    if (msg->arrivedOn("drlIn"))
    {
        cMessage * optMsg = check_and_cast<cMessage *>(msg);
        EV << "Received parameters from DRL app!" << omnetpp::endl;

        number = 0;
        EV << "Number set to " << number << omnetpp::endl;
    }
    else if (msg->isSelfMessage()) {
        EV_INFO << "Self-Scheduled message" << endl;
        scheduleAt(simTime()+1, appMsg);
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {
        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTag<SocketReq>()->setSocketId(connId);
        sendOrSchedule(request, delay + maxMsgDelay);
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {
        Packet *packet = check_and_cast<Packet *>(msg);
        int connId = packet->getTag<SocketInd>()->getSocketId();
        ChunkQueue& queue = socketQueue[connId];
        auto chunk = packet->peekDataAt(B(0), packet->getTotalLength());
        queue.push(chunk);
        emit(packetReceivedSignal, packet);

        bool doClose = false;
        while (queue.has<GenericAppMsg>(b(-1))) {
            std::cout << "gen" << endl;
            const auto& appmsg = queue.pop<GenericAppMsg>(b(-1));
            msgsRcvd++;
            bytesRcvd += B(appmsg->getChunkLength()).get();
            B requestedBytes = appmsg->getExpectedReplyLength();
            simtime_t msgDelay = appmsg->getReplyDelay();
            if (msgDelay > maxMsgDelay)
                maxMsgDelay = msgDelay;

            if (requestedBytes > B(0)) {
                Packet *outPacket = new Packet(msg->getName(), TCP_C_SEND);
                outPacket->addTag<SocketReq>()->setSocketId(connId);
                const auto& payload = makeShared<GenericAppMsg>();
                payload->setChunkLength(requestedBytes);
                payload->setExpectedReplyLength(B(0));
                payload->setReplyDelay(0);
                payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
                outPacket->insertAtBack(payload);
                sendOrSchedule(outPacket, delay + msgDelay);
            }
            if (appmsg->getServerClose()) {
                doClose = true;
                break;
            }
        }
        delete msg;

        if (doClose) {
            auto request = new Request("close", TCP_C_CLOSE);
            TcpCommand *cmd = new TcpCommand();
            request->addTag<SocketReq>()->setSocketId(connId);
            request->setControlInfo(cmd);
            sendOrSchedule(request, delay + maxMsgDelay);
        }
    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
    {
        socket.processMessage(msg);
    }
    else {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

/*
    Record statistics gathered in data members at the end of simulation.
    Only runs if the simulation finishes successfully.
*/
void TcpAdhocServerApp::finish()
{
    TcpGenericServerApp::finish();

    std::cout << "done" << endl;
}

int TcpAdhocServerApp::getNumber()
{
    return number;
}
