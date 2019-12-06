#include <iostream>
#include "comm/app/UDPClient.h"
#include "comm/Context.h"

int main()
{
	cys::comm::Context ctx;
	cys::comm::app::UDPClient client(&ctx);
	// client.create();
	client.create(44444);
	client.connect("127.0.0.1", 63910);
	ctx.run();

	for (auto i = 0; i < 100; ++i) {
		client.send(std::to_string(i));
	}	   

	std::this_thread::sleep_for(std::chrono::seconds(5));
	
	client.disconnect();
	client.destroy();
	
	ctx.release();
	return 0;
}
