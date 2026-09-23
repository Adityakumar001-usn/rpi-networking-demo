#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 6000;
const int BUFFER_SIZE = 1024;

int main() {
    // 1. Create a UDP socket using socket()
    // AF_INET for IPv4, SOCK_DGRAM for UDP
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    // Prepare the sockaddr_in structure
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any available interface
    server_addr.sin_port = htons(PORT);       // 2. Bind to UDP port 6000

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    std::cout << "UDP Server\n";
    std::cout << "Listening on port " << PORT << "...\n\n";

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Continuous communication loop
    while (true) {
        // 3. Wait for a datagram using recvfrom()
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                          (struct sockaddr*)&client_addr, &client_len);

        if (bytes_received == -1) {
            perror("recvfrom failed");
            break;
        }

        // Null-terminate safely
        buffer[bytes_received] = '\0';

        // 4. Display sender IP address
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        // 5. Display received message
        std::cout << "Message From " << client_ip << " : " << buffer << "\n";

        // 6. Send an echo/reply using sendto()
        std::string reply = std::string(buffer);
        ssize_t bytes_sent = sendto(server_fd, reply.c_str(), reply.length(), 0,
                                    (struct sockaddr*)&client_addr, client_len);

        if (bytes_sent == -1) {
            perror("sendto failed");
        } else {
            std::cout << "Reply sent.\n\n";
        }
    }

    // 7. Close the socket cleanly
    close(server_fd);

    return 0;
}
