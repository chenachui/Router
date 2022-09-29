#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define HAVE_REMOTE
#include "hea.h"
#include "pcap.h"
#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"ws2_32.lib")//����ws2_32.lib���ļ�������Ŀ��
#include <stdio.h>
//�궨��
#define PACAP_ERRBUF_SIZE 10
#define MAX_IP_NUM 10
using namespace std;
log1 ltable;//��־
//���߳�
HANDLE hThread;
DWORD dwThreadId;


int index;
int main()
{


	scanf("%d", &index);
	//const char* ��char*��ת��
	pcap_src_if_string = new char[strlen(PCAP_SRC_IF_STRING)];
	strcpy(pcap_src_if_string, PCAP_SRC_IF_STRING);

	//��ȡ����ip
	find_alldevs();

	for (int i = 0; i < 2; i++)
	{
		printf("%s\t", ip[i]);
		printf("%s\n", mask[i]);
	}
	getselfmac(inet_addr(ip[0]));
	getmac(selfmac);
	BYTE mac[6];

	int op;



	routetable rtable;//��ʼ��
	hThread = CreateThread(NULL, NULL, handlerRequest, LPVOID(&rtable), 0, &dwThreadId);


	routeitem a;
	//a.mask = inet_addr("255.255.255.0");
	//a.nextip = inet_addr("15.1.2.2");
	//a.net = inet_addr("15.1.3.0");

	//rtable.add(&a);

	while (1)
	{
		printf("1����·�ɱ��2ɾ��·�ɱ��3��ӡ·�ɱ�: ");
		scanf("%d", &op);
		if (op == 1)
		{
			routeitem a;
			char t[30];
			printf("���������룺");
			scanf("%s", &t);
			a.mask = inet_addr(t);
			printf("������Ŀ�����磺");
			scanf("%s", &t);
			a.net = inet_addr(t);
			printf("��������һ����ַ��");
			scanf("%s", &t);
			a.nextip = inet_addr(t);
			a.type = 1;
			rtable.add(&a);
		}
		else if (op == 2)
		{
			printf("������ɾ�������ţ�");
			int index;
			scanf("%d", &index);
			rtable.remove(index);
		}
		else if (op == 3)
		{
			rtable.print();
		}
		else {
			printf("��Ч������������ѡ��\n");
		}

	}

	routetable table;
	table.print();

	return 0;

}
//��������
void find_alldevs()	//��ȡ�����ϵ�IP
{

	if (pcap_findalldevs_ex(pcap_src_if_string, NULL, &alldevs, errbuf) == -1)//��ȡ�����б�
	{
		printf("%s", "error");
	}
	else
	{
		int i = 0;
		d = alldevs;
		//��ȡ������ӿ��豸��ip��ַ��Ϣ
		for (; d != NULL; d = d->next)
		{
			if (i == index)
			{
				net[i] = d;
				int t = 0;
				for (a = d->addresses; a != nullptr; a = a->next)
				{
					if (((struct sockaddr_in*)a->addr)->sin_family == AF_INET && a->addr)
					{//��ӡip��ַ

						printf("%d ", i);
						printf("%s\t", d->name, d->description);
						printf("%s\t%s\n", "IP��ַ:", inet_ntoa(((struct sockaddr_in*)a->addr)->sin_addr));
						strcpy(ip[t], inet_ntoa(((struct sockaddr_in*)a->addr)->sin_addr));//����һ��ȫ������
						strcpy(mask[t++], inet_ntoa(((struct sockaddr_in*)a->netmask)->sin_addr));

						//int s = 0;

					}
				}
				ahandle = open(d->name);
			}
			i++;
		}
	}
	pcap_freealldevs(alldevs);
}
pcap_t* open(char* name)//������ӿ�
{
	pcap_t* temp = pcap_open(name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 100, NULL, errbuf);
	if (temp == NULL)
		printf("error");
	return temp;
}


