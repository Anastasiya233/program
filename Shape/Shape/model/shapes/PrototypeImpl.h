#pragma once

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

template <typename T>
concept Polymorphic = std::has_virtual_destructor_v<T>;

template <typename CI>
concept CloneInterfaceLike = Polymorphic<CI> && requires(CI const& x) {
	{ x.Clone() } -> std::convertible_to<std::unique_ptr<CI>>;
};

template <typename CI, typename Base>
concept ProtoBaseOk = CloneInterfaceLike<CI> && std::derived_from<Base, CI>;

template <typename Impl, typename CloneInterface, typename Base = CloneInterface>
	requires ProtoBaseOk<CloneInterface, Base>
class PrototypeImpl : public Base
{
public:
	template <typename... Args>
	explicit PrototypeImpl(Args&&... args)
		: Base(std::forward<Args>(args)...)
	{
	}

	[[nodiscard]] std::unique_ptr<CloneInterface> Clone() const override
	{
		static_assert(std::is_base_of_v<Base, Impl>, "Impl must derive from Base");
		static_assert(std::is_copy_constructible_v<Impl>, "Impl must be copy constructible");
		return std::make_unique<Impl>(static_cast<const Impl&>(*this));
	}
};