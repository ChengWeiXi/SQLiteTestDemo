
#include "CSmtp.h"
#pragma comment(lib, "Kernel32.lib")  

int main()
{

	Csmtp mail(
		25,
		"smtp.126.com",
		"xxx@126.com",// ��Դ����
		"xxx",   //����
		"xxx.com" //Ŀ������
	);

	if (!mail.CReateSocket())
	{
		cout << "ReateSocket failed!" << endl;
		return -1;//
	}
	//����Ĭ����������������Ĭ����ip
	mail.setTitle("test mail");
	mail.setContent("this is content.");
	//����·�����󣬲�Ӱ���ʼ����ĵķ��͡�
	mail.addfile("test.jpg"); //��Ӹ���
	//mail.addfile("test2.png"); //��Ӹ���

	mail.SendMail(); //��������
	system("pause");
	return 0;
}

