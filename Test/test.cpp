#include "pch.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "../comm/Context.h"
#include "../comm/app/UDPServer.h"
#include "../comm/app/UDPClient.h"
#include "../comm/app/Const.h"

TEST(Context, ActivateTest) {
	cys::comm::Context ctx;
	EXPECT_EQ(ctx.run(), true);
	EXPECT_EQ(ctx.release(), true);
}

TEST(Context, InvalidThreadNumTest) {
	cys::comm::Context ctx(0);
	EXPECT_EQ(ctx.run(), false);
	EXPECT_EQ(ctx.release(), true);
}

TEST(UDP, ContextIndependencyTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	EXPECT_EQ(ctx.run(), true);
	EXPECT_EQ(ctx.release(), true);	
}

TEST(UDP, CreateValidTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);
	EXPECT_EQ(server.create(18282), true);
	EXPECT_EQ(client.create(28282), true);
	EXPECT_EQ(server.destroy(), true);
	EXPECT_EQ(client.destroy(), true);
}

TEST(UDP, SamePortExceptionTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);
	server.create(18282);	
	EXPECT_EQ(client.create(18282), false);
	server.destroy();
	client.destroy();
}

TEST(UDP, ConnectTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);
	
	server.create(18282);
	server.bind();
	client.create(28282);
	ctx.run();
	EXPECT_EQ(client.connect("127.0.0.1", 18282), true);
	client.destroy();
	server.destroy();
	ctx.release();
}

class UDPClientListenerMock : public cys::comm::app::UDPClientListener {
public:
	MOCK_METHOD0(onUDPClientConnected, void());
	MOCK_METHOD1(onUDPClientDisconnected, void(const boost::system::error_code& e));
	MOCK_METHOD1(onUDPClientSent, void(const boost::system::error_code& e));
	MOCK_METHOD1(onUDPClientSentTo, void(const boost::system::error_code& e));
	MOCK_METHOD2(onUDPClientReceived, void(const boost::system::error_code& e, 
		const std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM>& data));
	MOCK_METHOD1(onUDPClientError, void(const boost::system::error_code& e));
};

TEST(UDP, ConnectAsyncTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPClientListenerMock mock;

	server.create(18282);
	server.bind();
	client.addListener(&mock);
	client.create(28282);
	EXPECT_CALL(mock, onUDPClientConnected());
	client.connectAsync("127.0.0.1", 18282);
	ctx.run();

	EXPECT_CALL(mock, onUDPClientDisconnected(::testing::_));
	client.disconnect();
	server.destroy();
	client.destroy();
	ctx.release();
}

class UDPServerListenerMock : public cys::comm::app::UDPServerListener {
public:
	MOCK_METHOD0(onUDPServerBinded, void());
	MOCK_METHOD0(onUDPServerUnBinded, void());
	MOCK_METHOD1(onUDPServerSent, void(const boost::system::error_code& e));
	MOCK_METHOD1(onUDPServerSentTo, void(const boost::system::error_code& e));
	MOCK_METHOD2(onUDPServerReceived, void(const boost::system::error_code& e,
		const std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM>& data));
};

TEST(UDP, ListenerTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);
	
	UDPServerListenerMock sMock;
	UDPClientListenerMock cMock;

	EXPECT_EQ(server.addListener(&sMock), true);
	EXPECT_EQ(client.addListener(&cMock), true);
	EXPECT_EQ(server.addListener(&sMock), false);
	EXPECT_EQ(client.addListener(&cMock), false);
	EXPECT_EQ(server.deleteListener(&sMock), true);
	EXPECT_EQ(client.deleteListener(&cMock), true);
	EXPECT_EQ(server.deleteListener(&sMock), false);
	EXPECT_EQ(client.deleteListener(&cMock), false);
}

TEST(UDP, BindTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPServerListenerMock mock;

	server.create(18282);
	server.addListener(&mock);
	EXPECT_CALL(mock, onUDPServerBinded());
	server.bind();
	ctx.run();

	EXPECT_CALL(mock, onUDPServerUnBinded());
	server.destroy();
	ctx.release();
}

TEST(UDP, SendTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPServerListenerMock mock;
	
	server.create(18282);
	server.addListener(&mock);
	EXPECT_CALL(mock, onUDPServerBinded());
	server.bind();
	client.create(28282);
	client.connect("127.0.0.1", 18282);
	ctx.run();
	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> arr = { "TEST" };
	EXPECT_CALL(mock, onUDPServerReceived(::testing::_, arr));
	EXPECT_CALL(mock, onUDPServerUnBinded());
	EXPECT_EQ(client.send("TEST"), true);

	client.disconnect();
	client.destroy();
	server.destroy();
	ctx.release();
}

TEST(UDP, SendTestWithStreamBuf) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPServerListenerMock mock;

	server.create(18282);
	server.addListener(&mock);
	EXPECT_CALL(mock, onUDPServerBinded());
	server.bind();
	client.create(28282);
	client.connect("127.0.0.1", 18282);
	ctx.run();
	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> arr = { "TEST" };
	boost::asio::streambuf buf;
	std::ostream os(&buf);
	os << "TEST";
	EXPECT_CALL(mock, onUDPServerReceived(::testing::_, arr));
	EXPECT_CALL(mock, onUDPServerUnBinded());
	EXPECT_EQ(client.send(buf), true);

	client.disconnect();
	client.destroy();
	server.destroy();
	ctx.release();
}