int compare(BYTE a[6], BYTE b[6])//�Ƚ���������
{
	int index = 1;
	for (int i = 0; i < 6; i++)
	{
		if (a[i] != b[i])
			index = 0;
	}
	return index;
}

void getselfmac(DWORD ip)//��ȡ�Լ���mac��ַ
{
	memset(selfmac, 0, sizeof(selfmac));
	ARPFrame_t ARPFrame;
	//��APRFrame.FrameHeader.DesMAC����Ϊ�㲥��ַ
	for (int i = 0; i < 6; i++)
		ARPFrame.FrameHeader.DesMAC[i] = 0xff;
	//��APRFrame.FrameHeader.SrcMAC����Ϊ����������MAC��ַ


	ARPFrame.FrameHeader.SrcMAC[0] = 0x0f;
	ARPFrame.FrameHeader.SrcMAC[1] = 0x0f;
	ARPFrame.FrameHeader.SrcMAC[2] = 0x0f;
	ARPFrame.FrameHeader.SrcMAC[3] = 0x0f;
	ARPFrame.FrameHeader.SrcMAC[4] = 0x0f;
	ARPFrame.FrameHeader.SrcMAC[5] = 0x0f;


	ARPFrame.FrameHeader.FrameType = htons(0x806);//֡����ΪARP
	ARPFrame.HardwareType = htons(0x0001);//Ӳ������Ϊ��̫��
	ARPFrame.ProtocolType = htons(0x0800);//Э������ΪIP
	ARPFrame.HLen = 6;//Ӳ����ַ����Ϊ6
	ARPFrame.PLen = 4;//Э���ַ��Ϊ4
	ARPFrame.Operation = htons(0x0001);//����ΪARP����

	//��ARPFrame.SendHa����Ϊ���������ĵ�ַ
	/*for (int i = 0; i < 6; i++)
		ARPFrame.SendHa[i] = 0x66;*/
	ARPFrame.SendHa[0] = 0x0f;
	ARPFrame.SendHa[1] = 0x0f;
	ARPFrame.SendHa[2] = 0x0f;
	ARPFrame.SendHa[3] = 0x0f;
	ARPFrame.SendHa[4] = 0x0f;
	ARPFrame.SendHa[5] = 0x0f;
	//��ARPFrame.SendIP����Ϊ���������ϰ󶨵�IP��ַ
	ARPFrame.SendIP = inet_addr("122.122.122.122");
	//��ARPFrame.RecvHa����Ϊ0
	for (int i = 0; i < 6; i++)
		ARPFrame.RecvHa[i] = 0;
	//��ARPFrame.RecvIP����Ϊ�����IP��ַ
	//ARPFrame.RecvIP = inet_addr(ip[index]);


	ARPFrame.RecvIP = ip;

	u_char* h = (u_char*)&ARPFrame;
	int len = sizeof(ARPFrame_t);


	if (ahandle == nullptr) printf("�����ӿڴ򿪴���\n");
	else
	{
		if (pcap_sendpacket(ahandle, (u_char*)&ARPFrame, sizeof(ARPFrame_t)) != 0)
		{
			//���ʹ�����
			printf("senderror\n");
		}
		else
		{
			//���ͳɹ�
			while (1)
			{
				//printf("send\n");
				pcap_pkthdr* pkt_header;
				const u_char* pkt_data;
				int rtn = pcap_next_ex(ahandle, &pkt_header, &pkt_data);
				//pcap_sendpacket(ahandle, (u_char*)&ARPFrame, sizeof(ARPFrame_t));
				if (rtn == 1)
				{
					ARPFrame_t* IPPacket = (ARPFrame_t*)pkt_data;
					if (ntohs(IPPacket->FrameHeader.FrameType) == 0x806)
					{//���Ŀ��MAC��ַ
						if (!compare(IPPacket->FrameHeader.SrcMAC, ARPFrame.FrameHeader.SrcMAC) && compare(IPPacket->FrameHeader.DesMAC, ARPFrame.FrameHeader.SrcMAC))
						{
							ltable.write2log_arp(IPPacket);
							//���ԴMAC��ַ
							for (int i = 0; i < 6; i++)
							{
								selfmac[i] = IPPacket->FrameHeader.SrcMAC[i];
								//if (flag == 0)
								//printf("%02x.", IPPacket->FrameHeader.SrcMAC[i]);
								//ARPFrame.FrameHeader.SrcMAC[i] = IPPacket->FrameHeader.SrcMAC[i];
								//ARPFrame.SendHa[i] = IPPacket->FrameHeader.SrcMAC[i];
							}
							break;

						}

					}
				}
			}
		}
	}
}




