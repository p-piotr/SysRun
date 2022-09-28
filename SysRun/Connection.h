#ifndef _CONNECTION_H
#define _CONNECTION_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SR_WS2_32DLL_NAME TEXT("ws2_32.dll")
#define SR_WSASTARTUP_NAME "WSAStartup"
#define SR_WSASOCKETW_NAME "WSASocketW"
#define SR_WSACONNECT_NAME "WSAConnect"
#define SR_SETSOCKOPT_NAME "setsockopt"
#define SR_SEND_NAME "send"
#define SR_RECV_NAME "recv"
#define SR_SELECT_NAME "select"

typedef int(*PWSA_STARTUP)(
	WORD wVersionRequired,
	_Out_ LPWSADATA lpWSAData
	);
typedef PWSA_STARTUP LPWSA_STARTUP;

typedef SOCKET(WSAAPI* PWSA_SOCKET_W)(
	_In_ int af,
	_In_ int type,
	_In_ int protocol,
	_In_opt_ LPWSAPROTOCOL_INFOW lpProtocolInfo,
	_In_ GROUP g,
	_In_ DWORD dwFlags
	);
typedef PWSA_SOCKET_W LPWSA_SOCKET_W;

typedef int(WSAAPI* PWSA_CONNECT)(
	_In_ SOCKET s,
	_In_ const struct sockaddr* name,
	_In_ int namelen,
	_In_ LPWSABUF lpCallerData,
	_Out_ LPWSABUF lpCalleeData,
	_In_ LPQOS lpSQOS,
	_In_ LPQOS lpGQOs
	);
typedef PWSA_CONNECT LPWSA_CONNECT;

typedef int(*PSETSOCKOPT)(
	_In_ SOCKET s,
	_In_ int level,
	_In_ int optname,
	_In_ const char* optval,
	_In_ int optlen
	);
typedef PSETSOCKOPT LPSETSOCKOPT;

typedef int(WSAAPI* PSEND)(
	_In_ SOCKET s,
	_In_ const char* buf,
	_In_ int len,
	_In_ int flags
	);
typedef PSEND LPSEND;

typedef int(WSAAPI* PRECV)(
	_In_ SOCKET s,
	_Out_ char* buf,
	_In_ int len,
	_In_ int flags
	);
typedef PRECV LPRECV;

typedef int(WSAAPI* PSELECT)(
	_In_ int nfds,
	_Inout_ fd_set* readfds,
	_Inout_ fd_set* writefds,
	_Inout_ fd_set* exceptfds,
	_In_ const struct timeval* timeout
	);
typedef PSELECT LPSELECT;

typedef struct _CONNECTION_THREAD_DATA
{
	LPWSTR lpWS2_32DllName;
	LPSTR lpWSAStartupName;
	LPSTR lpWSASocketWName;
	LPSTR lpWSAConnectName;
	LPSTR selectName;
	LPWSA_STARTUP WsaStartup; // for internal use only
	LPWSA_SOCKET_W WsaSocketW; // for internal use only
	LPWSA_CONNECT WsaConnect; // for internal use only
	LPSELECT Select; // for internal use only
	struct in_addr ipAddress;
	USHORT port; // network byte order!
} CONNECTION_THREAD_DATA, * PCONNECTION_THREAD_DATA;

#endif // _CONNECTION_H