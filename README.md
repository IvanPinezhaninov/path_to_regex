# Path-to-Regex for C++

[![Language C++17](https://img.shields.io/badge/language-C%2B%2B17-004482?style=flat-square
)](https://en.cppreference.com/w/cpp/17) [![MIT License](https://img.shields.io/github/license/IvanPinezhaninov/path_to_regex?label=license&style=flat-square)](http://opensource.org/licenses/MIT) [![Build status](https://img.shields.io/github/actions/workflow/status/IvanPinezhaninov/path_to_regex/build-and-test.yml?style=flat-square
)](https://github.com/IvanPinezhaninov/path_to_regex/actions/workflows/build-and-test.yml) [![Codecov](https://img.shields.io/codecov/c/github/IvanPinezhaninov/path_to_regex?style=flat-square)](https://codecov.io/gh/IvanPinezhaninov/path_to_regex)

## Overview
A C++ header-only library inspired by [path-to-regexp](https://github.com/pillarjs/path-to-regexp), designed for compiling path patterns into regular expressions and extracting dynamic segments from paths.

## Usage

```cpp
#include <iostream>
#include <path_to_regex.hpp>

int main()
{
  auto matcher = path_to_regex::match("/api/v1/download/:file{.:ext}");

  auto [matched, params] = matcher("/api/v1/download/archive.zip");
  if (matched)
    std::cout << "File '" << params["file"] << "' with extension '" << params["ext"] << "' requested" << std::endl;
  else
    std::cerr << "Wrong path" << std::endl;

  return EXIT_SUCCESS;
}
```

### Parameters
Parameters capture arbitrary strings in a path by matching up to the end of a segment or the next token. They are defined by placing a colon before the parameter name (:foo), with names consisting of alphanumeric characters.
```cpp
auto matcher = path_to_regex::match("/:category/:id");

auto [matched, params] = matcher("/products/123");
//=> matched: true, params: {{"category", "products"}, {"id", "123"}}
```

### Optional
Braces can be used to specify optional sections within a path.
```cpp
auto matcher = path_to_regex::match("/download/:file{.:ext}");

auto [matched, params] = matcher("/download/archive");
//=> matched: true, params: {{ "file", "archive"}, {"ext", ""}}

auto [matched, params] = matcher("/download/archive.zip");
//=> matched: true, params: {{ "file", "archive"}, {"ext", "zip"}}
```

### Wildcard
Wildcard parameters can match multiple characters across several segments. They are similar to regular parameters but are defined by prefixing the parameter name with an asterisk (*foo).
```cpp
auto matcher = path_to_regex::match("/*foo");

matcher("/bar/baz");
//=> matched: true, params: {{ "foo", "bar/baz"}}
```

## License
This code is distributed under the [MIT License](LICENSE)

## Author
[Ivan Pinezhaninov](mailto:ivan.pinezhaninov@gmail.com)
