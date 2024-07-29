#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT "8080"
#define BUFFER_SIZE 1024


int main () {
    WSADATA wsaData;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    
    char recvbuf[BUFFER_SIZE];
    int iResult, iSendResult;
    int recvbuflen = BUFFER_SIZE;


    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed \n");
        return 1;
    }

    //Create socket
    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    if ((getaddrinfo(NULL, PORT, &hints, &result))!= 0) {
        printf("getaddrinfo failed \n");
        WSACleanup();
        return 1;
    }

    
    
    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if (bind(ListenSocket, (*result).ai_addr, (int)(*result).ai_addrlen) == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Listening on port %s for client\n", PORT);
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR ) {
        printf("Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    
    // Receive until the peer shuts down the connection
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            printf("Message: %s\n", recvbuf);

            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);


    // shutdown the send half of the connection since no more data will be sent
    //iResult = shutdown(ClientSocket, SD_SEND);
    if (shutdown(ClientSocket, SD_SEND) == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}


/*
int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == SOCKET_ERROR) { // Increase the backlog size
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %d\n", PORT);

    // Accept and handle incoming connections
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Receive data from client
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received == SOCKET_ERROR) {
        printf("Recv failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("Message received from client: %s\n", buffer);

    // Echo the data back to the client
    if (send(client_socket, buffer, bytes_received, 0) == SOCKET_ERROR) {
        printf("Send failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Close the sockets
    closesocket(client_socket);
    closesocket(server_socket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
*/