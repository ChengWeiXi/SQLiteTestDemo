//#include "stdafx.h"
#include "CSmtp.h"


static const char base64Char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

CSendMail::CSendMail()
{
    //测试使用QQ邮箱，如有需要后面再更改
    m_STMPAddress = "295849158@qq.com";
    //很多邮箱的默认端口都是25
    m_STMPPort = 25;
    m_Login = false;

}
CSendMail::~CSendMail()
{

}
//base64加密
char* base64Encode(char const* origSigned, unsigned origLength)
{
    unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set  
    if (orig == NULL) return NULL;

    unsigned const numOrig24BitValues = origLength / 3;
    bool havePadding = origLength > numOrig24BitValues * 3;
    bool havePadding2 = origLength == numOrig24BitValues * 3 + 2;
    unsigned const numResultBytes = 4 * (numOrig24BitValues + havePadding);
    char* result = new char[numResultBytes + 3]; // allow for trailing '/0'  

    // Map each full group of 3 input bytes into 4 output base-64 characters:  
    unsigned i;
    for (i = 0; i < numOrig24BitValues; ++i)
    {
        result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
        result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
        result[4 * i + 2] = base64Char[((orig[3 * i + 1] << 2) | (orig[3 * i + 2] >> 6)) & 0x3F];
        result[4 * i + 3] = base64Char[orig[3 * i + 2] & 0x3F];
    }

    // Now, take padding into account.  (Note: i == numOrig24BitValues)  
    if (havePadding)
    {
        result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
        if (havePadding2)
        {
            result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
            result[4 * i + 2] = base64Char[(orig[3 * i + 1] << 2) & 0x3F];
        }
        else
        {
            result[4 * i + 1] = base64Char[((orig[3 * i] & 0x3) << 4) & 0x3F];
            result[4 * i + 2] = '=';
        }
        result[4 * i + 3] = '=';
    }

    result[numResultBytes] = '\0';
    return result;
}

bool CSendMail::SetSMTP(string address, int port)
{
    return true;
}

int CSendMail::_GetError(int flags)
{
    Sleep(SLEEPTIME);
    char buff[10240] = { 0 };//有时候错误返回比较长，多留点空间
    int len = recv(m_SMTPSocket, buff, 10240, 0);
    if (len < 0)return len;
    string data = buff;
    printf("%s\n", buff);
    int iRe = atoi(data.substr(0, 3).c_str());
    return iRe;
}