void getothermac(DWORD mask_, DWORD ip_, BYTE mac[])//��ȡip��Ӧ��mac
{
	memset(mac, 0, sizeof(mac));
	ARPFrame_t ARPFrame;
	//��APRFrame.FrameHeader.DesMAC����Ϊ�㲥��ַ
	for (int i = 0; i < 6; i++)
		ARPFrame.FrameHeader.DesMAC[i] = 0xff;
	//��APRFrame.FrameHeader.SrcMAC����Ϊ����������MAC��ַ
	for (int i = 0; i < 6; i++)
	{
		ARPFrame.FrameHeader.SrcMAC[i] = selfmac[i];
		ARPFrame.SendHa[i] = selfmac[i];

	}


	ARPFrame.FrameHeader.FrameType = htons(0x806);//֡����ΪARP
	ARPFrame.HardwareType = htons(0x0001);//Ӳ������Ϊ��̫��
	ARPFrame.ProtocolType = htons(0x0800);//Э������ΪIP
	ARPFrame.HLen = 6;//Ӳ����ַ����Ϊ6
	ARPFrame.PLen = 4;//Э���ַ��Ϊ4
	ARPFrame.Operation = htons(0x0001);//����ΪARP����

	//��ARPFrame.SendHa����Ϊ����������MAC��ַ
	/*for (int i = 0; i < 6; i++)
		ARPFrame.SendHa[i] = 0x66;*/

		//��ARPFrame.SendIP����Ϊ���������ϰ󶨵�IP��ַ
	for (int i = 0; i < 2; i++)
	{
		//printf("---------");
		//ipprint(inet_addr(ip[i]));
		//ipprint(mask_&ip_);
		//ipprint(inet_addr(ip[i])&mask_);
		//printf("%d\n",inet_addr(ip[i])&mask_);
		//printf("%d\n",mask_&ip_);

		if ((mask_ & ip_) == (inet_addr(ip[i]) & mask_))
			ARPFrame.SendIP = inet_addr(ip[i]);
	}

	//ipprint(ARPFrame.SendIP);
	//��ARPFrame.RecvHa����Ϊ0
	for (int i = 0; i < 6; i++)
		ARPFrame.RecvHa[i] = 0;
	//��ARPFrame.RecvIP����Ϊ�����IP��ַ
	//ARPFrame.RecvIP = inet_addr(ip[index]);


	ARPFrame.RecvIP = ip_;

	u_char* h = (u_char*)&ARPFrame;
	int len = sizeof(ARPFrame_t);


	if (ahandle == nullptr) printf("�����ӿڴ򿪴���\n");
	else
	{
		if (pcap_sendpacket(ahandle, (u_char*)&ARPFrame, sizeof(ARPFrame_t)) != 0)
		{
			//���ʹ�����
			printf("senderror\n");
		}
		else
		{
			//���ͳɹ�
			while (1)
			{
				//printf("send\n");
				pcap_pkthdr* pkt_header;
				const u_char* pkt_data;
				int rtn = pcap_next_ex(ahandle, &pkt_header, &pkt_data);
				//pcap_sendpacket(ahandle, (u_char*)&ARPFrame, sizeof(ARPFrame_t));
				if (rtn == 1)
				{
					ARPFrame_t* IPPacket = (ARPFrame_t*)pkt_data;
					if (ntohs(IPPacket->FrameHeader.FrameType) == 0x806)
					{//���Ŀ��MAC��ַ
						if (!compare(IPPacket->FrameHeader.SrcMAC, ARPFrame.FrameHeader.SrcMAC) && compare(IPPacket->FrameHeader.DesMAC, ARPFrame.FrameHeader.SrcMAC) && IPPacket->SendIP == ip_)//&&ip==IPPacket->SendIP
						{
							//ipprint(ip_);
							//ipprint(IPPacket->SendIP);///////////////////

							ltable.write2log_arp(IPPacket);
							//���ԴMAC��ַ
							for (int i = 0; i < 6; i++)
							{
								mac[i] = IPPacket->FrameHeader.SrcMAC[i];
								//if (flag == 0)
								//printf("%02x.", IPPacket->FrameHeader.SrcMAC[i]);
								//ARPFrame.FrameHeader.SrcMAC[i] = IPPacket->FrameHeader.SrcMAC[i];
								//ARPFrame.SendHa[i] = IPPacket->FrameHeader.SrcMAC[i];
							}
							//getmac(mac);
							break;

						}

					}
				}
			}
		}
	}
}




