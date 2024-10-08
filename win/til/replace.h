// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

namespace til
{
    // Method Description:
    // - This is a function for finding all occurrences of a given string
    //   `needle` in a larger string `haystack`, and replacing them with the
    //   string `replacement`.
    // - This find/replace is done in-place, leaving `haystack` modified as a result.
    // Arguments:
    // - haystack: The string to find and replace in
    // - needle: The string to search for
    // - replacement: The string to replace `needle` with
    // Return Value:
    // - <none>
    template<typename T>
    void replace_needle_in_haystack_inplace(T& haystack,
                                            const as_view_t<T>& needle,
                                            const as_view_t<T>& replacement)
    {
        auto pos{ T::npos };
        while ((pos = haystack.rfind(needle, pos)) != T::npos)
        {
            haystack.replace(pos, needle.size(), replacement);
        }
    }

    // Method Description:
    // - This is a function for finding all occurrences of a given string
    //   `needle` in a larger string `haystack`, and replacing them with the
    //   string `replacement`.
    // - This find/replace is done on a copy of `haystack`, leaving `haystack`
    //   unmodified, and returning a new string.
    // Arguments:
    // - haystack: The string to search for `needle` in.
    // - needle: The string to search for
    // - replacement: The string to replace `needle` with
    // Return Value:
    // - a copy of `haystack` with all instances of `needle` replaced with `replacement`.`
    template<typename T>
    T replace_needle_in_haystack(const T& haystack,
                                 const as_view_t<T>& needle,
                                 const as_view_t<T>& replacement)
    {
        std::basic_string<typename T::value_type> result{ haystack };
        replace_needle_in_haystack_inplace(result, needle, replacement);
        return result;
    }
}
