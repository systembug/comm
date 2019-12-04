#include <iostream>
#include "comm/app/TCPClient.h"
#include "comm/Context.h"

int main()
{
	cys::comm::Context ctx;
	cys::comm::app::TCPClient client(&ctx);
	// client.create();
	client.create(44444);
	client.connect("127.0.0.1", 56671);

	std::string test(10, 'a');

	client.sendAsync(test);

	// std::array<uint8_t, 100> data;
	// client.receive(data);
	client.receiveAsync();
	// client.receiveFrom(data);
	// auto ep = client.receiveFrom(18282, data);
	// client.disconnect(channel);

	ctx.run();
	
	std::this_thread::sleep_for(std::chrono::seconds(5));
	
	client.disconnect();
	client.destroy();
	
	ctx.release();


    std::cout << "Hello World!\n"; 

}