void getothermac(DWORD ip_, BYTE mac[])//��ȡip��Ӧ��mac
{
	memset(mac, 0, sizeof(mac));
	ARPFrame_t ARPFrame;
	//��APRFrame.FrameHeader.DesMAC����Ϊ�㲥��ַ
	for (int i = 0; i < 6; i++)
		ARPFrame.FrameHeader.DesMAC[i] = 0xff;
	//��APRFrame.FrameHeader.SrcMAC����Ϊ����������MAC��ַ
	for (int i = 0; i < 6; i++)
	{
		ARPFrame.FrameHeader.SrcMAC[i] = selfmac[i];
		ARPFrame.SendHa[i] = selfmac[i];

	}


	ARPFrame.FrameHeader.FrameType = htons(0x806);//֡����ΪARP
	ARPFrame.HardwareType = htons(0x0001);//Ӳ������Ϊ��̫��
	ARPFrame.ProtocolType = htons(0x0800);//Э������ΪIP
	ARPFrame.HLen = 6;//Ӳ����ַ����Ϊ6
	ARPFrame.PLen = 4;//Э���ַ��Ϊ4
	ARPFrame.Operation = htons(0x0001);//����ΪARP����

	//��ARPFrame.SendHa����Ϊ����������MAC��ַ
	/*for (int i = 0; i < 6; i++)
		ARPFrame.SendHa[i] = 0x66;*/

		//��ARPFrame.SendIP����Ϊ���������ϰ󶨵�IP��ַ
	ARPFrame.SendIP = inet_addr(ip[0]);
	//ipprint(ARPFrame.SendIP);
	//��ARPFrame.RecvHa����Ϊ0
	for (int i = 0; i < 6; i++)
		ARPFrame.RecvHa[i] = 0;
	//��ARPFrame.RecvIP����Ϊ�����IP��ַ
	//ARPFrame.RecvIP = inet_addr(ip[index]);


	ARPFrame.RecvIP = ip_;

	u_char* h = (u_char*)&ARPFrame;
	int len = sizeof(ARPFrame_t);


	if (ahandle == nullptr) printf("�����ӿڴ򿪴���\n");
	else
	{
		if (pcap_sendpacket(ahandle, (u_char*)&ARPFrame, sizeof(ARPFrame_t)) != 0)
		{
			//���ʹ�����
			printf("senderror\n");
		}
		else
		{
			//���ͳɹ�
			while (1)
			{
				//printf("send\n");
				pcap_pkthdr* pkt_header;
				const u_char* pkt_data;
				int rtn = pcap_next_ex(ahandle, &pkt_header, &pkt_data);
				//pcap_sendpacket(ahandle, (u_char*)&ARPFrame, sizeof(ARPFrame_t));
				if (rtn == 1)
				{
					ARPFrame_t* IPPacket = (ARPFrame_t*)pkt_data;
					if (ntohs(IPPacket->FrameHeader.FrameType) == 0x806)//ARP��Ϣ
					{//���Ŀ��MAC��ַ
						if (!compare(IPPacket->FrameHeader.SrcMAC, ARPFrame.FrameHeader.SrcMAC) && compare(IPPacket->FrameHeader.DesMAC, ARPFrame.FrameHeader.SrcMAC) && IPPacket->SendIP == ip_)//&&ip==IPPacket->SendIP
						{
							//ipprint(ip_);
							//ipprint(IPPacket->SendIP);///////////////////

							ltable.write2log_arp(IPPacket);
							//���ԴMAC��ַ
							for (int i = 0; i < 6; i++)
							{
								mac[i] = IPPacket->FrameHeader.SrcMAC[i];
								//if (flag == 0)
								//printf("%02x.", IPPacket->FrameHeader.SrcMAC[i]);
								//ARPFrame.FrameHeader.SrcMAC[i] = IPPacket->FrameHeader.SrcMAC[i];
								//ARPFrame.SendHa[i] = IPPacket->FrameHeader.SrcMAC[i];
							}
							//getmac(mac);
							break;

						}

					}
				}
			}
		}
	}
}



