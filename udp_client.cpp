#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 6000;
const int BUFFER_SIZE = 1024;
const std::string DEFAULT_SERVER_IP = "192.168.1.100";

int main(int argc, char *argv[]) {
    // 2. Accept optional server IP from command-line argument
    // 3. Default to 192.168.1.100
    std::string server_ip = DEFAULT_SERVER_IP;
    if (argc > 1) {
        server_ip = argv[1];
    }

    // 1. Create a UDP socket
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); // 4. Use UDP port 6000

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported: " << server_ip << "\n";
        close(client_fd);
        return 1;
    }

    // Interactive user input
    std::cout << "Enter message: ";
    std::string message;
    std::getline(std::cin, message);

    // 5. Send a message using sendto()
    ssize_t bytes_sent = sendto(client_fd, message.c_str(), message.length(), 0,
                                (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (bytes_sent == -1) {
        perror("sendto failed");
        close(client_fd);
        return 1;
    }

    // 6. Receive the server response using recvfrom()
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    // Use a small timeout or assume prompt response for simplicity of demo
    // We will just block until we receive the reply.
    ssize_t bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr *)&from_addr, &from_len);

    if (bytes_received > 0) {
        // 7. Display the response
        std::cout << "Server Replied: " << buffer << "\n";
    } else {
        perror("recvfrom failed");
    }

    // 8. Close the socket correctly
    close(client_fd);

    return 0;
}
