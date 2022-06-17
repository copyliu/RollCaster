#ifndef UNICODE
#define UNICODE
#endif

#include "unicodeStringConv.h"
#include "guiIO.h"
using namespace std;
using namespace N_Gui;


/********************************************************
 * UTF Input Buffer
 ********************************************************/
inputBufferClass::inputBufferClass( void* dataTemp ){
	data = (BYTE*)dataTemp;
	nowData = 0;
}

int inputBufferClass::readUTF( myString* str ){
	short size = revShort( *(short*)&data[ nowData ] );
	nowData += 2;
	
	if( str ){
		str->set( &data[ nowData ], size);
	}
	nowData += size;
	
	if( str && str->fail() ) return 1;
	return 0;
}

int inputBufferClass::read( void* buf, int dummy, int len ){
	if( !buf ) return 1;
	if( dummy ){}
	memcpy( buf, &data[ nowData ], len );
	
	nowData += len;
	return 0;
}

LONGLONG inputBufferClass::readLong(){
	LONGLONG ret = revLong( *(LONGLONG*)&data[ nowData ] );
	
	nowData += 8;
	return ret;
}

short inputBufferClass::readShort(){
	short ret = revShort( *(short*)&data[ nowData ] );
	
	nowData += 2;
	return ret;
}

BYTE inputBufferClass::readByte(){
	BYTE ret = data[ nowData ];
	
	nowData += 1;
	return ret;
}



/********************************************************
 * UTF Output Buffer
 ********************************************************/
outputBufferClass::outputBufferClass( void* dataTemp, int size ){
	data = (BYTE*)dataTemp;
	nowData = 0;
	bufSize = size;
}

int outputBufferClass::writeUTF( myString* str ){
	if(nowData + 2 > bufSize) return 1;
	short size = str->remoteSize;
	*(short*)&data[ nowData ] = revShort( size );
	nowData += 2;
	
	if(nowData + size > bufSize) return 1;
	memcpy(&data[ nowData ], str->remote, size);
	nowData += size;
	
	return 0;
}

/*
int outputBufferClass::writeUTF( WCHAR* str ){
	if(nowData + 2 > bufSize) return 1;
	short size = wcslen(str);
	*(short*)&data[ nowData ] = revShort( size );
	nowData += 2;
	
	if(nowData + size > bufSize) return 1;
	memcpy(&data[ nowData ], str, size);
	nowData += size;
	
	return 0;
}
*/

int outputBufferClass::writeShort(short shortVal){
	if(nowData + 2 > bufSize) return 1;
	*(short*)&data[ nowData ] = revShort(shortVal);
	nowData += 2;
	
	return 0;
}

int outputBufferClass::writeByte(BYTE byteVal){
	if(nowData + 1 > bufSize) return 1;
	data[ nowData ] = byteVal;
	nowData += 1;
	
	return 0;
}

/********************************************************
 * UTF Convert String Class
 ********************************************************/
int myString::set( WCHAR* baseString, short baseStringSize ){
	if( local ){
		delete[] local;
		local = NULL;
		localSize = 0;
	}
	if( remote ){
		delete[] remote;
		remote = NULL;
		remoteSize = 0;
	}
	
	if( !baseString ){
		return 1;
	}
	
	if( !baseStringSize ){
		//サイズ0指定は終端保証文字列として扱う
		baseStringSize = wcslen( baseString );
	}
	
	if(baseStringSize == 0){
		remote = new BYTE[4];
		local = new WCHAR[1];
		
		*(DWORD*)&remote[0] = 0;
		local[0] = 0;
	}else{
		//WCHAR
		local = new WCHAR[ baseStringSize + 1 ];
		memcpy( local, baseString, (baseStringSize + 1) * sizeof(WCHAR) );
		local[ baseStringSize ] = 0;
		localSize = baseStringSize;
		
		remoteSize = WideCharToMultiByte( CP_UTF8, 0, local, baseStringSize, NULL, 0, NULL, NULL );
		remote = new BYTE[ remoteSize + 1 ];
		int ret = WideCharToMultiByte( CP_UTF8, 0, local, baseStringSize, (LPSTR)remote, remoteSize + 1, NULL, NULL );
		remote[remoteSize] = 0;
		if(ret == 0 || ret != remoteSize){
			return 1;
		}
	}
	
	return 0;
}