void getmac(BYTE MAC[])//��ӡmac
{
	printf("MAC��ַΪ�� ");
	for (int i = 0; i < 5; i++)
		printf("%02X-", MAC[i]);
	printf("%02X\n", MAC[5]);
}


void routetable::add(routeitem* a)//·�ɱ�
{
	routeitem* pointer;
	//�ҵ����ʵĵط�
	//Ĭ��·��,һ�����ʼ��ʱ������
	if (!a->type)//ֱ��Ͷ��
	{
		a->nextitem = head->nextitem;
		head->nextitem = a;
		a->type = 0;
	}

	//���������������ɳ������ҵ����ʵ�λ��
	else
	{
		for (pointer = head->nextitem; pointer != tail && pointer->nextitem != tail; pointer = pointer->nextitem)//head�����ݣ�tailû��
		{
			if (a->mask < pointer->mask && a->mask >= pointer->nextitem->mask || pointer->nextitem == tail)
				break;
		}
		a->nextitem = pointer->nextitem;
		pointer->nextitem = a;//���뵽����λ��
		//a->type = 1;
	}
	routeitem* p = head->nextitem;
	for (int i = 0; p != tail; p = p->nextitem, i++)
	{
		p->index = i; 
	}
	num++;
}

void routeitem::printitem()//·�ɱ������ӡ
{
	//inet_ntoa(((struct sockaddr_in*)a->addr)->sin_addr)
	/*pcap_addr a;
	a.netmask = (sockaddr*)&mask;*/

	//index mask net nextip
	in_addr addr;
	printf("%d   ", index);
	addr.s_addr = mask;
	char* pchar = inet_ntoa(addr);
	printf("%s\t", pchar);

	addr.s_addr = net;
	pchar = inet_ntoa(addr);
	printf("%s\t", pchar);

	addr.s_addr = nextip;
	pchar = inet_ntoa(addr);
	printf("%s\t\t", pchar);

	printf("%d\n", type);
}


void routetable::print()//��ӡ
{
	routeitem* p = head->nextitem;
	for (; p != tail; p = p->nextitem)
	{
		p->printitem();
	}
}


routetable::routetable()//��ʼ��������ֱ�����ӵ�����
{
	head = new routeitem;
	tail = new routeitem;
	head->nextitem = tail;
	num = 0;
	for (int i = 0; i < 2; i++)
	{
		routeitem* temp = new routeitem;
		temp->net = (inet_addr(ip[i])) & (inet_addr(mask[i]));
		temp->mask = inet_addr(mask[i]);
		temp->type = 0;
		this->add(temp);
	}
}

