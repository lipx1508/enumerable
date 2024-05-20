#pragma once
#ifndef ENUMERABLE_H
#define ENUMERABLE_H

/* -=- Compile-time checks -=- */
#if __cplusplus <= 199711L
    #error enumerable requires C++11 or higher
#endif

/* -=- Includes -=- */
#if !defined(ENUMERABLE_NO_COUT_IMPL)
    #include <iostream>
#endif

#include <array>
#include <map>
#include <type_traits>

/* -=- Macro utils -=- */

// Macros for iterating over __VA_ARGS__
#define __ENUM_PARENS ()

#define __ENUM_EXPAND(...)  __ENUM_EXPAND4(__ENUM_EXPAND4(__ENUM_EXPAND4(__ENUM_EXPAND4(__VA_ARGS__))))
#define __ENUM_EXPAND4(...) __ENUM_EXPAND3(__ENUM_EXPAND3(__ENUM_EXPAND3(__ENUM_EXPAND3(__VA_ARGS__))))
#define __ENUM_EXPAND3(...) __ENUM_EXPAND2(__ENUM_EXPAND2(__ENUM_EXPAND2(__ENUM_EXPAND2(__VA_ARGS__))))
#define __ENUM_EXPAND2(...) __ENUM_EXPAND1(__ENUM_EXPAND1(__ENUM_EXPAND1(__ENUM_EXPAND1(__VA_ARGS__))))
#define __ENUM_EXPAND1(...) __VA_ARGS__

#define __ENUM_FOREACH(__macro, ...) __VA_OPT__(__ENUM_EXPAND(__ENUM_FOREACH1(__macro, __VA_ARGS__)))
#define __ENUM_FOREACH1(__macro, __value, ...) __macro(__value) __VA_OPT__(__ENUM_FOREACH2 __ENUM_PARENS (__macro, __VA_ARGS__))
#define __ENUM_FOREACH2() __ENUM_FOREACH1

// Counts the number of arguments in __VA_ARGS__
#define __ENUM_VA_COUNT1(v) 0, 
#define __ENUM_VA_COUNT(...) sizeof((int[]){ __ENUM_FOREACH(__ENUM_VA_COUNT1, __VA_ARGS__) }) / sizeof(int)
#define __ENUM_VA_FIRST(__f, ...) __f

// Macro conversions
#define __ENUM_TO_STRING(v) #v
#define __ENUM_TO_ARRAY(v) __##v, 
#define __ENUM_TO_STRING_ARRAY(v) #v,
#define __ENUM_TO_DECL(v) static inline value_type __##v;
#define __ENUM_TO_CEXPR(v) static inline __name v;

/* -=- enums:: namespace -=- */
namespace enums {
    /* -=- detail:: namespace -=- */
    namespace detail {
        /* -=- Enumerable information type -=- */
        template <typename T, typename Tc, size_t sz>
        struct enum_info {
            // NOTE: This stores information about the enumerator
            //       and it's respective fields

            typedef std::basic_string<Tc> string_type;

            typedef std::array<T, sz>           l_vals_type;
            typedef std::array<string_type, sz> l_strs_type;
            typedef std::map<T, string_type>    l_item_type;

            l_vals_type * vals;
            l_strs_type * strs;
            l_item_type * items;

            inline constexpr enum_info(l_vals_type * m_vals, l_strs_type * m_strs, l_item_type * m_items) {
                vals  = m_vals;
                strs  = m_strs;
                items = m_items;
            }
        };

        template <typename Tc>
        static inline auto trim_str(std::basic_string<Tc> str, bool & has_eq) {
            size_t end = 0;
            for (size_t i = 0; i < str.length(); ++i) {
                Tc c = str[i];
                if ((c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '=') && end == 0) {
                    end = i;
                }
                if (c == '=') {
                    has_eq = true;
                    break;
                }
            }
            if (end == 0) return str;
            return str.substr(0, end);
        }

        template <typename T, typename Tc, size_t sz>
        static inline size_t construct(enum_info<T, Tc, sz> info) {
            T last = 0;
            for (size_t i = 0; i < sz; ++i) {
                bool has_eq = false;
                auto v = info.vals->at(i);
                auto s = trim_str<Tc>(info.strs->at(i), has_eq);
                if (has_eq)
                    last = v + 1;
                else
                    v = last++;
                
                info.vals->at(i) = v;
                info.strs->at(i) = s;
                
                if (info.items->find(v) == info.items->end())
                    info.items->insert({ v, s });
            }
            
            return sz;
        }
    }

