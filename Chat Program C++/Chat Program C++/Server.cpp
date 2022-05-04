#include <winsock2.h>
#include <iostream>
#include <thread>
#include <set>
#include <map>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define PACKET_SIZE 1024 
SOCKET skt,client_sock;
SOCKADDR_IN client = {};

set<SOCKET> clientSock;
map<SOCKET, thread> clientThread;

void proc_recvs(SOCKET socket)
{
	char buffer[PACKET_SIZE] = { 0 };

	while (!WSAGetLastError())
	{
		ZeroMemory(&buffer, PACKET_SIZE);

		if (!recv(socket, buffer, PACKET_SIZE, 0))
		{
			//메세지를 받을 수 없다고 판단
			break;
		}

		std::cout << buffer << "\n";

		for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
		{
			//다른 클라이언트에게 브로드캐스트
			if (*iter != socket)
			{
				send(*iter, buffer, strlen(buffer), 0);
			}
		}
	}
}
void recv_client()
{
	while (1)
	{
		int client_size = sizeof(client);

		//ZeroMemory : client에 client_size만큼 메모리 할당
		ZeroMemory(&client, client_size);
		client_sock = accept(skt, (SOCKADDR*)&client, &client_size);
		if (clientSock.find(client_sock) == clientSock.end())
		{
			clientSock.insert(client_sock);
			clientThread[client_sock] = thread(proc_recvs, client_sock);
		}
	}

	for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
	{
		clientThread[*iter].join();
	}
}

int main()
{
	//WSADATA 윈도우 소캣 데이터 구조체
	WSADATA wsa;

	//WSAStartup(버전,소캣) : 윈도우 소캣을 초기화 하는 함수
	//MAKEWORD : 매크로함수 (0x0202를 리턴한다)
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//socket(도메인,타입,프로토콜) : 소캣 생성함수
	//PF_INET : IPv4 인터넷 프로토콜을 사용
	//SOCK_STREAM : TCP/IP 프로토콜을 이용
	//IPPROTO_TCP : TCP 프로토콜
	skt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//SOCKADDR_IN : 소캣 주소 구조체
	SOCKADDR_IN addr = {};

	//sin_family : 주소 체계를 구분하기 위한 변수
	//AF_INET : IPv4 인터넷 프로토콜
	//항상 이걸로 설정해야한다.
	addr.sin_family = AF_INET;

	//sin_port : 포트번호를 의미하는 변수
	addr.sin_port = htons(4444);

	//sin_addr : 호스트 IP주소
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind : 소캣에 주소를 할당
	bind(skt, (SOCKADDR*)&addr, sizeof(addr));

	//listen : 소캣을 연결요청 대기 상태로 만든다
	listen(skt, SOMAXCONN);

	thread proc(recv_client);

	char buffer[PACKET_SIZE] = { 0 };
	while (!WSAGetLastError())
	{
		//메세지 전송
		//소캣에러가 발생중이지 않을때
		std::cin >> buffer;
		string msg = "공지 : ";
		msg += buffer;
		for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
		{
			send(*iter, msg.c_str(), strlen(msg.c_str()), 0);
		}
	}

	proc.join();

	for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
	{
		closesocket(*iter);
	}

	closesocket(skt); 
	WSACleanup();
}