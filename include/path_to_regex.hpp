/******************************************************************************
**
** Copyright (C) 2025 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
**
** This file is part of the Path-to-Regex which can be found at
** https://github.com/IvanPinezhaninov/path_to_regex/.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
** OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
******************************************************************************/

#ifndef PATH_TO_REGEX_H
#define PATH_TO_REGEX_H

#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace path_to_regex {

/**
 * @enum case_sensitivity
 * @brief Enum class to specify case sensitivity options.
 *
 * Indicates whether path comparison should be case-sensitive or case-insensitive.
 */
enum class case_sensitivity {
  case_sensitive,  ///< Path comparison should consider the case of the characters.
  case_insensitive ///< Path comparison should ignore the case of the characters.
};

namespace details {

inline std::string percent_encode(std::string_view str)
{
  constexpr auto hex_chars = "0123456789ABCDEF";
  constexpr std::string_view special_chars = R"(!"#$%&'()*+,-./:;<=>?@[\]^_{|}~`)";

  std::string encoded;
  encoded.reserve(str.size() * 3);

  for (unsigned char ch : str) {
    if (std::isalnum(ch) || special_chars.find(ch) != std::string_view::npos) {
      encoded.push_back(ch);
    } else {
      encoded.push_back('%');
      encoded.push_back(hex_chars[ch >> 4]);
      encoded.push_back(hex_chars[ch & 0x0F]);
    }
  }

  return encoded;
}

inline std::string percent_decode(std::string_view str)
{
  std::string decoded;
  decoded.reserve(str.size());

  for (size_t i = 0; i < str.size();) {
    if (str[i] == '%' && i + 2 < str.size() && std::isxdigit(str[i + 1]) && std::isxdigit(str[i + 2])) {
      char ch = 0;
      for (int j = 1; j < 3; ++j) {
        ch <<= 4;
        char hex = str[i + j];
        ch |= (hex >= '0' && hex <= '9') ? (hex - '0') : (std::toupper(hex) - 'A' + 10);
      }
      decoded.push_back(ch);
      i += 3;
    } else {
      decoded.push_back(str[i]);
      ++i;
    }
  }

  return decoded;
}

inline char find_separator(std::string_view path)
{
  return (path.find('/') <= path.find('\\')) ? '/' : '\\';
}

inline std::string make_pattern(const std::string& path, std::vector<std::string>& keys, char separator)
{
  // Regex pattern structure:    | Optional |      Required       | Wildcard |    Special Chars    |
  static auto rx = std::regex{R"(\{([^}]*)\}|:([\w%]+)(\([^)]+\))?|\*([\w%]+)|([.\^$*+?()|\[\]{}\\]))"};
  std::sregex_iterator it{path.cbegin(), path.cend(), rx};
  std::sregex_iterator end;
  size_t last_pos = 0;
  std::string pattern;

  constexpr auto optional_param_key_idx = 1;
  constexpr auto required_param_key_idx = 2;
  constexpr auto required_param_pattern_idx = 3;
  constexpr auto wildcard_param_key_idx = 4;
  constexpr auto special_chars_key_idx = 5;

  for (; it != end; ++it) {
    auto match = *it;

    if (last_pos < match.position()) pattern += path.substr(last_pos, match.position() - last_pos);

    if (match[optional_param_key_idx].matched) {
      auto subpattern = make_pattern(match[optional_param_key_idx].str(), keys, separator);
      if (!subpattern.empty()) pattern += "(?:" + subpattern + ")?";
    } else if (match[required_param_key_idx].matched) {
      keys.push_back(percent_decode(match[required_param_key_idx].str()));
      auto subpattern = match[required_param_pattern_idx].str();
      if (subpattern.empty()) {
        pattern += "([^\\";
        pattern += separator;
        pattern += "]+?)";
      } else {
        pattern += subpattern;
      }
    } else if (match[wildcard_param_key_idx].matched) {
      keys.push_back(percent_decode(match[wildcard_param_key_idx].str()));
      pattern += "(\\S+?)";
    } else if (match[special_chars_key_idx].matched) {
      pattern += '\\';
      pattern += match[special_chars_key_idx].str();
    }

    last_pos = match.position() + match.length();
  }

  if (last_pos < path.length()) pattern += path.substr(last_pos, path.length() - last_pos);

  return pattern;
}

inline std::string make_pattern(std::string_view path, std::vector<std::string>& keys)
{
  auto encoded_path = percent_encode(path);
  auto separator = find_separator(path);
  auto pattern = make_pattern(encoded_path, keys, separator);
  if (pattern.empty() || pattern.back() != separator) {
    pattern += '\\';
    pattern += separator;
  }
  return '^' + pattern + "?$";
}

inline std::regex_constants::syntax_option_type make_regex_flags(path_to_regex::case_sensitivity sensitivity)
{
  auto flags = std::regex_constants::ECMAScript;
  if (sensitivity == path_to_regex::case_sensitivity::case_insensitive) flags |= std::regex_constants::icase;
  return flags;
}

} // namespace details

/**
 * @class matcher
 * @brief Matches paths against a compiled regular expression pattern.
 *
 * Compiles a path pattern into a regular expression and matches paths,
 * extracting params from them.
 */
class matcher {
public:
  /**
   * @struct result
   * @brief Result of a path match operation.
   *
   * Indicates whether the path matched and contains extracted params if matched.
   */
  struct result {
    bool matched = false;                                ///< True if the path matched the pattern.
    std::unordered_map<std::string, std::string> params; ///< Extracted params from the matched path.
  };

  matcher(std::string pattern, std::vector<std::string> keys, case_sensitivity sensitivity)
    : m_pattern{std::move(pattern)}
    , m_regex{m_pattern, details::make_regex_flags(sensitivity)}
    , m_keys{std::move(keys)}
  {}

  /**
   * @brief Matches a path against the compiled pattern.
   *
   * Matches the path to the pattern and extracts params if a match is found.
   *
   * @param path Path to match.
   * @return A `result` indicating match status and params.
   *
   * @see result
   */
  matcher::result operator()(std::string_view path) const
  {
    auto encoded_path = details::percent_encode(path);

    std::smatch match;
    result res = {std::regex_match(encoded_path, match, m_regex)};

    if (res.matched) {
      for (size_t i = 0; i < m_keys.size(); ++i)
        res.params[m_keys[i]] = details::percent_decode(match[i + 1].str());
    }

    return res;
  }

  /**
   * @brief Returns the original pattern string.
   *
   * @return The pattern string used for matching.
   */
  std::string pattern() const
  {
    return m_pattern;
  }

private:
  std::string m_pattern;
  std::regex m_regex;
  std::vector<std::string> m_keys;
};

/**
 * @brief Compiles a path pattern into a matcher.
 *
 * Converts a path pattern into a regular expression and
 * returns a matcher that can validate paths against it.
 *
 * @param path The path pattern.
 * @param sensitivity The case sensitivity option for matching.
 *                    Defaults to `case_sensitivity::case_sensitive`.
 * @return A `matcher` object with the compiled regular expression.
 *
 * @see matcher
 */
inline matcher match(std::string_view path, case_sensitivity sensitivity = case_sensitivity::case_sensitive)
{
  std::vector<std::string> keys;
  auto pattern = details::make_pattern(path, keys);
  return {std::move(pattern), std::move(keys), sensitivity};
}

} // namespace path_to_regex

#endif // PATH_TO_REGEX_H
