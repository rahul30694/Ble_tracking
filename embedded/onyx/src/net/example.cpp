#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


#define MAX_CONNECTIONS			10
#define DEFAULT_PORT_NUM		0

class ip_socket {

public:
	enum con_type_e {
		CON_TYPE_INVALID=0,
		CON_TYPE_TCP,
		CON_TYPE_UDP
	};

	enum con_state_e {
		CON_INVALID=0,
		CON_INIT,
		CON_LISTEN,
	};

private:
	
	int sock_fd;

	int con_fds[MAX_CONNECTIONS];
	std::bitset<MAX_CONNECTIONS> con_fds_status;


	int setConFD(int con_fds);
	int findConFD(int con_ndx);

public:
	ip_socket();
	~ip_socket() { close(); }

	int getSocketDescriptor() { return sock_fd; }

	int create(con_type_e type, const char *own_ip_addr=NULL, unsigned port=DEFAULT_PORT_NUM);
	void close()
	{
		if (sock_fd < 0)
			return;
		::close(sock_fd);
	}

	int send(int con_ndx, const void *buf, unsigned len);
	int receive(int con_ndx, void *buf, unsigned max_len);

	int send(int con_ndx, const void *buf, unsigned len, char *dest_ip, unsigned dest_port);
	int receive(int con_ndx, void *buf, unsigned len, char *dest_ip, unsigned dest_port);

	int listen(void);
	int accept(char *ip_addr, unsigned max_len);
	int connect(char *ip_addr, unsigned port_num);
};


ip_socket::ip_socket(void)
{
	sock_fd = -1;
	memset(&con_fds, -1, sizeof(con_fds));
}

int ip_socket::setConFD(int con_fd)
{
	unsigned i;
	for (i=1; i<MAX_CONNECTIONS; i++)
		if (!con_fds_status[i])
			break;
	if (i >= MAX_CONNECTIONS)
		return -1;
	con_fds_status[i] = 1;
	con_fds[i] = con_fd;
	return i;
}

int ip_socket::create(con_type_e type, const char *own_ip_addr, unsigned port)
{
	if (type == CON_TYPE_TCP)
		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	else
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock_fd < 0)
		return -1;

	struct sockaddr_in ip_addr;
	memset(&ip_addr, 0, sizeof(ip_addr));

	ip_addr.sin_family = AF_INET;
	ip_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ip_addr.sin_port = htons(port);

	if ((own_ip_addr != NULL) and (inet_pton(AF_INET, own_ip_addr, &ip_addr.sin_addr)) <= 0)
		goto FAIL;
	
	if (bind(sock_fd, reinterpret_cast<const struct sockaddr *>(&ip_addr), sizeof(ip_addr)))
		goto FAIL;

	std :: cout << "Socket Opened at";
	if (own_ip_addr)
		std :: cout << " " << own_ip_addr << " and";
	std :: cout << " port: " << port << std :: endl;

	con_fds[0]=sock_fd;
	con_fds_status[0] = 1;
	return 0;

FAIL:
	std :: cerr << "Failed to open socket" << std :: endl;
	close();
	return -1;
}

int ip_socket::listen()
{
	if (::listen(sock_fd, MAX_CONNECTIONS))
		return -1;
	return 0;
}

int ip_socket::accept(char *ip_addr, unsigned max_len)
{
	if (max_len < INET_ADDRSTRLEN)
		return -1;
	struct sockaddr_in dest_addr;
	struct sockaddr_storage storage;
	socklen_t addrlen = sizeof(storage);

	int fd = ::accept(sock_fd, reinterpret_cast<struct sockaddr *>(&dest_addr), &addrlen);
	if (fd < 0)
		return -1;
	::inet_ntop(AF_INET, &(dest_addr.sin_addr), ip_addr, INET_ADDRSTRLEN);
	return setConFD(fd);
}

int ip_socket::connect(char *dest_ip, unsigned dest_port)
{
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(dest_port);

	if (inet_pton(AF_INET, dest_ip, &(dest_addr.sin_addr)) <= 0)
		return -1;

	if (::connect(sock_fd, reinterpret_cast<const struct sockaddr *>(&dest_addr), sizeof(dest_addr)) < 0)
		return -1;
	return 0;
}

