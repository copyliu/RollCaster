#ifndef LOBBY_STRING_CONV
#define LOBBY_STRING_CONV

#include <windows.h>
#include <process.h>
#include <winsock2.h>
#include <io.h>
#include <iostream.h>
#include <iomanip.h>
#include <ws2tcpip.h>
#include <shlwapi.h>
#include <stdlib.h>

namespace N_Gui{

int hashcmp( WCHAR*, WCHAR* );
int StoU( BYTE*, BYTE*, short* );
int UtoS( BYTE*, BYTE*, short* );
int revInt( int );
LONGLONG revLong( LONGLONG );
short revShort( short );
int convIP( DWORD, WCHAR* );


class myString{
	public :
	myString();
	myString( WCHAR*);
	myString( WCHAR*, short);
	myString( BYTE*, short);
	~myString();
	int fail();
	int set( WCHAR*, short);
	int set( BYTE*, short);
	
	WCHAR* local;
	short localSize;
	
	BYTE* remote;
	short remoteSize;
};

class inputBufferClass{
	public :
	inputBufferClass( void* );
	int readUTF( myString* );
	int read( void*, int, int );
	LONGLONG readLong();
	short readShort();
	BYTE readByte();
	
	BYTE* data;
	int nowData;
};

class outputBufferClass{
	public :
	outputBufferClass( void* , int);
	int writeUTF( myString* );
	int writeShort(short);
	int writeByte(BYTE);
	
	BYTE* data;
	int nowData;
	int bufSize;
};

}

#endif