void routetable::remove(int index)//ɾ������
{

	for (routeitem* t = head; t->nextitem != tail; t = t->nextitem)
	{
		if (t->nextitem->index == index)
		{
			if (t->nextitem->type == 0)
			{
				printf("�����ɾ��\n");
				return;
			}
			else
			{
				t->nextitem = t->nextitem->nextitem;
				return;
			}
		}
	}
	printf("�޸ñ���\n");
}




//�������ݱ���д����־
int iprecv(pcap_pkthdr* pkt_header, const u_char* pkt_data)
{
	int rtn = pcap_next_ex(ahandle, &pkt_header, &pkt_data);
	return rtn;
}




//���ݱ�ת��,�޸�Դmac��Ŀ��mac
void resend(ICMP_t data, BYTE dmac[])
{
	Data_t* temp = (Data_t*)&data;
	//temp->FrameHeader = data.FrameHeader;
	//temp->IPHeader = data.IPHeader;
	//ltable.write2log_ip(&temp);
	memcpy(temp->FrameHeader.SrcMAC, temp->FrameHeader.DesMAC, 6);
	memcpy(temp->FrameHeader.DesMAC, dmac, 6);
	temp->IPHeader.TTL -= 1;
	if (temp->IPHeader.TTL < 0)return;
	//WORD s0=temp.IPHeader.Checksum;
	//int m=checkchecksum((Data_t*)&temp);
	setchecksum(temp);
	//WORD s1=temp.IPHeader.Checksum;
	//int rtn = ipsend((u_char*)temp);
	//ipprint(temp.IPHeader.SrcIP);
	//ipprint(temp.IPHeader.DstIP);
	int rtn = pcap_sendpacket(ahandle, (const u_char*)temp, 74);
	if (rtn == 0)
		ltable.write2log_ip("ת��", temp);
}


//����·�ɱ���Ӧ����,��������һ����ip��ַ
DWORD routetable::lookup(DWORD ip)
{
	routeitem* t = head->nextitem;
	for (; t != tail; t = t->nextitem)
	{
		if ((t->mask & ip) == t->net)
			return t->nextip;
	}
	//printf("δ�ҵ���Ӧ��ת��ַ");
	return -1;
}






int log1::num = 0;
log1 log1::diary[50] = {};
FILE* log1::fp = nullptr;
log1::log1()
{
	fp = fopen("log.txt", "a+"); //��һ������ǰ���ļ�λ�á�����֮���Ǵ��ļ���ʽ
//fprintf(fp, "This is testing for fprintf...\n");
}

log1::~log1()
{
	fclose(fp);
}


void log1::print()//��ӡ��־
{
	int i;
	if (num > 50)
		i = (num + 1) % 50;
	else i = 0;
	for (; i < num % 50; i++)
	{
		printf("%d ", diary[i].index);
		printf("%s\t ", diary[i].type);
		//printf("%s\n",diary[i].detail);
		if (!strcmp(diary[i].type, "ARP"))
		{
			in_addr addr;
			addr.s_addr = diary[i].arp.ip;
			char* pchar = inet_ntoa(addr);
			printf("%s\t", pchar);
			for (int i = 0; i < 5; i++)
			{
				printf("%02X.", diary[i].arp.mac[i]);
			}
			printf("%02X\n", diary[i].arp.mac[5]);
		}
		else if (!strcmp(diary[i].type, "IP"))
		{
			in_addr addr;
			addr.s_addr = diary[i].ip.sip;
			char* pchar = inet_ntoa(addr);
			printf("ԴIP��%s\t", pchar);
			addr.s_addr = diary[i].ip.dip;
			pchar = inet_ntoa(addr);
			printf("Ŀ��IP��%s\t", pchar);
			printf("ԴMAC: ");
			for (int i = 0; i < 5; i++)
			{
				printf("%02X.", diary[i].ip.smac[i]);
			}
			printf("%02X\t", diary[i].ip.smac[5]);
			printf("Ŀ��MAC: ");
			for (int i = 0; i < 5; i++)
			{
				printf("%02X.", diary[i].ip.dmac[i]);
			}
			printf("%02X\n", diary[i].ip.dmac[5]);
		}
	}
}
void log1::write2log_ip(Data_t* pkt)//ip����
{




	diary[num % 100].index = num++;
	strcpy(diary[num % 100].type, "IP");
	diary[num % 100].ip.sip = pkt->IPHeader.SrcIP;
	diary[num % 100].ip.dip = pkt->IPHeader.DstIP;

	//memcpy(copy,arr,len*sizeof(int)); 
	memcpy(diary[num % 100].ip.smac, pkt->FrameHeader.SrcMAC, 6);
	memcpy(diary[num % 100].ip.dmac, pkt->FrameHeader.DesMAC, 6);
}

