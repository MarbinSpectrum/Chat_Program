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
	std::cout << "�������� ����� �г����� �Է����ּ���!\n";
	std::cin >> nickName;

	//WSADATA ������ ��Ĺ ������ ����ü
	WSADATA wsa;

	//WSAStartup(����,��Ĺ) : ������ ��Ĺ�� �ʱ�ȭ �ϴ� �Լ�
	//MAKEWORD : ��ũ���Լ� (0x0202�� �����Ѵ�)
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//socket(������,Ÿ��,��������) : ��Ĺ �����Լ�
	//PF_INET : IPv4 ���ͳ� ���������� ���
	//SOCK_STREAM : TCP/IP ���������� �̿�
	//IPPROTO_TCP : TCP ��������
	skt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	//SOCKADDR_IN : ��Ĺ �ּ� ����ü
	SOCKADDR_IN addr = {};

	//sin_family : �ּ� ü�踦 �����ϱ� ���� ����
	//AF_INET : IPv4 ���ͳ� ��������
	//�׻� �̰ɷ� �����ؾ��Ѵ�.
	addr.sin_family = AF_INET;

	//sin_port : ��Ʈ��ȣ�� �ǹ��ϴ� ����
	addr.sin_port = htons(4444);

	//sin_addr : ȣ��Ʈ IP�ּ�
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	while (1)  
	{ 
		//connect : ���� ȣ��Ʈ�� ���� ����
		if (!connect(skt, (SOCKADDR*)&addr, sizeof(addr)))
		{
			break; 
		}
	}
	thread proc1(proc_recv);

	char buffer[PACKET_SIZE] = { 0 };

	while (!WSAGetLastError())
	{
		//��Ĺ������ �߻������� ������
		//�޼��� �Է�

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
