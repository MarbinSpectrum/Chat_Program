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
			//�޼����� ���� �� ���ٰ� �Ǵ�
			break;
		}

		std::cout << buffer << "\n";

		for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
		{
			//�ٸ� Ŭ���̾�Ʈ���� ��ε�ĳ��Ʈ
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

		//ZeroMemory : client�� client_size��ŭ �޸� �Ҵ�
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
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind : ��Ĺ�� �ּҸ� �Ҵ�
	bind(skt, (SOCKADDR*)&addr, sizeof(addr));

	//listen : ��Ĺ�� �����û ��� ���·� �����
	listen(skt, SOMAXCONN);

	thread proc(recv_client);

	char buffer[PACKET_SIZE] = { 0 };
	while (!WSAGetLastError())
	{
		//�޼��� ����
		//��Ĺ������ �߻������� ������
		std::cin >> buffer;
		string msg = "���� : ";
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