void log1::write2log_ip(const char* a, Data_t* pkt)//ip����
{
	fprintf(fp, "IP  ");
	fprintf(fp, a);
	fprintf(fp, "  ");


	in_addr addr;
	addr.s_addr = pkt->IPHeader.SrcIP;
	char* pchar = inet_ntoa(addr);

	fprintf(fp, "ԴIP�� ");
	fprintf(fp, "%s  ", pchar);
	fprintf(fp, "Ŀ��IP�� ");
	addr.s_addr = pkt->IPHeader.DstIP;
	fprintf(fp, "%s  ", pchar);
	fprintf(fp, "ԴMAC�� ");
	for (int i = 0; i < 5; i++)
		fprintf(fp, "%02X-", pkt->FrameHeader.SrcMAC[i]);
	fprintf(fp, "%02X  ", pkt->FrameHeader.SrcMAC[5]);
	fprintf(fp, "Ŀ��MAC�� ");
	for (int i = 0; i < 5; i++)
		fprintf(fp, "%02X-", pkt->FrameHeader.DesMAC[i]);
	fprintf(fp, "%02X\n", pkt->FrameHeader.DesMAC[5]);

}


void log1::write2log_arp(ARPFrame_t* pkt)//arp����
{
	fprintf(fp, "ARP  ");

	in_addr addr;
	addr.s_addr = pkt->SendIP;
	char* pchar = inet_ntoa(addr);
	fprintf(fp, "IP�� ");
	fprintf(fp, "%s  ", pchar);

	fprintf(fp, "MAC�� ");
	for (int i = 0; i < 5; i++)
		fprintf(fp, "%02X-", pkt->SendHa[i]);
	fprintf(fp, "%02X\n", pkt->SendHa[5]);

}

