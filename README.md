# type-wrappers

[![stable](http://badges.github.io/stability-badges/dist/stable.svg)](http://github.com/badges/stability-badges)

Enable generic handling of type, template type and value template parameters by generating type (un)wrappers.

## Motivation

C++ templates are incredibly powerful, but while trying to write higher-order templates, you quickly run into problems:

```c++
template<class,class>
struct ClassTemplate;

template<template<class...>class, int>
struct NonClassTemplate;

template<template<class...>class T, class... Args>
using apply = T<Args...>;

using x = apply<ClassTemplate,int,bool>; // ClassTemplate<int,bool>
using y = apply<NonClassTemplate,ClassTemplate,1>; // Doesn't compile :(
```

There is no way in the language to write `apply` so that it will accept anything more general than `class...` templates.
A fix for this is to wrap both `NonClassTemplate` and its arguments in types and pass those:
```c++
template<auto Val>
struct ValueWrapper {
  static constexpr value = Val;
};
template<template<class...>class T>
struct TWrapper {
  template<class... Args>
  using type = T<Args...>;
};
template<template<template<class...>class,int>class Wrapped>
struct UnWrapper {
  template<class TArg,class intArg>
  using type = Wrapped<TArg::template type,intArg::value>;
};

using y = apply<UnWrapper<NonClassTemplate>::template type,TWrapper<ClassTemplate>,ValueWrapper<1>>; // yay! :)
```

These wrappers and unwrappers have to be written for every template signature you want to use in this way.

This single-header library aims to make this easier with the help of (YUCK!) macro's that automatically make these wrappers and unwrappers for you.

## Usage

This library defines six user-facing macro's:
 - `TYPE_WRAPPERS_WRAP_[EXACT|VARARG](...)`
 - `TYPE_WRAPPERS_UNWRAP_[EXACT|VARARG](...)`
 - `TYPE_WRAP(arg)`
 - `TYPE_UNWRAPPER(arg)`

For example, to create the wrapper for `ClassTemplate`(`TWrapper` in the above example code), you use `TYPE_WRAPPERS_WRAP_VARARG(class)`, or if you only want a `class,class` wrapper: `TYPE_WRAPPERS_WRAP_EXACT(class,class)`.
The `VARARG` variants create their wrappers with the last argument as a variable template argument.

Because the first argument to `NonClassTemplate` is a template template parameter, you need to define the following two macro's with a unique name before you can create its unwrapper:
```c++
#define TYPE_WRAPPERS_GET_tcc template<class,class>class
#define TYPE_WRAPPERS_CLASS_tcc TYPE_WRAPPERS_CLASS_TT
```

`TYPE_WRAPPERS_GET_[name]` is to define what the signature of the template class looks like, while `TYPE_WRAPPERS_CLASS_[name]` defines what type of parameter it is(a template template parameter), the other two being `TYPE_WRAPPERS_CLASS_class` and `TYPE_WRAPPERS_CLASS_auto` for type and value template arguments respectively.

The unwrapper can then be defined like this:
```c++
TYPE_WRAPPERS_UNWRAP(tcc,auto)
```

Which then enables this:
```c++
using y = apply<TYPE_UNWRAPPER(NonClassTemplate),TYPE_WRAP(ClassTemplate),TYPE_WRAP(1)>;
```

Full code:
```c++
#include <type_wrappers.h>

template<class,class>
struct ClassTemplate;

template<template<class...>class, int>
struct NonClassTemplate;

template<template<class...>class T, class... Args>
using apply = T<Args...>;

#define TYPE_WRAPPERS_GET_tcc template<class,class>class
#define TYPE_WRAPPERS_CLASS_tcc TYPE_WRAPPERS_CLASS_TT

TYPE_WRAPPERS_WRAP_VARARG(class)
TYPE_WRAPPERS_UNWRAP_EXACT(tcc,auto)

using y = apply<TYPE_UNWRAPPER(NonClassTemplate),TYPE_WRAP(ClassTemplate),TYPE_WRAP(1)>;
```

## Gotcha

Defining multiple wrappers or unwrappers can get tricky when there is a possibility of overlap in what they capture. If you wrap a template type that matches the signature of multiple wrappers, you will get a compile-time error. This is primarily a problem when defining vararg templates:
```c++
TYPE_WRAPPERS_WRAP_VARARG(class,auto)
TYPE_WRAPPERS_WRAP_VARARG(class)

template<class,auto...>
struct Foo;

template<class...>
struct Bar;

template<class>
struct FooBar;

using fooWrapper = TYPE_WRAP(Foo); //OK
using barWrapper = TYPE_WRAP(Bar); //OK
using foobarWrapper = TYPE_WRAP(FooBar); //compile-time error bc ambiguous overload
```
