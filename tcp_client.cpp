#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int PORT = 5000;
const int BUFFER_SIZE = 1024;
const std::string DEFAULT_SERVER_IP = "192.168.1.100";

int main(int argc, char *argv[]) {
    // 2. Accept optional server IP from command-line argument
    // 3. Default to 192.168.1.100 if no argument is supplied
    std::string server_ip = DEFAULT_SERVER_IP;
    if (argc > 1) {
        server_ip = argv[1];
    }

    // 1. Create a TCP socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); // 4. Connect to TCP port 5000

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported: " << server_ip << "\n";
        close(client_fd);
        return 1;
    }

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_fd);
        return 1;
    }

    // 5. Print "Connected to Server!"
    std::cout << "Connected to Server!\n";

    // Continuous communication loop
    while (true) {
        std::cout << "\nEnter message: ";
        std::string message;
        std::getline(std::cin, message);

        if (message == "exit") {
            std::cout << "Closing connection...\n";
            break;
        }

        // 6. Send the message
        ssize_t bytes_sent = send(client_fd, message.c_str(), message.length(), 0);
        if (bytes_sent == -1) {
            perror("Send failed");
            break;
        }

        // 7. Receive the server response
        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received > 0) {
            // Null-terminate safely just in case
            buffer[bytes_received] = '\0';
            // 8. Display the response
            std::cout << "Server Replied: " << buffer << "\n";
        } else if (bytes_received == 0) {
            std::cout << "Server disconnected gracefully.\n";
            break;
        } else {
            perror("Receive failed");
            break;
        }
    }

    // 9. Close the socket correctly
    close(client_fd);

    return 0;
}
