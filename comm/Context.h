#pragma once
#include <boost/asio.hpp>

namespace cys {
namespace comm {
	// TODO: control individual strand...
	class Context
	{
	public:
		Context();
		Context(uint32_t threadNum);
		~Context();
		Context(const Context& ctx) = delete;
		Context(Context&& ctx) noexcept;
		Context& operator=(const Context& ctx) = delete;
		Context& operator=(Context&& ctx) noexcept;

	public:
		bool run();
		bool release();

		inline boost::asio::io_context& getContext() const { return *m_context; }

	private:
		uint32_t m_threadNum;
		std::unique_ptr<boost::asio::io_context> m_context;
		std::unique_ptr<boost::asio::io_context::strand> m_strand;
		std::unique_ptr<boost::asio::io_context::work> m_work;
		std::vector<std::future<std::size_t>> m_threads;
	};
}
}



