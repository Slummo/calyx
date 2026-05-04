#ifndef CX_CORE_H
#define CX_CORE_H

#include <ks/core.h>

/* Calyx namespace */

#define CX_TYPE(name)                           NS_TYPE(cx, name)
#define CX_TEMPLATED_TYPE(name, T)              NS_TEMPLATED_TYPE(cx, name, T)

#define CX_STRUCT_DECL(name)                    NS_STRUCT_DECL(cx, name)
#define CX_STRUCT_DEF(name, ...)                NS_STRUCT_DEF(cx, name, __VA_ARGS__)
#define CX_STRUCT(name, ...)                    NS_STRUCT(cx, name, __VA_ARGS__)
#define CX_TEMPLATED_STRUCT(name, T, ...)       NS_TEMPLATED_STRUCT(cx, name, T, __VA_ARGS__)

#define CX_UNION_DECL(name)                     NS_UNION_DECL(cx, name)
#define CX_UNION_DEF(name, ...)                 NS_UNION_DEF(cx, name, __VA_ARGS__)
#define CX_UNION(name, ...)                     NS_UNION(cx, name, __VA_ARGS__)
#define CX_TEMPLATED_UNION(name, T, ...)        NS_TEMPLATED_UNION(cx, name, T, __VA_ARGS__)

#define CX_TEMPLATED_METHOD(name, action, T)    NS_TEMPLATED_METHOD(cx, name, action, T)

#define CX_ENUM(name, ...)                      NS_ENUM(cx, name, __VA_ARGS__)

#define CX_FUNC(T, name, ...)                   NS_FUNC(T, cx, name, __VA_ARGS__)

#define CX_API KS_API

#endif  // CX_CORE_H