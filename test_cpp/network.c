#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "network.h"

int StartServer( const char* sock_addr, uint32_t port, uint32_t backlog )
{
	// socket descriptor
	uint32_t tcpSocket;

	// socket information
	struct sockaddr_in sock_info;

	// SOCK_STREAM - tcp socket type
	tcpSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( tcpSocket == -1 )
		return -1; // error 1

	// option value ( to pass to socket options )
	static const uint32_t opt_value = 1;

	// set option reuse address to 1
	if( setsockopt( tcpSocket, SOL_SOCKET, SO_REUSEADDR, & opt_value, sizeof( opt_value ) ) == -1 )
		return -2; // error 2

	// fill socket info structure
	sock_info.sin_family = AF_INET;
	sock_info.sin_port = htons( port );
	sock_info.sin_addr.s_addr = inet_addr( sock_addr );

	// bind socket descriptor with socket info
	if( bind( tcpSocket, (struct sockaddr *) & sock_info, sizeof( sock_info ) ) == -1 )
		return -3; // error 3

	// start listen
	if( listen( tcpSocket, backlog ) == -1 )
		return -4; // error 4

	return tcpSocket;
}

int DisableServer( uint32_t sockFd )
{
	if( sockFd != -1 )
		close( sockFd );

	sockFd = -1;
	return 0;
}
