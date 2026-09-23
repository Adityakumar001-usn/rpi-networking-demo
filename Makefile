CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGETS = tcp_server tcp_client udp_server udp_client

all: $(TARGETS)

tcp_server: tcp_server.cpp
	$(CXX) $(CXXFLAGS) -o tcp_server tcp_server.cpp

tcp_client: tcp_client.cpp
	$(CXX) $(CXXFLAGS) -o tcp_client tcp_client.cpp

udp_server: udp_server.cpp
	$(CXX) $(CXXFLAGS) -o udp_server udp_server.cpp

udp_client: udp_client.cpp
	$(CXX) $(CXXFLAGS) -o udp_client udp_client.cpp

clean:
	rm -f $(TARGETS)

.PHONY: all clean