//gngsftrrdlnebjhh
bool CSendMail::LoginSMTP(string Email, string Password)
{
    if (Email.empty() || Password.empty())return false;
    m_UserEMail = Email;
    m_PassWord = Password;
    if (WSAStartup(MAKEWORD(2, 0), &m_Wsadata) != 0)
    {
        cout << "StartUp WSA Failed" << endl;
        return false;
    }
    //启动socket，准备Telnet到Smtp
    memset(&m_STMPAddr, 0, sizeof(m_STMPAddr));
    m_SMTPSocket = socket(PF_INET, SOCK_STREAM, 0);

    m_STMPAddr.sin_family = AF_INET;
    m_STMPAddr.sin_port = htons(m_STMPPort);

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;

    //获取smtp.qq.com服务器域名所代表的IP地址 
    int dwRetval = getaddrinfo("smtp.qq.com", 0, &hints, &result);
    if (dwRetval != 0)
    {
        // 报错信息 xxxx;
    }
    else
    {
        for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next)
        {
            if (ptr->ai_family == AF_INET)
            {
                struct sockaddr_in* sinp = (struct sockaddr_in*)ptr->ai_addr;
                m_STMPAddr.sin_addr = sinp->sin_addr;
            }

        }
    }

    //memcpy(&m_STMPAddr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
    //printf("IP of smpt.QQ.com is : %d:%d:%d:%d", m_STMPAddr.sin_addr.S_un.S_un_b.s_b1, m_STMPAddr.sin_addr.S_un.S_un_b.s_b2, m_STMPAddr.sin_addr.S_un.S_un_b.s_b3, m_STMPAddr.sin_addr.S_un.S_un_b.s_b4);

    //链接
    if (connect(m_SMTPSocket, (sockaddr*)&m_STMPAddr, sizeof(m_STMPAddr)) == SOCKET_ERROR)
    {
        cout << "connect failed!" << endl;
        m_Login = false;
        return m_Login;
    }
    //设置接收超时等待
    int timeout = 200;
    int ret = setsockopt(m_SMTPSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int));
    if (ret != 0)
    {
        cout << "Set Failed" << endl;
    }
    //返回
    if (_GetError() != 220)
    {
        //cout << "connect failed!" << endl;
    }
    //链接成功，发送helo指令，准备登录
    char bufferHello[] = "helo QQMail.com\r\n";
    send(m_SMTPSocket, bufferHello, strlen(bufferHello), 0);
    if (_GetError() != 250)
    {
        cout << "helo failed!" << endl;
        m_Login = false;
        return m_Login;
    }
    Sleep(SLEEPTIME);
    //准备登录，输入用户邮箱名，全称
    char bufferLogin[] = "auth login\r\n";
    send(m_SMTPSocket, bufferLogin, strlen(bufferLogin), 0);
    if (_GetError() != 334)
    {
        cout << "login failed!" << endl;
        m_Login = false;
        return m_Login;
    }
    Sleep(SLEEPTIME);
    //输入用户名
    char user[1024];
    memset(user, NULL, sizeof(char) * 1024);
    string bufferUserName = base64Encode(m_UserEMail.c_str(), m_UserEMail.length());
    bufferUserName += "\r\n";
    strcpy_s(user, bufferUserName.c_str());
    send(m_SMTPSocket, user, strlen(user), 0);
    if (_GetError() != 334)
    {
        cout << "UserName Error!" << endl;
        m_Login = false;
        return m_Login;
    }
    Sleep(SLEEPTIME);
    //输入密码
    string bufferpsw = base64Encode(m_PassWord.c_str(), m_PassWord.length());
    bufferpsw += "\r\n";
    send(m_SMTPSocket, bufferpsw.c_str(), strlen(bufferpsw.c_str()), 0);
    if (_GetError() != 235)
    {
        cout << "PassWord Error!" << endl;
        m_Login = false;
        return m_Login;
    }
    m_Login = true;
    return m_Login;
}
bool CSendMail::SetEnclPath(vector<string>filename)
{
    m_Filename.clear();
    for (unsigned int i = 0; i < filename.size(); i++)
    {
        m_Filename.push_back(filename.at(i));
    }
    return true;
}
bool CSendMail::SetTargetEmail(string Email, string title, string body, bool enclosure)
{
    if (!m_Login)return false;
    string bufferMailFrom = "mail from:<" + m_UserEMail + ">\r\n";
    send(m_SMTPSocket, bufferMailFrom.c_str(), strlen(bufferMailFrom.c_str()), 0);
    if (_GetError() != 250)
    {
        cout << "SetTargetEmail failed!" << endl;
        return false;
    }
    Sleep(SLEEPTIME);
    string bufferRcptTo = "rcpt to:<" + Email + ">\r\n";
    send(m_SMTPSocket, bufferRcptTo.c_str(), strlen(bufferRcptTo.c_str()), 0);
    if (_GetError() != 250)
    {
        cout << "SetTargetEmail failed!" << endl;
        return false;
    }
    Sleep(SLEEPTIME);
    // data
    char bufferData[] = "DATA\r\n";
    cout << "data..." << endl;
    send(m_SMTPSocket, bufferData, strlen(bufferData), 0);
    if (_GetError(1) != 354)
    {
        send(m_SMTPSocket, bufferData, strlen(bufferData), 0);
    }
    Sleep(SLEEPTIME);
    string message = "from:" + m_UserEMail + "\r\n";
    message += "to:" + Email + "\r\n";
    message += "subject:" + title + "\r\n";
    message += "MIME-Version: 1.0\r\n";
    message += "Content-Type: multipart/mixed;boundary=@boundary@\r\n\r\n";
    send(m_SMTPSocket, message.c_str(), message.length(), 0);
    Sleep(SLEEPTIME);
    //  正文 
    string content = body;
    message = "--@boundary@\r\nContent-Type: text/plain;charset=\"gb2312\"\r\n\r\n" + content + "\r\n\r\n";
    send(m_SMTPSocket, message.c_str(), message.length(), 0);
    Sleep(SLEEPTIME);
    if (enclosure)
    {
        _SendEnclosure();
    }
    return true;
}
bool CSendMail::_SendEnclosure()
{
    if (!m_Login)return false;
    if (m_Filename.empty())
    {
        cout << "附件添加失败，文件路径为空！！" << endl;
        return false;
    }
    for (unsigned int i = 0; i < m_Filename.size(); i++)
    {
        ifstream ifs(m_Filename.at(i), ios::in | ios::binary);
        if (false == ifs.is_open())
        {
            cout << "无法打开文件！" << endl;
            return false;
        }
        char  m_cSendBuff[4096] = {0};//发送缓冲区

        sprintf_s(m_cSendBuff, "--@boundary@\r\nContent-Type: application/octet-stream;\r\n  name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;\r\n  filename=\"%s\"\r\n\r\n", m_Filename.at(i).c_str(), m_Filename.at(i).c_str());
        send(m_SMTPSocket, m_cSendBuff, strlen(m_cSendBuff), 0);

        // get length of file:
        ifs.seekg(0, ifs.end);
        int length = (int)ifs.tellg();
        ifs.seekg(0, ifs.beg);
        cout << "length:" << length << endl;
        // allocate memory:
        char* buffer = new char[length];
        // read data as a block:
        ifs.read(buffer, length);
        ifs.close();
        char* pbase = base64Encode(buffer, length);
        delete[]buffer;
        string str(pbase);
        delete[]pbase;
        str += "\r\n";
        int err = send(m_SMTPSocket, str.c_str(), strlen(str.c_str()), 0);
        if (err != strlen(str.c_str()))
        {
            cout << "附件发送出错!" << endl;
            return false;
        }
    }
    return true;
}
bool CSendMail::Send()
{
    if (!m_Login)return false;
    char bufferPoint[] = "--@boundary@--End--\r\n.\r\n";
    send(m_SMTPSocket, bufferPoint, strlen(bufferPoint), 0);
    if (_GetError(1) != 250)
    {
        cout << "Send failed!" << endl;
        return false;
    }
    return true;
}