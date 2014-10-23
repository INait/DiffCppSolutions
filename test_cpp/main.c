#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "network.h"

#define BUF_SIZE 256

void EncodeMsg( unsigned char* msg, int msg_size )
{
	int i = 0;
	for( i = 0; i < msg_size; i++ )
		msg[ i ] = ( msg[ i ] + 3 ) % 256;
}

void DecodeMsg( unsigned char* msg, int msg_size )
{
	int i = 0;
	for( i = 0; i < msg_size; i++ )
		msg[ i ] = ( msg[ i ] - 3 ) % 256;
}

int main()
{
	unsigned char msg[BUF_SIZE];

	FILE* pTestFile;
	FILE* pOutputFile;
	int i, size;

	// server on linux tcp socket
	int tcpSocket = StartServer( "0.0.0.0", 35555, 10000 );

	DisableServer( tcpSocket );

	pTestFile = fopen( "test.msg", "r" );
	if( pTestFile == NULL )
	{
		printf( "error opening file" );
		return -1;
	}


	pOutputFile = fopen( "test.enc", "wb" );
	if( pOutputFile == NULL )
	{
		printf( "error opening file" );
		return -2;
	}

	i = 0;
	size = 0;
	while( !feof( pTestFile ) )
	{
		int read_size = fread( msg, sizeof( unsigned char ), BUF_SIZE, pTestFile );

		i++;
		size += read_size;

		EncodeMsg( msg, read_size );

		fwrite( msg, sizeof( unsigned char ), read_size, pOutputFile );
		printf( "%d piece completed, total size = %d\n", i, size );
	}

	fclose( pTestFile );
	fclose( pOutputFile );

	printf( "====== check read file ============\n" );
	pTestFile = fopen( "test.enc", "r" );
	if( pTestFile == NULL )
	{
		printf( "error opening file" );
		return -1;
	}

	pOutputFile = fopen( "test.dec", "w" );
	if( pOutputFile == NULL )
	{
		printf( "error opening file" );
		return -2;
	}

	i = 0;
	size = 0;
	while( !feof( pTestFile ) )
	{
		int read_size = fread( msg, sizeof( unsigned char ), BUF_SIZE, pTestFile );

		i++;
		size += read_size;

		DecodeMsg( msg, read_size );

		fwrite( msg, sizeof( unsigned char ), read_size, pOutputFile );
		printf( "%d piece completed, total size = %d\n", i, size );
	}

	fclose( pTestFile );
	fclose( pOutputFile );

	return 0;
}
