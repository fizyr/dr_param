#pragma once
#include <string>
#include <type_traits>
#include <utility>

/*
 * This header contains some utilities for limited introspection.
 *
 * It is used amongst others to implement automatically generated conversions to/from YAML.
 */

namespace dr::param {

namespace detail {
	/// Compile time check to see if a type is a std::reference_wrapper.
	template<typename T>
	struct is_reference_wrapper : std::false_type {};

	template<typename T>
	struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};

	/// Get the raw reference type of dereferencing a T.
	/**
	 * While this is just T for normal references,
	 * it's `T &` for pointers and std::reference_wrapper.
	 *
	 * Note that technically dereferencing something actually gives you a reference.
	 */
	template<typename T>
	struct dereference_t {
		using type = T;
	};

	template<typename T>
	struct dereference_t<T *> {
		using type = T &;
	};

	template<typename T>
	struct dereference_t<std::reference_wrapper<T>> {
		using type = T &;
	};
}

template<typename T>
constexpr bool is_reference_wrapper = detail::is_reference_wrapper<T>{};

template<typename T>
constexpr bool is_dereferencable = std::is_pointer_v<T> || is_reference_wrapper<T>;

template<typename T>
using dereferenced_t = typename detail::dereference_t<T>::type;


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

	using const_accesor_type = std::invoke_result_t<F, T const &>;
	using   mut_accesor_type = std::invoke_result_t<F, T       &>;

	using const_result_type = std::conditional_t<std::is_pointer_v<const_accesor_type>,
		dereferenced_t<const_accesor_type>,
		const_accesor_type
	>;

	using mut_result_type = std::conditional_t<is_dereferencable<mut_accesor_type>,
		dereferenced_t<mut_accesor_type>,
		mut_accesor_type
	>;

	static_assert(std::is_reference_v<mut_accesor_type> || std::is_pointer_v<mut_accesor_type> || is_reference_wrapper<mut_accesor_type>,
		"result of invoking accessor(T &) must be a reference, pointer or std::reference_wrapper");

	const_result_type access(T const & parent) const {
		if constexpr (std::is_pointer_v<const_accesor_type>) {
			return *accessor(parent);
		} else {
			return accessor(parent);
		}
	}

	mut_result_type access(T & parent) const {
		if constexpr (std::is_pointer_v<mut_accesor_type> || is_reference_wrapper<mut_accesor_type>) {
			return *accessor(parent);
		} else {
			return accessor(parent);
		}
	}
};

/// Concrete implementation of MemberInfo concept using a pointer to member function.
template<typename T, typename M>
struct MemberPtrInfo : MemberInfoBase {
	M T::* member;

	M const & access(T const & parent) const { return parent.*member; }
	M       & access(T       & parent) const { return parent.*member; }
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
struct Decomposition;

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
