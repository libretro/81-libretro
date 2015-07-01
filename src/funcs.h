#ifndef FUNCS_H
#define FUNCS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

SHORT GetKeyState( int nVirtKey );
int   myrandom( int x );
void  DebugUpdate( void );

#define GENERIC_READ 0
#define GENERIC_WRITE 0
#define FILE_SHARE_READ 0
#define FILE_SHARE_WRITE 0
#define OPEN_EXISTING 0
#define INVALID_HANDLE_VALUE (void*)0
#define CreateFile( a, b, c, d, e, f, g ) ( (void)( a ), (void)( b ), (void)( c ), (void)( d ), (void)( e ), (void)( f ), (void)( g ), INVALID_HANDLE_VALUE )

#define IOCTL_DISK_GET_DRIVE_GEOMETRY 0
#define IOCTL_DISK_GET_PARTITION_INFO 0
typedef int* LPOVERLAPPED;
#define DeviceIoControl( a, b, c, d, e, f, g, h ) ( (void)( a ), (void)( b ), (void)( c ), (void)( d ), (void)( e ), (void)( f ), (void)( g ), (void)( h ), 0 )

#define FILE_BEGIN 0
#define INVALID_SET_FILE_POINTER 0
#define SetFilePointer( a, b, c, d ) ( (void)( a ), (void)( b ), (void)( c ), (void)( d ), INVALID_SET_FILE_POINTER )

#define ReadFile( a, b, c, d, e ) ( (void)( a ), (void)( b ), (void)( c ), (void)( d ), (void)( e ), 0 )
#define WriteFile( a, b, c, d, e ) ( (void)( a ), (void)( b ), (void)( c ), (void)( d ), (void)( e ), 0 )
#define CloseHandle( a ) ( (void)( a ), 0 )

#define ERROR_ACCESS_DENIED 0
#define ERROR_GEN_FAILURE 0
#define ERROR_SHARING_VIOLATION 0
#define GetLastError() 0

#define SND_PURGE 0
#define SND_MEMORY 0
#define SND_ASYNC 0
#define SND_NOSTOP 0
#define PlaySound( a, b, c ) ( (void)( a ), (void)( b ), (void)( c ), FALSE )

#ifdef __cplusplus
}
#endif

#endif /* FUNCS_H */
