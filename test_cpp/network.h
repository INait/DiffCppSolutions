#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdint.h>

int StartServer( const char* sock_addr, uint32_t port, uint32_t backlog );

int DisableServer( uint32_t sockFd );

#endif // _NETWORK_H_
