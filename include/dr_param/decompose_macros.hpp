#pragma once
#include "decompose.hpp"

#include <tuple>

#define DR_PARAM_MEMBER_TUPLE_ENTRY_FIRST_END
#define DR_PARAM_MEMBER_TUPLE_ENTRY1_END
#define DR_PARAM_MEMBER_TUPLE_ENTRY2_END

#define DR_PARAM_MEMBER_TUPLE_ENTRY_FIRST(...) dr::param::memberInfo<Type>(__VA_ARGS__) DR_PARAM_MEMBER_TUPLE_ENTRY1
#define DR_PARAM_MEMBER_TUPLE_ENTRY1(...) , dr::param::memberInfo<Type>(__VA_ARGS__) DR_PARAM_MEMBER_TUPLE_ENTRY2
#define DR_PARAM_MEMBER_TUPLE_ENTRY2(...) , dr::param::memberInfo<Type>(__VA_ARGS__) DR_PARAM_MEMBER_TUPLE_ENTRY1

#define DR_PARAM_ADD_END(...) DR_PARAM_ADD_END2(__VA_ARGS__)
#define DR_PARAM_ADD_END2(...) __VA_ARGS__ ## _END

#define DR_PARAM_MEMBERS_TUPLE(...) std::make_tuple(DR_PARAM_ADD_END(DR_PARAM_MEMBER_TUPLE_ENTRY_FIRST __VA_ARGS__))

/// Macro to easily define the decomposition of a type with full flexibility.
/**
 * This macro must be invoked from the global namespace.
 *
 * Usage:
 * DR_PARAM_DEFINE_DECOMPOSITION(T,
 *  ("bar", "int",    "The bar member of T", true, &T::bar)
 *  ("baz", "double", "The baz member of T", true, [] (auto & v) { return v.baz() })
 * )
 *
 * This macro defines a specialization for the struct dr::param::Decomposition<Foo>.
 *
 * All parenthesis enclosed groups of arguments are passed to dr::param::memberInfo.
 * See that function for more details.
 */
#define DR_PARAM_DEFINE_DECOMPOSITION(T, ...) \
namespace dr::param { \
	template<> struct Decomposition<T> { \
		using Type = T; \
		static auto decompose() { \
			return DR_PARAM_MEMBERS_TUPLE(__VA_ARGS__); \
		} \
	}; \
}

#define DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY_FIRST_END
#define DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY1_END
#define DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY2_END

#define DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY_FIRST(NAME, TYPE, DESCRIPTION, REQUIRED)   dr::param::memberInfo(#NAME, TYPE, DESCRIPTION, REQUIRED, &Type::NAME) DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY1
#define DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY1(NAME, TYPE, DESCRIPTION, REQUIRED)      , dr::param::memberInfo(#NAME, TYPE, DESCRIPTION, REQUIRED, &Type::NAME) DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY2
#define DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY2(NAME, TYPE, DESCRIPTION, REQUIRED)      , dr::param::memberInfo(#NAME, TYPE, DESCRIPTION, REQUIRED, &Type::NAME) DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY1

#define DR_PARAM_STRUCT_MEMBERS_TUPLE(...) std::make_tuple(DR_PARAM_ADD_END(DR_PARAM_MEMBER_TUPLE_STRUCT_ENTRY_FIRST __VA_ARGS__))

/// Macro to easily define the decomposition of a simple type.
/**
 * This macro must be invoked from the global namespace.
 * It is less flexible but easier to use compared to DR_PARAM_DEFINE_DECOMPOSITION.
 *
 * Usage:
 * DR_PARAM_DEFINE_STRUCT_DECOMPOSITION(T,
 *  (bar, "int",    "The bar member of T", true)
 *  (baz, "double", "The baz member of T", true)
 * )
 *
 * This macro defines a specialization for the struct dr::param::Decomposition<T>.
 *
 * Given a parenthesis enclosed group of argumentes (member, type, description, required),
 * each one is turned into a call to dr::param::memberInfo("member", type, description, required, &T::member).
 */
#define DR_PARAM_DEFINE_STRUCT_DECOMPOSITION(T, MEMBERS) \
namespace dr::param { \
	template<> struct Decomposition<T> { \
		using Type = T; \
		static auto decompose() { \
			return DR_PARAM_STRUCT_MEMBERS_TUPLE(MEMBERS); \
		} \
	}; \
}
