#ifndef CSENDMAIL_H
#define	CSENDMAIL_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define SLEEPTIME 500

/************ CSendMail类 使用说明 ******************************
************* LoginSMTP      ： 1，先登录
************* SetEnclPath    ： 2，设置附件路径
************* SetTargetEmail ： 3，设置目标邮箱，以及是否发送附件
************* Send           ： 4，发送邮件
*****************************************************************/

class CSendMail
{
public:
    CSendMail();
    ~CSendMail();

    //设置自己发件人的邮箱
    bool SetSMTP(string address, int port);
    //登录
    bool LoginSMTP(string Email, string Password);
    //设置发送目标，邮箱，标题，内容，是否有附件
    bool SetTargetEmail(string Email, string title, string body, bool enclosure = false);
    //设置附件路径
    bool SetEnclPath(vector<string>filename);
    //发送邮件
    bool Send();

protected:
    int  _GetError(int flags = 0);
    bool _SendEnclosure();

private:
    string m_UserEMail;
    string m_PassWord;
    string m_STMPAddress;
    int    m_STMPPort;
    bool   m_Login;
    vector<string> m_Filename;

    /*---------------Socket------------*/
    WSADATA     m_Wsadata;
    sockaddr_in m_STMPAddr;
    SOCKET      m_SMTPSocket;
};


#endif	/* CSENDMAIL_H */