int myString::set( BYTE* baseString, short baseStringSize ){
	if( local ){
		delete[] local;
		local = NULL;
		localSize = 0;
	}
	if( remote ){
		delete[] remote;
		remote = NULL;
		remoteSize = 0;
	}
	
	if(!baseString){
		return 1;
	}
	if(baseStringSize == 0){
		remote = new BYTE[4];
		local = new WCHAR[1];
		
		*(DWORD*)&remote[0] = 0;
		local[0] = 0;
	}else{
		//UTF-8
		remote = new BYTE[ baseStringSize + 1 ];
		memcpy( remote, baseString, baseStringSize );
		remote[ baseStringSize ] = 0;
		remoteSize = baseStringSize;
		
		localSize = MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)remote, baseStringSize, NULL, 0 );
		local = new WCHAR[ localSize + 1 ];
		int ret = MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)remote, baseStringSize, local, (localSize + 1) * sizeof(WCHAR) );
		local[localSize] = 0;
		if(ret == 0 || ret != localSize){
			return 1;
		}
	}
	
	return 0;
}

myString::myString( void ){
	local = NULL;
	remote = NULL;
	
	localSize = 0;
	remoteSize = 0;
}

myString::myString( WCHAR* baseString){
	local = NULL;
	remote = NULL;
	
	localSize = 0;
	remoteSize = 0;
	
	set(baseString, 0);
}

myString::myString( WCHAR* baseString, short baseStringSize){
	local = NULL;
	remote = NULL;
	
	localSize = 0;
	remoteSize = 0;
	
	set(baseString, baseStringSize);
}

myString::myString( BYTE* baseString, short baseStringSize){
	local = NULL;
	remote = NULL;
	
	localSize = 0;
	remoteSize = 0;
	
	set(baseString, baseStringSize);
}

myString::~myString(){
	if( local ) delete[] local;
	if( remote ) delete[] remote;
}

int myString::fail(){
	if( !local || !remote ) return 1;
//	if( !localSize || !remoteSize ) return 1;
	
	return 0;
}

/********************************************************
 * 
 ********************************************************/
LONGLONG N_Gui::revLong( LONGLONG Target ){
	LONGLONG Ret = 0;
	BYTE* pTarget = (BYTE*)&Target;
	BYTE* pRet    = (BYTE*)&Ret;
	
	pRet[0] = pTarget[7];
	pRet[1] = pTarget[6];
	pRet[2] = pTarget[5];
	pRet[3] = pTarget[4];
	pRet[4] = pTarget[3];
	pRet[5] = pTarget[2];
	pRet[6] = pTarget[1];
	pRet[7] = pTarget[0];
	
	return Ret;
}

int N_Gui::revInt( int Target ){
	int Ret = 0;
	BYTE* pTarget = (BYTE*)&Target;
	BYTE* pRet    = (BYTE*)&Ret;
	
	pRet[0] = pTarget[3];
	pRet[1] = pTarget[2];
	pRet[2] = pTarget[1];
	pRet[3] = pTarget[0];
	
	return Ret;
}

short N_Gui::revShort( short Target ){
	short Ret = 0;
	BYTE* pTarget = (BYTE*)&Target;
	BYTE* pRet    = (BYTE*)&Ret;
	
	pRet[0] = pTarget[1];
	pRet[1] = pTarget[0];
	
	return Ret;
}


/********************************************************
 * Compare UID
 *
 * If param is NULL, treat as 0
 ********************************************************/
int N_Gui::hashcmp( WCHAR* src, WCHAR* target ){
	int Counter;
	
	if( !src && ! target ){
		return 1;
	}
	if( !src ){
		for( Counter = 0; Counter < 32; Counter++ ){
			if( 0 != target[ Counter ] ){
				return 1;
			}
		}
		return 0;
	}
	if( !target ){
		for( Counter = 0; Counter < 32; Counter++ ){
			if( 0 != src[ Counter ] ){
				return 1;
			}
		}
		return 0;
	}
	
	for( Counter = 0; Counter < 32; Counter++ ){
		if( src[ Counter ] != target[ Counter ] ){
			return 1;
		}
	}
	return 0;
}

/********************************************************
 * Convert DWORD to IP str
 ********************************************************/
int N_Gui::convIP( DWORD dwIP, WCHAR* IP ){
	if( !IP ) return 1;
	
	BYTE* pIP = (BYTE*)&dwIP;
	IP[0] = 0;
	WCHAR temp[ 10 ];
	memset( temp, 0, sizeof(temp) );
	
	_itow( pIP[0], temp, 10 );
	wcscat( IP, temp );
	memset( temp, 0, sizeof(temp) );
	
	wcscat( IP, L"." );
	_itow( pIP[1], temp, 10 );
	wcscat( IP, temp );
	memset( temp, 0, sizeof(temp) );
	
	wcscat( IP, L"." );
	_itow( pIP[2], temp, 10 );
	wcscat( IP, temp );
	memset( temp, 0, sizeof(temp) );
	
	wcscat( IP, L"." );
	_itow( pIP[3], temp, 10 );
	wcscat( IP, temp );
	memset( temp, 0, sizeof(temp) );
	
	return 0;
}

