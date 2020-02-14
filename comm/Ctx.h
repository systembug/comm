#include <cstdint>

namespace cys {
namespace comm {
	class Context;
namespace wrapper {
	class Ctx 
	{
	public:
		Ctx();
		Ctx(uint32_t threadNum);
		~Ctx();
		Ctx(const Ctx& ctx);
		Ctx(Ctx&& ctx) noexcept;
		Ctx& operator=(const Ctx& ctx);
		Ctx& operator=(Ctx&& ctx) noexcept;

	public:
		bool run();
		bool release();

	public:
		cys::comm::Context* getContext();

	private:
		cys::comm::Context* m_ctx;
	};
}
}
}