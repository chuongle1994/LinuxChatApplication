#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <chrono>
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " ip_address port" << std::endl;
        exit(1);
    }

    const char *serverIp = argv[1];
    int port = std::atoi(argv[2]);

    char msg[2000];
    struct sockaddr_in sendSockAddr;
    std::memset(&sendSockAddr, 0, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(serverIp);
    sendSockAddr.sin_port = htons(port);

    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSd == -1) {
        std::cerr << "Error creating socket!" << std::endl;
        exit(1);
    }

    if (connect(clientSd, reinterpret_cast<struct sockaddr*>(&sendSockAddr), sizeof(sendSockAddr)) < 0) {
        std::cerr << "Error connecting to socket!" << std::endl;
        exit(1);
    }

    std::cout << "Connected to the server!" << std::endl;
    
    int bytesRead = 0, bytesWritten = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (true) {
        std::cout << "> ";
        std::string data;
        std::getline(std::cin, data);

        std::memset(msg, 0, sizeof(msg));
        std::strcpy(msg, data.c_str());

        if (data == "exit") {
            send(clientSd, msg, std::strlen(msg), 0);
            break;
        }

        bytesWritten += send(clientSd, msg, std::strlen(msg), 0);
        std::cout << "Awaiting server response..." << std::endl;

        std::memset(msg, 0, sizeof(msg));
        bytesRead += recv(clientSd, msg, sizeof(msg), 0);

        if (!std::strcmp(msg, "exit")) {
            std::cout << "Server has quit the session" << std::endl;
            break;
        }

        std::cout << "Server: " << msg << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    close(clientSd);

    std::cout << "******** Session ********" << std::endl;
    std::cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << std::endl;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "Connection closed" << std::endl;

    return 0;
}
