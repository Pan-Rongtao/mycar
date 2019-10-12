#include "Service.h"
#include "core/Log.h"
#include "core/Runtime.h"
#include <Poco/Util/IniFileConfiguration.h>
#include <Poco/Net/TCPServer.h>
#include <Poco/Net/TCPServerConnectionFactory.h>

using namespace uit;
using namespace Poco::Util;

#define LOG_TAG	"server"
#define DEFAULT_DB_DIR			uit::Runtime::getUitEtcDirectory() + "account.db"
#define CONFIGURE_FILE_PATH		uit::Runtime::getUitEtcDirectory() + "server.conf"

int main(int argc, char **argv)
{
	try {
		Service app;
		app.run(argc, argv);
	}
	catch (RCF::Exception &e)	{ uit::Log::error(LOG_TAG, "a RCF::Exception occur: %s", e.what()); }
	catch (Poco::Exception &e)	{ uit::Log::error(LOG_TAG, "a Poco::Exception occur: %s", e.what()); }
	catch (std::exception &e)	{ uit::Log::error(LOG_TAG, "a std::exception occur: %s", e.what()); }
	catch (...)					{ uit::Log::error(LOG_TAG, "unknown exception"); }
}

int Service::main(const std::vector<std::string>& args)
{
	waitForTerminationRequest();
	return ServerApplication::EXIT_OK;
}

void Service::initialize(Application & app)
{
	ServerApplication::initialize(app);

	auto k = uit::getTickCount();
	if (!DB::instance()->load(DEFAULT_DB_DIR))
	{
		Log::error(LOG_TAG, "load [%s] fail, exit.", DB::instance()->getDBPath().data());
		terminate();
	}

	auto ip = getLocalIp();
	auto interfacePort = 8888;
	auto publisherPort = 9999;
	try {
		Poco::AutoPtr<IniFileConfiguration> ini = new IniFileConfiguration(CONFIGURE_FILE_PATH);
		interfacePort = ini->getInt("InterfacePort", 8888);
		publisherPort = ini->getInt("PublisherPort", 9999);
	}
	catch(...){
		Log::warn(LOG_TAG, "get config fail, user default.");
	}

	RCF::init();
	m_interfaceServer = std::make_shared<RCF::RcfServer>(RCF::TcpEndpoint(ip, interfacePort));
	m_interfaceServer->bind<AccountInterface>(*AccountStub::instance());
	m_interfaceServer->bind<CarInterface>(*CarStub::instance());
	m_interfaceServer->start();

	m_publisherServer = std::make_shared<RCF::RcfServer>(RCF::TcpEndpoint(ip, publisherPort));
	m_publisherAccount = m_publisherServer->createPublisher<AccountNotify>();
	m_publisherCar = m_publisherServer->createPublisher<CarNotify>();
	m_publisherServer->start();

	AccountStub::instance()->AccountChanged.addHandler([&](const AccountStub::AccountChangedArgs &args) {
		m_publisherAccount->publish().onAccountChanged(args.userID, args.info);
	});
	AccountStub::instance()->P2PMessageArrived.addHandler([&](const AccountStub::P2PMessageArrivedArgs &args) {
		m_publisherAccount->publish().onP2PMessageArrived(args.fromID, args.msg);
	});
	AccountStub::instance()->GroupMessageArrived.addHandler([&](const AccountStub::GroupMessageArrivedArgs &args) {
		m_publisherAccount->publish().onGroupMessageArrived(args.groupID, args.msg);
	});
	CarStub::instance()->CarChanged += ([&](const CarStub::CarChangedArgs &args) {
		m_publisherCar->publish().onCarChanged(args.userID, args.info);
	});

	Log::info(LOG_TAG, "server startup, interface[%s:%d], publisher[%s:%d], cost %d ms", ip.data(), interfacePort, ip.data(), publisherPort, (int)(uit::getTickCount() - k));
}

void Service::uninitialize()
{
	ServerApplication::uninitialize();
	m_publisherAccount->close();
	m_publisherCar->close();
	m_interfaceServer->stop();
	m_publisherServer->stop();
	uit::Log::info(LOG_TAG, "service shutdown.");
}

void Service::onAccountChanged(const AccountStub::AccountChangedArgs & args)
{
	m_publisherAccount->publish().onAccountChanged(args.userID, args.info);
}

void Service::onCarChanged(const CarStub::CarChangedArgs & args)
{
	m_publisherCar->publish().onCarChanged(args.userID, args.info);
}

std::string Service::getLocalIp() const
{
#ifdef WIN32
	WSADATA Data;
	auto x = WSAStartup(MAKEWORD(1, 1), &Data);
	char hostName[256] = { 0 };
	gethostname(hostName, sizeof(hostName));
	PHOSTENT hostinfo;
	hostinfo = gethostbyname(hostName);
	std::string ip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
	WSACleanup();
	return ip;
#else
	auto getDeviceIp = [](const std::string &sDev)->std::string {
		char ip[80] = { 0 };
		struct ifreq ifr;
		int sk = socket(AF_INET, SOCK_DGRAM, 0);
		strcpy(ifr.ifr_name, sDev.data());
		if (ioctl(sk, SIOCGIFADDR, &ifr) == 0)
			strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
		close(sk);
		return ip;
	};
#if POCO_ARCH == ARM
	auto eth = getDeviceIp("eth1");
	if(!eth.empty())
		return eth;
	else
		return getDeviceIp("mlan0");
#else
	return getDeviceIp("ens160");
#endif

#endif
}
