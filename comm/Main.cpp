#include <iostream>
#include <regex>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>  
#include <boost/iostreams/device/back_inserter.hpp>
#include "comm/app/UDPServer.h"
#include "comm/app/UDPServerListener.h"
#include "comm/app/TCPClient.h"
#include "comm/Context.h"
#include "ViewerData.h"

class Listener : public cys::comm::app::UDPServerListener {
private:
	// std::string testStr;
	std::vector<uint8_t> testStr;

public:
	Listener()
		: cys::comm::app::UDPServerListener()
	{}

	void onUDPServerBinded() override {

	}
	void onUDPServerUnBinded() override {

	}
	void onUDPServerSent(const boost::system::error_code& e) override {

	}
	void onUDPServerSentTo(const boost::system::error_code& e) override {

	}
	void onUDPServerReceived(const boost::system::error_code& e, 
		const std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM>& data) override {
		int16_t* cnt = reinterpret_cast<int16_t*>(const_cast<uint8_t*>(&data[0]));		
		// testStr.append(std::string(data.begin() + 2, data.end()));	
		std::cout << *cnt << std::endl;
		if (*cnt == -99) {
			SPLidar::ViewerData test;
			{
				// std::stringstream ss(testStr);
				std::stringstream ss;
				std::copy(testStr.begin(), testStr.end(), std::ostream_iterator<int8_t>(ss));
				try {
					// boost::archive::text_iarchive ia(ss);
					boost::archive::binary_iarchive ia(ss);
					ia >> test;
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
					testStr.clear();
					return;
				}
			}
			testStr.clear();
		}
		else
			std::copy(data.begin() + 2, data.end(), std::back_inserter(testStr));
	}
};

int main()
{
	/*
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	// client.create();
	server.create(60001);
	server.bind();
	ctx.run();

	Listener listener;
	server.addListener(&listener);

	std::this_thread::sleep_for(std::chrono::seconds(1000));
	
	server.unBind();
	server.destroy();
	
	ctx.release();
	return 0;
	*/

	cys::comm::Context ctx;
	cys::comm::app::TCPClient client(&ctx);
	client.create();
	ctx.run();

	client.connect("127.0.0.1", 53000);
	client.disconnect();
	client.connect("127.0.0.1", 53000);
	client.disconnect();

	ctx.release();
	return 0;
}
