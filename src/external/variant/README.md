# README #

This is an implementation of a C++ `variant` class, matching the C++17 draft http://wg21.link/n4606

The default constructor default-constructs the first type. If the first type
isn't default-constructible then there is no default constructor.

If a copy or move or emplace assignment operation throws then the variant is put
in a special "valueless by exception" state, which can be queried with
`valueless_by_exception()`.

It has been tested with gcc 6.1.1-2ubuntu12~16.04 and clang 3.8.0-2ubuntu4 on Ubuntu Linux.

It is provided as a single header file released under the BSD license.

Copyright (c) 2015-2016, Just Software Solutions Ltd
