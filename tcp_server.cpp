#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 5000;
const int BUFFER_SIZE = 1024;

int main() {
    // 1. Create a TCP socket using socket()
    // AF_INET for IPv4, SOCK_STREAM for TCP
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure the socket appropriately (allow address reuse)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(server_fd);
        return 1;
    }

    // 2. Prepare the sockaddr_in structure
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any available interface
    server_addr.sin_port = htons(PORT);       // Bind to TCP port 5000

    // 3. Bind the socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // 4. Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    std::cout << "TCP Server\n";
    std::cout << "Listening on port " << PORT << "...\n";
    std::cout << "Waiting for client...\n\n";

    // 5. Accept a client connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd == -1) {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    // Convert client IP to string
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

    std::cout << "Client connected from " << client_ip << "\n";

    // Continuous communication loop
    while (true) {
        // 6. Receive a message from the client
        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received > 0) {
            // Null-terminate safely
            buffer[bytes_received] = '\0';

            // 7. Display the received message
            std::cout << "Client Says: " << buffer << "\n";

            // 8. Send an echo/reply back to the client
            std::string reply = std::string(buffer); // echo the message
            ssize_t bytes_sent = send(client_fd, reply.c_str(), reply.length(), 0);

            if (bytes_sent == -1) {
                perror("Send failed");
                break;
            } else {
                std::cout << "Reply sent.\n";
            }
        } else if (bytes_received == 0) {
            std::cout << "Client disconnected.\n";
            break;
        } else {
            perror("Receive failed");
            break;
        }
    }

    std::cout << "Server shutting down...\n";

    // 9 & 10. Close sockets correctly
    close(client_fd);
    close(server_fd);

    return 0;
}
