#ifndef __TYPE_WRAPPERS_H__
#define __TYPE_WRAPPERS_H__

#include <boost/preprocessor/seq/to_list.hpp>
#include <boost/preprocessor/list/for_each_i.hpp>
#include <boost/preprocessor/list/fold_left.hpp>
#include <boost/preprocessor/list/transform.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/preprocessor/control/expr_iif.hpp>
#include <boost/preprocessor/logical/bitand.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/cat.hpp>

namespace type_wrappers {
	namespace detail {
        template<class T>
        struct TypeWrapper {
            using type = T;
        };
		template<auto Val>
		struct ValueWrapper {
			static constexpr auto value = Val;
		};
	}
	template<auto Val> detail::ValueWrapper<Val> wrap();
	template<class T> detail::TypeWrapper<T> wrap();
}

#define TYPE_WRAPPERS_GET_class class
#define TYPE_WRAPPERS_GET_auto auto

#define TYPE_WRAPPERS_CLASS_class 0
#define TYPE_WRAPPERS_CLASS_auto 1
#define TYPE_WRAPPERS_CLASS_TT 2

//(auto)(class),1 -> auto,class...
#define TYPE_WRAPPERS_GETDEF(r, state, i, x) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(TYPE_WRAPPERS_GET_,x)
#define TYPE_WRAPPERS_TARGS(LIST,VARARG) BOOST_PP_LIST_FOR_EACH_I(TYPE_WRAPPERS_GETDEF, _, LIST) BOOST_PP_EXPR_IIF(VARARG,...)

//(auto)(class),1 -> auto Arg0,class... Arg1
#define TYPE_WRAPPERS_GETVARARG(r, state, i, x) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(TYPE_WRAPPERS_GET_,x) BOOST_PP_EXPR_IIF(BOOST_PP_EQUAL(state,i),...) Arg##i
#define TYPE_WRAPPERS_GETARG(r, state, i, x) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(TYPE_WRAPPERS_GET_,x) Arg##i
#define TYPE_WRAPPERS_NAMED_ARGS(LIST,VARARG) BOOST_PP_LIST_FOR_EACH_I(BOOST_PP_IIF(VARARG,TYPE_WRAPPERS_GETVARARG,TYPE_WRAPPERS_GETARG), BOOST_PP_DEC(BOOST_PP_LIST_SIZE(LIST)), LIST)

//(auto)(class),1 -> Arg0,Arg1...
#define TYPE_WRAPPERS_INSERT_ARGS_WRAP(name,value) name
#define TYPE_WRAPPERS_INSERT_ARGS_UNWRAP(name,value) BOOST_PP_CAT(TYPE_WRAPPERS_INSERT_ARGS_UNWRAP,BOOST_PP_CAT(TYPE_WRAPPERS_CLASS_,value))(name)
#define TYPE_WRAPPERS_INSERT_ARGS_UNWRAP0(name) typename name::type
#define TYPE_WRAPPERS_INSERT_ARGS_UNWRAP1(name) name::value
#define TYPE_WRAPPERS_INSERT_ARGS_UNWRAP2(name) name::template type
#define TYPE_WRAPPERS_GETARGNAME(r, state, i, x) BOOST_PP_COMMA_IF(i) state(Arg##i,x)
#define TYPE_WRAPPERS_INSERT_ARGS(LIST,VARARG,WRAPTYPE) BOOST_PP_LIST_FOR_EACH_I(TYPE_WRAPPERS_GETARGNAME, TYPE_WRAPPERS_INSERT_ARGS_##WRAPTYPE, LIST) BOOST_PP_EXPR_IIF(VARARG,...)

//(auto)(class) -> Wrap_auto_class
#define TYPE_WRAPPERS_GETWRAPPERNAME(s, state, x) BOOST_PP_CAT(BOOST_PP_CAT(state,_),x)
#define TYPE_WRAPPERS_WRAPPER_NAME(LIST) BOOST_PP_CAT(Wrap_,BOOST_PP_LIST_FOLD_LEFT(TYPE_WRAPPERS_GETWRAPPERNAME, _, LIST))

#define TYPE_WRAPPERS_ALL_TO_CLASS_OP(s,data,elem) class
#define TYPE_WRAPPERS_ALL_TO_CLASS(LIST) BOOST_PP_LIST_TRANSFORM(TYPE_WRAPPERS_ALL_TO_CLASS_OP,_,LIST)

#define TYPE_WRAPPERS_WRAP_IMPL(ARGS,VARARG) \
namespace type_wrappers {\
namespace detail {\
template<template<TYPE_WRAPPERS_TARGS(ARGS,VARARG)>class TT> struct TYPE_WRAPPERS_WRAPPER_NAME(ARGS) {\
    template<TYPE_WRAPPERS_NAMED_ARGS(ARGS,VARARG)>\
    using type = TT<TYPE_WRAPPERS_INSERT_ARGS(ARGS,VARARG,WRAP)>;\
};\
}\
template<template<TYPE_WRAPPERS_TARGS(ARGS,VARARG)>class TT> detail::TYPE_WRAPPERS_WRAPPER_NAME(ARGS)<TT> wrap();\
}\
/**/

#define TYPE_WRAPPERS_UNWRAP_IMPL(ARGS,VARARG) \
namespace type_wrappers {\
namespace detail {\
template<template<TYPE_WRAPPERS_TARGS(ARGS,VARARG)>class TT> struct BOOST_PP_CAT(Un,TYPE_WRAPPERS_WRAPPER_NAME(ARGS)) {\
    template<TYPE_WRAPPERS_NAMED_ARGS(TYPE_WRAPPERS_ALL_TO_CLASS(ARGS),VARARG)>\
    using type = TT<TYPE_WRAPPERS_INSERT_ARGS(ARGS,VARARG,UNWRAP)>;\
};\
}\
template<template<TYPE_WRAPPERS_TARGS(ARGS,VARARG)>class TT> detail::BOOST_PP_CAT(Un,TYPE_WRAPPERS_WRAPPER_NAME(ARGS))<TT> unwrapper();\
}\
/**/

#define TYPE_WRAPPERS_WRAP_EXACT(ARGS)  TYPE_WRAPPERS_WRAP_IMPL(BOOST_PP_SEQ_TO_LIST(ARGS),0)
#define TYPE_WRAPPERS_WRAP_VARARG(ARGS) TYPE_WRAPPERS_WRAP_IMPL(BOOST_PP_SEQ_TO_LIST(ARGS),1)

#define TYPE_WRAPPERS_UNWRAP_EXACT(ARGS) TYPE_WRAPPERS_UNWRAP_IMPL(BOOST_PP_SEQ_TO_LIST(ARGS),0)
#define TYPE_WRAPPERS_UNWRAP_VARARG(ARGS) TYPE_WRAPPERS_UNWRAP_IMPL(BOOST_PP_SEQ_TO_LIST(ARGS),1)

#endif
