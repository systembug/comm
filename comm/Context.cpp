#include "Context.h"

namespace cys {
namespace comm {
	Context::Context()
		: Context(std::thread::hardware_concurrency())
	{
	}

	Context::Context(uint32_t threadNum)
		: m_threadNum(threadNum)
		, m_context(std::make_unique<boost::asio::io_context>())
		, m_strand(std::make_unique<boost::asio::io_context::strand>(*m_context))
		, m_work()
		, m_threads()
	{
	}

	Context::~Context()
	{
		release();
	}

	Context::Context(Context&& ctx) noexcept
		: m_threadNum(std::move(ctx.m_threadNum))
		, m_context(std::move(ctx.m_context))
		, m_strand(std::move(ctx.m_strand))
		, m_work(std::move(ctx.m_work))
		, m_threads(std::move(ctx.m_threads))
	{
	}

	Context& Context::operator=(Context&& ctx) noexcept
	{
		if (this != &ctx) {
			m_threadNum = std::move(ctx.m_threadNum);
			m_context = std::move(ctx.m_context);
			m_strand = std::move(ctx.m_strand);
			m_work = std::move(ctx.m_work);
			m_threads = std::move(ctx.m_threads);
		}

		return *this;
	}

	bool Context::run() {
		if (m_threadNum < 1) return false;

		if (m_work.get() == nullptr) m_work = std::make_unique<boost::asio::io_context::work>(*m_context);		
		m_threads.reserve(m_threadNum);
		for (unsigned int i = 0; i < m_threadNum; ++i) {
			m_threads.emplace_back(std::async(std::launch::async, [this]() {
				return m_context->run();
			}));
		}

		return true;
	}
	
	bool Context::release() {
		if (m_work.get() != nullptr) m_work.reset(nullptr);

		for (auto& thread : m_threads) {
			if (thread.valid()) thread.get();
		}

		m_threads.clear();
		m_threads.shrink_to_fit();
		return true;
	}	
}
}


