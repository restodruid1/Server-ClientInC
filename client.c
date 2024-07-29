#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDRESS "" // Replace with the actual server IP address
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in server_addr;
    char *message = "Hello, Server!";
    char buffer[BUFFER_SIZE];
    int iResult;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Send data to server
    if (send(client_socket, message, strlen(message), 0) < 0) {
        printf("Send failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Message sent to server: %s\n", message);
    int i;
    do {
        send(client_socket, message, strlen(message), 0);
        iResult = recv(client_socket, buffer, BUFFER_SIZE, 0);
        ++i;
        if (i > 5) {
            printf("Connection closed: \n");
            break;
        }
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            printf("Message received from server %s\n", buffer);
        }
        else if (iResult == 0) {
            printf("Connection closed: \n");
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
        }
    } while (iResult > 0);
    /*
    // Receive data from server
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        printf("Recv failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("Message received from server: %s\n", buffer);
    */
    // Close the socket
    closesocket(client_socket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}

