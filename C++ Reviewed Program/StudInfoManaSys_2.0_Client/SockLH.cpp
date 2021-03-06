#include "SockLH.h"
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma  warning (disable:4996)
CSockLH::CSockLH(void)
{
	WSADATA wd;
	WSAStartup(0x0202,&wd);
	m_hSocket = INVALID_SOCKET;
}


CSockLH::~CSockLH(void)
{
	Close();
}


BOOL CSockLH::Create(UINT nSocketPort, int nSocketType, LPCTSTR lpszSocketAddress)
{
	m_hSocket = socket(AF_INET,nSocketType,0);
	if(m_hSocket == INVALID_SOCKET)
		return false;
	sockaddr_in sa = {AF_INET,htons(nSocketPort)};
	if(lpszSocketAddress)
		sa.sin_addr.S_un.S_addr = inet_addr(lpszSocketAddress);
	return !bind(m_hSocket,(sockaddr*)&sa,sizeof(sa));
}




BOOL CSockLH::Accept(CSockLH& rConnectedSocket, LPSTR szIP, UINT* nPort)
{
	sockaddr_in sa = {AF_INET};
	int nLen = sizeof(sa);
	rConnectedSocket.m_hSocket = accept(m_hSocket,(sockaddr*)&sa,&nLen);
	if(rConnectedSocket.m_hSocket == INVALID_SOCKET)
		return false;
	if(szIP)
		strcpy(szIP,inet_ntoa(sa.sin_addr));
	if(nPort)
		*nPort = htons(sa.sin_port);
	return true;
}


BOOL CSockLH::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	sockaddr_in sa = {AF_INET,ntohs(nHostPort)};
	sa.sin_addr.S_un.S_addr = inet_addr(lpszHostAddress);
	return !connect(m_hSocket,(sockaddr*)&sa,sizeof(sa));
}


int CSockLH::SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
	sockaddr_in sa = {AF_INET,ntohs(nHostPort)};
	sa.sin_addr.S_un.S_addr = inet_addr(lpszHostAddress);
	return sendto(m_hSocket,(char*)lpBuf,nBufLen,0,(sockaddr*)&sa,sizeof(sa));
}


int CSockLH::ReceiveFrom(void* lpBuf, int nBufLen, char* rSocketAddress, UINT& rSocketPort, int nFlags)
{//
	sockaddr_in sa = {AF_INET};
	int nLen = sizeof(sa);
	int nRet =  recvfrom(m_hSocket,(char*)lpBuf,nBufLen,0,(sockaddr*)&sa,&nLen);
	if(nRet > 0)
	{
		rSocketPort = htons(sa.sin_port);
		strcpy(rSocketAddress,inet_ntoa(sa.sin_addr));
	}
	return nRet;
}




BOOL CSockLH::GetPeerName(char* rSocketAddress, UINT& rSocketPort)
{
	sockaddr_in sa = {AF_INET};
	int nLen = sizeof(sa);
	if(getpeername(m_hSocket,(sockaddr*)&sa,&nLen)<0)
		{
			int n =  WSAGetLastError();
			return false;
		}
	strcpy(rSocketAddress,inet_ntoa(sa.sin_addr));	//深拷贝
	/*rSocketAddress = inet_ntoa(sa.sin_addr);//注意此处不能用浅拷贝	*/
	rSocketPort = htons(sa.sin_port);


	return 0;
}


BOOL CSockLH::GetSockName(char* rSocketAddress, UINT& rSocketPort)
{
	sockaddr_in sa = { AF_INET };
	int nLen = sizeof(sa);
	if (getsockname(m_hSocket, (sockaddr*)&sa, &nLen) < 0)
		return FALSE;
	rSocketPort = htons(sa.sin_port);
	strcpy(rSocketAddress, inet_ntoa(sa.sin_addr));
	return TRUE;
	return 0;
}

