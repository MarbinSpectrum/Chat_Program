#include <winsock2.h> 
#include <WS2tcpip.h>
#include <iostream> 
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

string nickName;

#define PACKET_SIZE 1024 

SOCKET skt;

void proc_recv()
{
	char buffer[PACKET_SIZE] = {};
	string cmd;
	while (!WSAGetLastError())
	{
		ZeroMemory(&buffer, PACKET_SIZE);
		recv(skt, buffer, PACKET_SIZE, 0);
		cmd = buffer;
		std::cout << buffer << "\n";
	}
}

int main()
{
	std::cout << "서버에서 사용할 닉네임을 입력해주세요!\n";
	std::cin >> nickName;

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
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	while (1)  
	{ 
		//connect : 원격 호스트에 대한 연결
		if (!connect(skt, (SOCKADDR*)&addr, sizeof(addr)))
		{
			break; 
		}
	}
	thread proc1(proc_recv);

	char buffer[PACKET_SIZE] = { 0 };

	while (!WSAGetLastError())
	{
		//소캣에러가 발생중이지 않을때
		//메세지 입력

		std::cin >> buffer;
		string msg = "";
		msg += nickName;
		msg += " : ";
		msg += buffer;

		send(skt, msg.c_str(), strlen(msg.c_str()), 0);
	}
	proc1.join();

	closesocket(skt); 
	WSACleanup(); 
}
