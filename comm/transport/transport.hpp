#pragma once

namespace cys {
	namespace comm {
		template <class Trans>
		class Transport {
		public:
			Transport() = default;
			~Transport() = default;
			Transport(const Transport& trans) = default;
			Transport(Transport&& trans) noexcept = default;
			Transport& operator=(const Transport& trans) = default;
			Transport& operator=(Transport&& trans) noexcept = default;

		public:
			template <class Opt>
			inline Trans::Return create(Opt&& options);

			template <class Opt>
			inline Trans::Return bind(Opt&& options);

			template <class Opt>
			inline Trans::Return unBind(Opt&& options);

			template <class Opt>
			inline Trans::Return destroy(Opt&& options);

		public:
			template <class Contents>
			inline Trans::Return send(Contents&& contents);

			template <class Opt, class Contents>
			inline Trans::Return send(Opt&& options, Contents&& contents);

			template <class Contents>
			inline Trans::Return sendAsync(Contents&& contents);

			template <class Opt, class Contents>
			inline Trans::Return sendAsync(Opt&& options, Contents&& contents);
		};
	}
}