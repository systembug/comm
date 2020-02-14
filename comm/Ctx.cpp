#include "Ctx.h"
#include "Context.h"

namespace cys {
namespace comm {
namespace wrapper {
	Ctx::Ctx()
		: m_ctx(new cys::comm::Context())
	{}

	Ctx::Ctx(uint32_t threadNum)
		: m_ctx(new cys::comm::Context(threadNum))
	{}

	Ctx::~Ctx()
	{
		if (m_ctx != nullptr) {
			delete m_ctx;
			m_ctx = nullptr;
		}
	}

	Ctx::Ctx(const Ctx& ctx)
		: m_ctx(ctx.m_ctx)
	{}
	
	Ctx::Ctx(Ctx&& ctx) noexcept
		: m_ctx(std::move(ctx.m_ctx))
	{}

	Ctx& Ctx::operator=(const Ctx& ctx)
	{
		if (this != &ctx) {
			m_ctx = ctx.m_ctx;
		}

		return *this;
	}

	Ctx& Ctx::operator=(Ctx&& ctx) noexcept
	{
		if (this != &ctx) {
			m_ctx = std::move(ctx.m_ctx);
		}

		return *this;
	}

	bool Ctx::run()
	{
		return m_ctx->run();
	}

	bool Ctx::release()
	{
		return m_ctx->release();
	}

	cys::comm::Context* Ctx::getContext()
	{
		return m_ctx;
	}
}
}
}