TEST(UDP, SendAsyncTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPServerListenerMock sMock;
	UDPClientListenerMock cMock;

	server.create(18282);
	server.addListener(&sMock);
	EXPECT_CALL(sMock, onUDPServerBinded());
	server.bind();
	client.create(28282);
	client.addListener(&cMock);
	EXPECT_CALL(cMock, onUDPClientConnected());
	client.connect("127.0.0.1", 18282);
	ctx.run();
	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> arr = { "TEST" };
	EXPECT_CALL(sMock, onUDPServerReceived(::testing::_, arr));
	EXPECT_CALL(cMock, onUDPClientSent(::testing::_));
	EXPECT_EQ(client.sendAsync("TEST"), true);
	
	EXPECT_CALL(cMock, onUDPClientDisconnected(::testing::_));
	EXPECT_CALL(sMock, onUDPServerUnBinded());

	client.disconnect();
	client.destroy();
	server.destroy();
	ctx.release();
}

TEST(UDP, SendToTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPServerListenerMock mock;

	server.create(18282);
	server.addListener(&mock);
	EXPECT_CALL(mock, onUDPServerBinded());
	server.bind();
	client.create(28282);
	ctx.run();
	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> arr = { "TEST" };
	EXPECT_CALL(mock, onUDPServerReceived(::testing::_, arr));
	EXPECT_CALL(mock, onUDPServerUnBinded());
	EXPECT_EQ(client.sendTo("127.0.0.1", 18282, arr), true);

	client.destroy();
	server.destroy();
	ctx.release();
}

TEST(UDP, SendToAsyncTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPServerListenerMock sMock;
	UDPClientListenerMock cMock;

	server.create(18282);
	server.addListener(&sMock);
	EXPECT_CALL(sMock, onUDPServerBinded());
	server.bind();
	client.create(28282);
	client.addListener(&cMock);
	ctx.run();
	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> arr = { "TEST" };
	EXPECT_CALL(sMock, onUDPServerReceived(::testing::_, arr));
	EXPECT_CALL(cMock, onUDPClientSentTo(::testing::_));
	EXPECT_EQ(client.sendToAsync("127.0.0.1", 18282, arr), true);

	client.destroy();

	EXPECT_CALL(sMock, onUDPServerUnBinded());
	server.destroy();
	ctx.release();
}


TEST(UDP, SendBroadCastTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPServerListenerMock mock;

	server.create(18282);
	server.addListener(&mock);
	EXPECT_CALL(mock, onUDPServerBinded());
	server.bind();
	client.create(28282);
	ctx.run();
	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> arr = { "TEST" };
	EXPECT_CALL(mock, onUDPServerReceived(::testing::_, arr));
	EXPECT_CALL(mock, onUDPServerUnBinded());
	EXPECT_EQ(client.sendToBroadCast(18282, arr), true);

	client.destroy();
	server.destroy();
	ctx.release();
}

TEST(UDP, SendBroadFailTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPServerListenerMock mock;

	server.create(18282);
	server.addListener(&mock);
	EXPECT_CALL(mock, onUDPServerBinded());
	server.bind();
	client.create(28282);
	client.connect("127.0.0.1", 18282);
	ctx.run();
	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> arr = { "TEST" };
	EXPECT_CALL(mock, onUDPServerUnBinded());
	EXPECT_EQ(client.sendToBroadCast(18282, arr), false);

	client.destroy();
	server.destroy();
	ctx.release();
}


TEST(UDP, ReceiveFromTest) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPClientListenerMock cMock;

	server.create(18282);
	server.bind();
	client.create();
	client.addListener(&cMock);
	ctx.run();

	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> recv;
	auto re = std::async(std::launch::async, [&client, &recv]() {
		client.receiveFrom(28282, recv);
	});

	// Maybe stay some times...
	std::this_thread::sleep_for(std::chrono::seconds(2));

	std::array<uint8_t, cys::comm::app::MAX_BUFFER_NUM> arr = { "TEST" };
	server.sendTo("127.0.0.1", 28282, arr);	
	
	EXPECT_EQ((arr == recv), true);
	re.get();
	client.destroy();
	server.destroy();
	ctx.release();
}

TEST(UDP, ReceiveFromTestWithStreamBuf) {
	cys::comm::Context ctx;
	cys::comm::app::UDPServer server(&ctx);
	cys::comm::app::UDPClient client(&ctx);

	UDPClientListenerMock cMock;

	server.create(18282);
	server.bind();
	client.create(); 
	client.addListener(&cMock);
	ctx.run();

	boost::asio::streambuf buf;
	std::istream is(&buf);
	auto re = std::async(std::launch::async, [&client, &buf]() {
		client.receiveFrom(28282, buf);
	});

	// Maybe stay some times...
	std::this_thread::sleep_for(std::chrono::seconds(2));

	std::string arr("TEST");
	server.sendTo("127.0.0.1", 28282, arr);
	std::string recv;	
	is >> recv;

	EXPECT_EQ((arr == recv), true);
	re.get();
	client.destroy();
	server.destroy();
	ctx.release();
}
