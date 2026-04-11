all:client tcpepoll
client:client.cpp
	g++ -g -o client client.cpp InetAddress.cpp
tcpepoll:tcpepoll.cpp
	g++ -g -o tcpepoll tcpepoll.cpp InetAddress.cpp
clean:
	rm -f client tcpepoll