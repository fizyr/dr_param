#pragma once
#include <string>
#include <type_traits>
#include <utility>

namespace dr::param {

/// Base class for implementing a class that implements the MemberInfo concept.
/**
 * The MemberInfo concept requires the members of MemberInfoBase seen here,
 * and two overloads with the signatures:
 *   M       * access(T       &) const
 *   M const * access(T const &) const
 * where M denotes the type of the member and T denotes the type of the object being decomposed.
 *
 * The `access` function must return a reference to the correct member,
 * given a reference to the struct being decomposed.
 */
struct MemberInfoBase {
	/// The name of the member.
	std::string name;

	/// A human readable terse description of the type of the member.
	std::string type;

	/// A human readable more elaborate description of the member.
	std::string description;

	/// True if the member is required to form a valid whole object.
	/**
	 * This could be false for members which have a valid default value.
	 */
	bool required;
};

/// Concrete implementation of MemberInfo concept using a provided functor as member accessor.
template<typename T, typename F>
struct MemberInfo : MemberInfoBase {
	F accessor;

	static_assert(std::is_pointer_v<std::invoke_result_t<F, T const &>>, "result of invoking accessor(T const &) must be a pointer");
	static_assert(std::is_pointer_v<std::invoke_result_t<F, T       &>>, "result of invoking accessor(T &) must be a pointer");

	auto access(T const & parent) const { return accessor(parent); }
	auto access(T       & parent) const { return accessor(parent); }
};

/// Concrete implementation of MemberInfo concept using a pointer to member function.
template<typename T, typename M>
struct MemberPtrInfo : MemberInfoBase {
	M T::* member;

	M const * access(T const & parent) const { return &(parent.*member); }
	M       * access(T       & parent) const { return &(parent.*member); }
};


/// Create a member info struct from the required fields and an accessor functor.
template<typename T, typename F>
auto memberInfo(std::string name, std::string type, std::string description, bool required, F && accessor) {
	return MemberInfo<std::decay_t<T>, std::decay_t<F>> {
		{name, type, description, required}, std::forward<F>(accessor)
	};
}

/// Create a member info struct from the required fields and a pointer-to-member.
template<typename T, typename M>
auto memberInfo(std::string name, std::string type, std::string description, bool required, M T::* member) {
	return MemberPtrInfo<T, M>{{std::move(name), std::move(type), std::move(description), required}, member};
}

/// Base struct to specialize when implementing decompositions for a given type.
/**
 * The specialization should have atleast one static member function called `decompose()`.
 * It should return a tuple of structs that implement the `MemberInfo` concept.
 */
template<typename T>
struct Decomposition {
	static_assert(!std::is_same_v<T, T>, "no decomposition available for given type T");
};

namespace detail {
	template<typename T, typename = void> struct can_decompose_ : std::false_type {};
	template<typename T> struct can_decompose_<T, std::void_t<decltype(Decomposition<T>::decompose())>> : std::true_type{};
}

/// Check if a decomposition is available for a given type.
template<typename T> constexpr bool can_decompose = detail::can_decompose_<T>::value;

template<typename T>
auto decompose() {
	static_assert(can_decompose<T>, "no decomposition available for given type T");
	return Decomposition<T>::decompose();
}

}
