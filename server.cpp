#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <sys/time.h>
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " port" << std::endl;
        exit(1);
    }

    int port = std::atoi(argv[1]);

    char msg[2000];
    struct sockaddr_in servAddr;
    std::memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd == -1) {
        std::cerr << "Error establishing the server socket" << std::endl;
        exit(1);
    }

    if (bind(serverSd, reinterpret_cast<struct sockaddr*>(&servAddr), sizeof(servAddr)) < 0) {
        std::cerr << "Error binding socket to local address" << std::endl;
        exit(1);
    }

    std::cout << "Waiting for a client to connect..." << std::endl;

    listen(serverSd, 5);

    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

    int newSd = accept(serverSd, reinterpret_cast<struct sockaddr*>(&newSockAddr), &newSockAddrSize);
    if (newSd == -1) {
        std::cerr << "Error accepting request from client!" << std::endl;
        exit(1);
    }

    std::cout << "Connected with client!" << std::endl;

    struct timeval start1, end1;
    gettimeofday(&start1, nullptr);

    int bytesRead = 0, bytesWritten = 0;

    while (true) {
        std::cout << "Awaiting client response..." << std::endl;
        std::memset(msg, 0, sizeof(msg));

        bytesRead += recv(newSd, msg, sizeof(msg), 0);
        if (!std::strcmp(msg, "exit")) {
            std::cout << "Client has quit the session" << std::endl;
            break;
        }

        std::cout << "Client: " << msg << std::endl;
        std::cout << "> ";
        
        std::string data;
        std::getline(std::cin, data);
        std::memset(msg, 0, sizeof(msg));
        std::strcpy(msg, data.c_str());

        if (data == "exit") {
            send(newSd, msg, std::strlen(msg), 0);
            break;
        }

        bytesWritten += send(newSd, msg, std::strlen(msg), 0);
    }

    gettimeofday(&end1, nullptr);

    close(newSd);
    close(serverSd);

    std::cout << "******** Session ********" << std::endl;
    std::cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << std::endl;
    std::cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec) << " secs" << std::endl;
    std::cout << "Connection closed..." << std::endl;

    return 0;
}
