#ifndef __TYPE_WRAPPERS_H__
#define __TYPE_WRAPPERS_H__

#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
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

//(auto)(class),1 -> auto,class...
#define TYPE_WRAPPERS_GETDEF(r, state, i, x) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(TYPE_WRAPPERS_GET_,x)
#define TYPE_WRAPPERS_TARGS(SEQ,VARARG) BOOST_PP_SEQ_FOR_EACH_I(TYPE_WRAPPERS_GETDEF, _, SEQ) BOOST_PP_EXPR_IIF(VARARG,...)

//(auto)(class),1 -> auto Arg0,class... Arg1
#define TYPE_WRAPPERS_GETVARARG(r, state, i, x) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(TYPE_WRAPPERS_GET_,x) BOOST_PP_EXPR_IIF(BOOST_PP_EQUAL(state,i),...) Arg##i
#define TYPE_WRAPPERS_GETARG(r, state, i, x) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(TYPE_WRAPPERS_GET_,x) Arg##i
#define TYPE_WRAPPERS_NAMED_ARGS(SEQ,VARARG) BOOST_PP_SEQ_FOR_EACH_I(BOOST_PP_IIF(VARARG,TYPE_WRAPPERS_GETVARARG,TYPE_WRAPPERS_GETARG), BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(SEQ)), SEQ)

//(auto)(class),1 -> Arg0,Arg1...
#define TYPE_WRAPPERS_GETARGNAME(r, state, i, x) BOOST_PP_COMMA_IF(i) Arg##i
#define TYPE_WRAPPERS_INSERT_ARGS(SEQ,VARARG) BOOST_PP_SEQ_FOR_EACH_I(TYPE_WRAPPERS_GETARGNAME, _, SEQ) BOOST_PP_EXPR_IIF(VARARG,...)

//(auto)(class) -> Wrap_auto_class
#define TYPE_WRAPPERS_GETWRAPPERNAME(s, state, x) BOOST_PP_CAT(BOOST_PP_CAT(state,_),x)
#define TYPE_WRAPPERS_WRAPPER_NAME(SEQ) BOOST_PP_CAT(Wrap_,BOOST_PP_SEQ_FOLD_LEFT(TYPE_WRAPPERS_GETWRAPPERNAME, BOOST_PP_SEQ_HEAD(SEQ), BOOST_PP_SEQ_TAIL(SEQ)))

#define TYPE_WRAPPERS_DEF_IMPL(ARGS,VARARG) \
namespace type_wrappers {\
namespace detail {\
template<template<TYPE_WRAPPERS_TARGS(ARGS,VARARG)>class TT> struct TYPE_WRAPPERS_WRAPPER_NAME(ARGS) {\
    template<TYPE_WRAPPERS_NAMED_ARGS(ARGS,VARARG)>\
        using type = TT<TYPE_WRAPPERS_INSERT_ARGS(ARGS,VARARG)>;\
};\
}\
template<template<TYPE_WRAPPERS_TARGS(ARGS,VARARG)>class TT> detail::TYPE_WRAPPERS_WRAPPER_NAME(ARGS)<TT> wrap();\
}\
/**/

#define TYPE_WRAPPERS_DEF_EXACT(ARGS)  TYPE_WRAPPERS_DEF_IMPL(ARGS,0)
#define TYPE_WRAPPERS_DEF_VARARG(ARGS) TYPE_WRAPPERS_DEF_IMPL(ARGS,1)

#endif
