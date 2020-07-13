// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef LEXY_ENCODING_HPP_INCLUDED
#define LEXY_ENCODING_HPP_INCLUDED

#include <lexy/_detail/config.hpp>

namespace lexy
{
/// An encoding where the input is some 8bit encoding (ASCII, UTF-8, extended ASCII etc.).
struct default_encoding
{
    using char_type = char;
    using int_type  = int;

    template <typename OtherCharType>
    static constexpr bool is_secondary_char_type = false;

    static LEXY_CONSTEVAL int_type eof()
    {
        return -1;
    }

    static constexpr int_type to_int_type(char_type c) noexcept
    {
        if constexpr (std::is_unsigned_v<char_type>)
            // We can just convert it to int directly.
            return static_cast<int_type>(c);
        else
        {
            // We first need to prevent negative values, by making it unsigned.
            auto value = static_cast<unsigned char>(c);
            return static_cast<int_type>(value);
        }
    }
};

// An encoding where the input is assumed to be valid ASCII.
struct ascii_encoding
{
    using char_type = char;
    using int_type  = char;

    template <typename OtherCharType>
    static constexpr bool is_secondary_char_type = false;

    static LEXY_CONSTEVAL int_type eof()
    {
        if constexpr (std::is_signed_v<char_type>)
            return static_cast<int_type>(-1);
        else
            return static_cast<int_type>(0xFF);
    }

    static constexpr int_type to_int_type(char_type c) noexcept
    {
        return static_cast<int_type>(c);
    }
};

/// An encoding where the input is assumed to be valid UTF-8.
struct utf8_encoding
{
    using char_type = LEXY_CHAR8_T;
    using int_type  = LEXY_CHAR8_T;

    template <typename OtherCharType>
    static constexpr bool is_secondary_char_type = false;

    static LEXY_CONSTEVAL int_type eof()
    {
        // 0xFF is not part of valid UTF-8.
        return static_cast<int_type>(0xFF);
    }

    static constexpr int_type to_int_type(char_type c) noexcept
    {
        return static_cast<int_type>(c);
    }
};
template <>
constexpr bool utf8_encoding::is_secondary_char_type<char> = true;

/// An encoding where the input is just raw bytes, not characters.
struct raw_encoding
{
    using char_type = unsigned char;
    using int_type  = int;

    template <typename OtherCharType>
    static constexpr bool is_secondary_char_type = false;

    static LEXY_CONSTEVAL int_type eof()
    {
        return -1;
    }

    static constexpr int_type to_int_type(char_type c) noexcept
    {
        return static_cast<int_type>(c);
    }
};
template <>
constexpr bool raw_encoding::is_secondary_char_type<char> = true;
} // namespace lexy

namespace lexy
{
template <typename CharT>
struct _deduce_encoding;
template <typename CharT>
using deduce_encoding = typename _deduce_encoding<CharT>::type;

template <>
struct _deduce_encoding<char>
{
    using type = default_encoding; // Don't know the exact encoding.
};

#if LEXY_HAS_CHAR8_T
template <>
struct _deduce_encoding<LEXY_CHAR8_T>
{
    using type = encoding_utf8;
};
#endif

template <>
struct _deduce_encoding<unsigned char>
{
    using type = raw_encoding;
};
} // namespace lexy

#endif // LEXY_ENCODING_HPP_INCLUDED