int ip_socket::send(int con_ndx, const void *buf, unsigned len, char *dest_ip, unsigned dest_port)
{
	int con_fd = findConFD(con_ndx);
	if (con_fd < 0)
		return -1;

	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(dest));

	dest.sin_family = AF_INET;
	dest.sin_port = htons(dest_port);

	if ((inet_pton(AF_INET, dest_ip, &dest.sin_addr)) <= 0)
		return -1;
	
	int send_bytes = ::sendto(con_fd, buf, len, 0, reinterpret_cast<const struct sockaddr *>(&dest), sizeof(dest));
	if (send_bytes < 0)
		return -1;
	return send_bytes;
}

int ip_socket::receive(int con_ndx, void *buf, unsigned len, char *dest_ip, unsigned dest_port)
{
	int con_fd = findConFD(con_ndx);
	if (con_fd < 0)
		return -1;

	struct sockaddr_in dest;
	socklen_t addrlen = sizeof(dest);
	memset(&dest, 0, sizeof(dest));

	dest.sin_family = AF_INET;
	dest.sin_port = htons(dest_port);

	if ((inet_pton(AF_INET, dest_ip, &dest.sin_addr)) <= 0)
		return -1;

	int read_bytes = ::recvfrom(con_fd, buf, len, 0, reinterpret_cast<struct sockaddr *>(&dest), &addrlen);
	if (read_bytes < 0)
		return -1;
	return read_bytes;
}


int ip_socket::send(int con_ndx, const void *buf, unsigned len)
{	
	int con_fd = findConFD(con_ndx);
	if (con_fd < 0)
		return -1;
	
	int send_bytes = ::sendto(con_fd, buf, len, 0, NULL, 0);
	if (send_bytes < 0)
		return -1;
	return send_bytes;
}

int ip_socket::receive(int con_ndx, void *buf, unsigned len)
{
	int con_fd = findConFD(con_ndx);
	if (con_fd < 0)
		return -1;
	int read_bytes = ::recvfrom(con_fd, buf, len, 0, NULL, NULL);
	if (read_bytes < 0)
		return -1;
	return read_bytes;
}

int ip_socket::findConFD(int con_ndx)
{
	if (con_ndx >= MAX_CONNECTIONS)
		return -1;
	if (!con_fds_status[con_ndx])
		return -1;
	return con_fds[con_ndx];
}

int client_main(int argc, char *argv[])
{
	if (argc < 2)
		return -1;

	std :: cout << "Client Program" << std :: endl;
	std :: cout << "Client IP Address: 127.0.0.1" << " Port:" << argv[1] << std :: endl;

	unsigned server_port, client_port;
	sscanf(argv[1], "%u", &client_port);

	char server_ip[1024] = "127.0.0.1";
	char message[1024] = "GET / HTTP/1.1\r\nHOST:www.httpbin.org\r\n\r\n";

	ip_socket client;
	client.create(ip_socket::CON_TYPE_UDP, NULL, client_port);

	sscanf(argv[3], "%u", &server_port);

	client.send(0, message, strlen(message), server_ip, server_port);
	int s = client.receive(0, message, sizeof(message), server_ip, server_port);
	message[s] = '\0';

	std :: cout << "Message: " << message << std :: endl;
    return 0;

}

int server_main(int argc, char *argv[])
{
	if (argc < 2)
		return -1;

	std :: cout << "Server Program" << std :: endl;
	std :: cout << "Server IP Address: 127.0.0.1" << " Port:" << argv[1] << std :: endl;

	unsigned server_port, client_port;
	sscanf(argv[1], "%u", &server_port);

	ip_socket server;
	server.create(ip_socket::CON_TYPE_UDP, NULL, server_port);

	char client_ip[1024] = "127.0.0.1";
	char message[1024];

	sscanf(argv[3], "%u", &client_port);

	server.receive(0, message, 1024, client_ip, client_port);
	std :: cout << "Message: " << message << std :: endl;

	strcpy(message, "Hello");
	server.send(0, message, strlen(message), client_ip, client_port);
    return 0;
}

int main(int argc, char *argv[])
{
	#ifdef CLIENT 
		return client_main(argc, argv);
	#else
		return server_main(argc, argv);
	#endif
}
