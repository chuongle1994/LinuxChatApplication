This project demonstrates a basic client-server application implemented in C++. The server listens for incoming client connections on a specified port, while the client can connect to the server and exchange messages. The project also tracks and displays performance metrics such as bytes read/written and session duration.
Compile the server code:
Server....
g++ server.cpp -o server
Run the server with a specified port number:
./server <port_number>
Client...
Compile the client code:

g++ client.cpp -o client
Run the client with the server's IP address and port number:

./client <server_ip> <port_number>