    /* -=- Enumerable base class -=- */
    template <typename T = int, typename Tc = char>
    class enumerable {
    public:
        /* -=- Tags -=- */
        typedef T                     value_type;
        typedef Tc                    char_type;
        typedef std::basic_string<Tc> string_type;

        typedef std::map<value_type, string_type> imap_type;

        /* -=- Static assertions -=- */
        static_assert(std::is_integral_v<T>, "cannot declare enumerable from non-integral type");

        /* -=- Constructors -=- */
        enumerable()          = default;
        virtual ~enumerable() = default;

        /* -=- Operator overloading -=- */
        inline constexpr bool operator==(enumerable<T> other) {
            return __val == other.__val;
        }
        
        inline constexpr bool operator!=(enumerable<T> other) {
            return __val != other.__val;
        }

        inline constexpr bool operator>(enumerable<T> other) {
            return __val > other.__val;
        }
        
        inline constexpr bool operator>=(enumerable<T> other) {
            return __val >= other.__val;
        }
        
        inline constexpr bool operator<(enumerable<T> other) {
            return __val < other.__val;
        }
        
        inline constexpr bool operator<=(enumerable<T> other) {
            return __val <= other.__val;
        }

        /* -=- String reflection -=- */
        virtual inline const string_type string() const {
            return "(invalid enum)";
        }

        inline constexpr const char_type * c_str() const {
            return string().c_str();
        }

        virtual inline const char_type * name() const {
            return "enumerable";
        }

        /* -=- Integral conversion -=- */
        virtual inline const bool valid() const {
            return false;
        }

        explicit inline constexpr operator bool() {
            return static_cast<bool>(__val);
        }

        explicit inline constexpr operator value_type() {
            return __val;
        }
    protected:
        T __val;
    };

    /* -=- String conversion -=- */
    template <typename T>
    inline typename enumerable<T>::string_type to_string(const enumerable<T> e) {
        return e.string();
    }
}

#if !defined(ENUMERABLE_NO_COUT_IMPL)
template <typename T>
inline std::ostream & operator<<(std::ostream & os, const enums::enumerable<T> & e) {
    if (e.valid()) os << e.name() << "::";
    os << e.string();
    return os;
}
#endif

#define enumerable_adv(__type, __cnt, __name, ...) \
    class __name : public enums::enumerable<__type> {\
    public:\
        enum __item { __VA_ARGS__ };\
        inline __name() {\
            if constexpr ((__cnt) == 0) __val = 0;\
            else __val = __vals[0];\
        }\
        inline ~__name() { __val = 0; }\
        inline __name(__item v) { __val = static_cast<value_type>(v); }\
        inline __name(value_type v) { __val = v; }\
        inline __name(string_type str) {\
            __val = -1;\
            for (size_t i = 0; i < __count; ++i) {\
                if (__strs[i] == str || (str == string_type(string_type(#__name "::") + __strs[i]))) {\
                    __val = __vals[i]; break;\
                }\
            }\
        }\
        inline const string_type string() const override {\
            if (__items.find(__val) != __items.end()) return __items[__val];\
            return "(invalid enum)";\
        }\
        inline const char_type * name() const override { return #__name; }\
        inline const bool valid() const override { return __items.find(__val) != __items.end(); }\
        inline bool operator==(__name other) { return __val == other.__val; }\
        inline bool operator!=(__name other) { return __val != other.__val; }\
        inline bool operator>(__name other) { return __val > other.__val; }\
        inline bool operator>=(__name other) { return __val >= other.__val; }\
        inline bool operator<(__name other) { return __val < other.__val; }\
        inline bool operator<=(__name other) { return __val <= other.__val; }\
        inline operator bool() { return static_cast<bool>(__val); }\
        inline operator value_type() { return __val; }\
        static inline const auto items() { return __items; }\
        static inline const auto names() { return __strs; }\
        static inline const auto values() { return __vals; }\
        static inline constexpr size_t count() { return __count; }\
    private:\
        __ENUM_FOREACH(__ENUM_TO_DECL, __VA_ARGS__)\
        typedef enums::detail::enum_info<value_type, char_type, __cnt> info;\
        static inline info::l_vals_type __vals = { __ENUM_FOREACH(__ENUM_TO_ARRAY, __VA_ARGS__) };\
        static inline info::l_strs_type __strs = { __ENUM_FOREACH(__ENUM_TO_STRING_ARRAY, __VA_ARGS__) };\
        static inline info::l_item_type __items;\
        static inline const size_t __count = enums::detail::construct(info(&__vals, &__strs, &__items));\
    };
#define enumerable(__name, ...) enumerable_adv(int, __ENUM_VA_COUNT(__VA_ARGS__), __name, __VA_ARGS__)

#endif