DWORD WINAPI handlerRequest(LPVOID lparam)//�̺߳�������Ϣת����·�ɱ�ϵ�в���ͬʱ����
{
	routetable rtable = *(routetable*)(LPVOID)lparam;
	while (1)
	{
		pcap_pkthdr* pkt_header; const u_char* pkt_data;
		while (1)
		{
			int rtn = pcap_next_ex(ahandle, &pkt_header, &pkt_data);
			if (rtn)break;
		}
		FrameHeader_t* header = (FrameHeader_t*)pkt_data;
		if (compare(header->DesMAC, selfmac))//Ŀ��mac���Լ���mac
		{
			if (ntohs(header->FrameType) == 0x806)//�յ�arp����
			{

			}
			else if (ntohs(header->FrameType) == 0x800)//IP����
			{
				Data_t* data = (Data_t*)pkt_data;
				ltable.write2log_ip("����", data);

				DWORD ip1_ = data->IPHeader.DstIP;
				DWORD ip_ = rtable.lookup(ip1_);//����·�ɱ���Ӧ����
				if (ip_ == -1)continue;//û�ж�Ӧ�Ͷ���

				//bool t=checkchecksum(data);
				if (checkchecksum(data))//У���
					//if (data->IPHeader.DstIP == inet_addr("191.168.124.1") || data->IPHeader.SrcIP == inet_addr("192.168.124.1"))
				{
					if (data->IPHeader.DstIP != inet_addr(ip[0]) && data->IPHeader.DstIP != inet_addr(ip[1]))
					{
						int t1 = compare(data->FrameHeader.DesMAC, broadcast);//�Ƚ���������
						int t2 = compare(data->FrameHeader.SrcMAC, broadcast);
						if (!t1 && !t2)
						{

							ICMP_t* temp_ = (ICMP_t*)pkt_data;
							ICMP_t temp = *temp_;

							//printf("%d",sizeof(temp));

							BYTE mac[6];

							//in_addr addr;
							//addr.s_addr = data->IPHeader.SrcIP;
							//char* pchar = inet_ntoa(addr);
							//printf("S:%s\t", pchar);
							//printf("\n");

							//addr.s_addr = data->IPHeader.DstIP;
							//pchar = inet_ntoa(addr);
							//printf("D:%s\t", pchar);
							//printf("\n");




							//addr.s_addr = ip_;
							//pchar = inet_ntoa(addr);
							//printf("next:%s\t", pchar);
							//printf("\n");

							if (ip_ == 0)//ֱ��Ͷ��
							{
								if (!arptable::lookup(ip1_, mac))
									arptable::insert(ip1_, mac);
								//getmac(mac);
								resend(temp, mac);
							}

							else if (ip_ != -1)
							{
								/*getothermac(inet_addr("255.255.255.0"), ip, mac);
								getmac(mac);*/
								if (!arptable::lookup(ip_, mac))
									arptable::insert(ip_, mac);
								//getmac(mac);
								resend(temp, mac);
							}
							//printf("-------------------------\n");
							//printf("\n");


						}

					}
					else//�����Լ���
					{

					}

				}
			}
		}
	}
}
void ipprint(DWORD ip)//��ӡ
{
	in_addr addr;
	addr.s_addr = ip;
	char* pchar = inet_ntoa(addr);
	printf("%s\t", pchar);
	printf("\n");
}


void setchecksum(Data_t* temp)//����У���
{
	temp->IPHeader.Checksum = 0;
	unsigned int sum = 0;
	WORD* t = (WORD*)&temp->IPHeader;
	for (int i = 0; i < sizeof(IPHeader_t) / 2; i++)
	{
		sum += t[i];
		while (sum >= 0x10000)
		{
			int s = sum >> 16;
			sum -= 0x10000;
			sum += s;
		}
	}
	temp->IPHeader.Checksum = ~sum;
}
bool checkchecksum(Data_t* temp)//����
{
	unsigned int sum = 0;
	WORD* t = (WORD*)&temp->IPHeader;
	for (int i = 0; i < sizeof(IPHeader_t) / 2; i++)
	{
		sum += t[i];
		while (sum >= 0x10000)
		{
			int s = sum >> 16;
			sum -= 0x10000;
			sum += s;
		}
	}
	//printf("%d", (WORD)~temp->IPHeader.Checksum);
	if (sum == 65535)
		return 1;
	return 0;
}




int arptable::num = 0;
void arptable::insert(DWORD ip, BYTE mac[6])//�������
{
	atable[num].ip = ip;
	getothermac(ip, atable[num].mac);
	memcpy(mac, atable[num].mac, 6);
	num++;
}
int arptable::lookup(DWORD ip, BYTE mac[6])//�鿴ARP����
{
	memset(mac, 0, 6);
	for (int i = 0; i < num; i++)
	{
		if (ip == atable[i].ip)
		{
			memcpy(mac, atable[i].mac, 6);
			return 1;
		}
	}
	//δ֪����00
	return 0;
}
