#pragma once
#include "core/Def.h"
#include <Poco/SharedPtr.h>
#include <Poco/Data/SQLite/SQLite.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>
#include "core/Event.h"

namespace uit{

class ServerDomain;
class UIT_API AccountStubImpl
{
public:
	AccountStubImpl(ServerDomain *p);

	//ע��һ���˺�
	bool regist(const std::string &userID, const std::string &password, const std::string &nickname);

	//ע��һ���˺�
	void remove(const std::string &userID, const std::string &password);

	//�˺��Ƿ��Ѿ�����
	bool isUserIDExists(const std::string &userID);

	//��¼
	bool login(const std::string &userID, const std::string &password, int terminalType);
	bool logout(const std::string &userID, const std::string & password, int terminalType, bool kickout = false);

	//��ȡ�˺���Ϣ
	bool getAccountInfo(const std::string &userID, std::string &password, std::string &nickname, std::string &signaTure,
		std::string &photo, std::string &registTime, bool &vehicleOnline, bool &pcOnline, bool &handeldOnline, bool &padOnline) const;

	//����
	void setPassword(const std::string &userID, const std::string &password);
	std::string getPassword(const std::string &userID) const;

	//�ǳ�
	void setNickname(const std::string &userID, const std::string &nickname);
	std::string getNickname(const std::string &userID) const;

	//ǩ��
	void setSignaTure(const std::string &userID, const std::string &signaTure);
	std::string getSignaTure(const std::string &userID);

	//ͷ��
	bool setPhoto(const std::string &userID, const std::string &photoBuffer);
	std::string getPhoto(const std::string &userID) const;
	
private:
	std::string terminalTypeToOnlineString(int terminalType) const;
	std::string loadImage(const std::string &path) const;

	ServerDomain	*m_serverDomain;
};

}