
#include "DrlServerCommunication.h"

/*
    Initialize connection with DRL server and schedule next message.
*/
void DrlServerCommunication::createDrlConnection()
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
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return;
    }

    // send greeting to DRL server
    send(sock, "chandler", strlen("chandler"), 0);
    std::cout << "Initial message sent to DRL server." << std::endl;

    // receive ACK from DRL server
    int valread = read(sock, buffer, 1024);
    std::cout << buffer << std::endl;
}

/*
    Handle self-scheduled message talking to DRL server
*/
double DrlServerCommunication::sendDrlData(const char* data)
{
    EV << "Talking to server.." << omnetpp::endl;
    ::send(sock, data, strlen(data), 0);

    // receive ACK from DRL server
    char buffer[1024] = {0};
    int valread = read(sock, buffer, 1024);
    EV << buffer << omnetpp::endl;

    double X=((double)rand()/(double)RAND_MAX);

    return X;
}

/*
    Record statistics gathered in data members at the end of simulation.
    Only runs if the simulation finishes successfully.
*/
void DrlServerCommunication::closeDrlConnection()
{
    close(sock);
}
