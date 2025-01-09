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

#include <gtest/gtest.h>
#include <path_to_regex.hpp>

namespace {

struct TestCase {
  std::string path;
  std::string testPath;
  bool matched = false;
  std::unordered_map<std::string, std::string> params;
  path_to_regex::case_sensitivity sensitivity = path_to_regex::case_sensitivity::case_sensitive;
};

std::ostream& operator<<(std::ostream& os, const TestCase& tc)
{
  os << "{ path: \"" << tc.path << "\", " << "testPath: \"" << tc.testPath << "\", " << "matched: " << std::boolalpha
     << tc.matched << ", " << "params: {";
  for (const auto& [key, value] : tc.params)
    os << "\"" << key << "\": \"" << value << "\", ";
  if (!tc.params.empty()) os.seekp(-2, std::ios_base::end);
  os << "} }";
  return os;
}

class Test : public ::testing::TestWithParam<TestCase> {};

TEST_P(Test, Test)
{
  const auto& test = GetParam();

  auto matcher = path_to_regex::match(test.path, test.sensitivity);
  auto [matched, params] = matcher(test.testPath);
  SCOPED_TRACE("Path: " + test.path + ", test path: " + test.testPath + ", matcher pattern: " + matcher.pattern());
  EXPECT_EQ(matched, test.matched);
  EXPECT_EQ(params, test.params);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Tests, Test, ::testing::Values(
  TestCase{"", "", true, {}},
  TestCase{"", "/", true, {}},
  TestCase{"", "/foo", false, {}},
  TestCase{"/", "", true, {}},
  TestCase{"/", "/", true, {}},
  TestCase{"/", "/foo", false, {}},

  TestCase{"/foo", "/", false, {}},
  TestCase{"/foo", "/foo", true, {}},
  TestCase{"/foo", "/foo/", true, {}},
  TestCase{"/foo", "/bar", false, {}},
  TestCase{"/foo", "/foo/bar", false, {}},

  TestCase{"/foo/", "/", false, {}},
  TestCase{"/foo/", "/foo", true, {}},
  TestCase{"/foo/", "/foo/", true, {}},
  TestCase{"/foo/", "/bar", false, {}},
  TestCase{"/foo/", "/foo/bar", false, {}},

  TestCase{"/:foo", "/", false, {}},
  TestCase{"/:foo", "/x", true, {{"foo", "x"}}},
  TestCase{"/:foo", "/x/", true, {{"foo", "x"}}},
  TestCase{"/:foo", "/x/y", false, {}},

  TestCase{"/:foo/", "/", false, {}},
  TestCase{"/:foo/", "/x", true, {{"foo", "x"}}},
  TestCase{"/:foo/", "/x/", true, {{"foo", "x"}}},
  TestCase{"/:foo/", "/x/y", false, {}},

  TestCase{"/:foo(\\d{3})/", "/x", false, {}},
  TestCase{"/:foo(\\d{3})/", "/1", false, {}},
  TestCase{"/:foo(\\d{3})/", "/111", true, {{"foo", "111"}}},
  TestCase{"/:foo(\\d{3})/", "/111/", true, {{"foo", "111"}}},

  TestCase{"/:foo/:bar", "/x", false, {}},
  TestCase{"/:foo/:bar", "/x/y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo/:bar", "/x/y/", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo.:bar", "/x.y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo.:bar", "/x.y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo/:bar", "/x/y/z", false, {}},

  TestCase{"/:foo/:bar/", "/x", false, {}},
  TestCase{"/:foo/:bar/", "/x/y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo/:bar/", "/x/y/", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo.:bar/", "/x.y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo.:bar/", "/x.y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo/:bar/", "/x/y/z", false, {}},

  TestCase{"/foo/:bar", "/foo", false, {}},
  TestCase{"/foo/:bar", "/foo/y", true, {{"bar", "y"}}},
  TestCase{"/foo/:bar", "/foo/y/", true, {{"bar", "y"}}},
  TestCase{"/foo.:bar", "/foo.y", true, {{"bar", "y"}}},
  TestCase{"/foo.:bar", "/foo.y/", true, {{"bar", "y"}}},
  TestCase{"/foo/:bar", "/foo/y/z", false, {}},

  TestCase{"/foo/:bar/", "/foo", false, {}},
  TestCase{"/foo/:bar/", "/foo/y", true, {{"bar", "y"}}},
  TestCase{"/foo/:bar/", "/foo/y/", true, {{"bar", "y"}}},
  TestCase{"/foo.:bar/", "/foo.y", true, {{"bar", "y"}}},
  TestCase{"/foo.:bar/", "/foo.y/", true, {{"bar", "y"}}},
  TestCase{"/foo/:bar/", "/foo/y/z", false, {}},

  TestCase{"/:foo/bar", "/x", false, {}},
  TestCase{"/:foo/bar", "/x/bar", true, {{"foo", "x"}}},
  TestCase{"/:foo/bar", "/x/bar/", true, {{"foo", "x"}}},
  TestCase{"/:foo.bar", "/x.bar", true, {{"foo", "x"}}},
  TestCase{"/:foo.bar", "/x.bar/", true, {{"foo", "x"}}},
  TestCase{"/:foo/bar", "/x/bar/z", false, {}},

  TestCase{"/:foo/bar/", "/x", false, {}},
  TestCase{"/:foo/bar/", "/x/bar", true, {{"foo", "x"}}},
  TestCase{"/:foo/bar/", "/x/bar/", true, {{"foo", "x"}}},
  TestCase{"/:foo.bar/", "/x.bar", true, {{"foo", "x"}}},
  TestCase{"/:foo.bar/", "/x.bar/", true, {{"foo", "x"}}},
  TestCase{"/:foo/bar/", "/x/bar/z", false, {}},

  TestCase{"{}", "", true, {}},
  TestCase{"{}", "/", true, {}},
  TestCase{"{}", "/foo", false, {}},
  TestCase{"{/}", "", true, {}},
  TestCase{"{/}", "/", true, {}},
  TestCase{"{/}", "/foo", false, {}},

  TestCase{"{/foo}", "/", true, {}},
  TestCase{"{/foo}", "/foo", true, {}},
  TestCase{"{/foo}", "/foo/", true, {}},
  TestCase{"{/foo}", "/bar", false, {}},
  TestCase{"{/foo}", "/foo/bar", false, {}},

  TestCase{"{/foo}/", "/", true, {}},
  TestCase{"{/foo}/", "/foo", true, {}},
  TestCase{"{/foo}/", "/foo/", true, {}},
  TestCase{"{/foo}/", "/bar", false, {}},
  TestCase{"{/foo}/", "/foo/bar", false, {}},

  TestCase{"{/:foo}", "", true, {{"foo", ""}}},
  TestCase{"{/:foo}", "/x", true, {{"foo", "x"}}},
  TestCase{"{/:foo}", "/x/", true, {{"foo", "x"}}},
  TestCase{"{/:foo}", "/x/y", false, {}},

  TestCase{"{/:foo}/", "", true, {{"foo", ""}}},
  TestCase{"{/:foo}/", "/x", true, {{"foo", "x"}}},
  TestCase{"{/:foo}/", "/x/", true, {{"foo", "x"}}},
  TestCase{"{/:foo}/", "/x/y", false, {}},

  TestCase{"{/:foo}/:bar", "", false, {}},
  TestCase{"{/:foo}/:bar", "/y", true, {{"foo", ""}, {"bar", "y"}}},
  TestCase{"{/:foo}/:bar", "/y/", true, {{"foo", ""}, {"bar", "y"}}},
  TestCase{"{/:foo}/:bar", "/x/y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"{/:foo}/:bar", "/x/y/", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"{/:foo/}:bar", "/x/y/z", false, {}},

  TestCase{"{/:foo}/:bar/", "", false, {}},
  TestCase{"{/:foo}/:bar/", "/y", true, {{"foo", ""}, {"bar", "y"}}},
  TestCase{"{/:foo}/:bar/", "/y/", true, {{"foo", ""}, {"bar", "y"}}},
  TestCase{"{/:foo}/:bar/", "/x/y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"{/:foo}/:bar/", "/x/y/", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"{/:foo/}:bar/", "/x/y/z", false, {}},

  TestCase{"/:foo{/:bar}", "", false, {}},
  TestCase{"/:foo{/:bar}", "/x", true, {{"foo", "x"}, {"bar", ""}}},
  TestCase{"/:foo{/:bar}", "/x/", true, {{"foo", "x"}, {"bar", ""}}},
  TestCase{"/:foo{/:bar}", "/x/y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo{/:bar}", "/x/y/", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo{/:bar}", "/x/y/z", false, {}},

  TestCase{"/:foo{/:bar}/", "", false, {}},
  TestCase{"/:foo{/:bar}/", "/x", true, {{"foo", "x"}, {"bar", ""}}},
  TestCase{"/:foo{/:bar}/", "/x/", true, {{"foo", "x"}, {"bar", ""}}},
  TestCase{"/:foo{/:bar}/", "/x/y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo{/:bar}/", "/x/y/", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo{/:bar}/", "/x/y/z", false, {}},

  TestCase{"/*foo", "", false, {}},
  TestCase{"/*foo", "/", false, {}},
  TestCase{"/*foo", "/x", true, {{"foo", "x"}}},
  TestCase{"/*foo", "/x/", true, {{"foo", "x"}}},
  TestCase{"/*foo", "/x/y", true, {{"foo", "x/y"}}},
  TestCase{"/*foo", "/x/y/", true, {{"foo", "x/y"}}},

  TestCase{"/foo/*bar", "", false, {}},
  TestCase{"/foo/*bar", "/", false, {}},
  TestCase{"/foo/*bar", "/x", false, {}},
  TestCase{"/foo/*bar", "/foo/x", true, {{"bar", "x"}}},
  TestCase{"/foo/*bar", "/foo/x/", true, {{"bar", "x"}}},
  TestCase{"/foo/*bar", "/foo/x/y", true, {{"bar", "x/y"}}},
  TestCase{"/foo/*bar", "/foo/x/y/", true, {{"bar", "x/y"}}},

  TestCase{"/:foo/*bar", "", false, {}},
  TestCase{"/:foo/*bar", "/", false, {}},
  TestCase{"/:foo/*bar", "/x", false, {}},
  TestCase{"/:foo/*bar", "/x/y", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo/*bar", "/x/y/", true, {{"foo", "x"}, {"bar", "y"}}},
  TestCase{"/:foo/*bar", "/x/y/z", true, {{"foo", "x"}, {"bar", "y/z"}}},
  TestCase{"/:foo/*bar", "/x/y/z/", true, {{"foo", "x"}, {"bar", "y/z"}}},

  TestCase{"/café", "/café", true, {}},
  TestCase{"/café", "/café/", true, {}},
  TestCase{"/café", "/caf%C3%A9", true, {}},
  TestCase{"/café", "/caf%C3%A9/", true, {}},
  TestCase{"/caf%C3%A9", "/café", true, {}},
  TestCase{"/caf%C3%A9", "/café/", true, {}},
  TestCase{"/a%2Fb", "/a%2Fb", true, {}},

  TestCase{"/:café", "/x", true, {{"café", "x"}}},
  TestCase{"/:café", "/x/", true, {{"café", "x"}}},
  TestCase{"/:caf%C3%A9", "/x", true, {{"café", "x"}}},
  TestCase{"/:caf%C3%A9", "/x/", true, {{"café", "x"}}},
  TestCase{"/:foo", "/a%2Fb", true, {{"foo", "a/b"}}},

  TestCase{"/;,:@&=+$-_.!~*()", "/;,:@&=+$-_.!~*()", true, {}},
  TestCase{"/:foo", "/;,:@&=+$-_.!~*()", true, {{"foo", ";,:@&=+$-_.!~*()"}}},
  TestCase{"/:foo", "/param%2523", true, {{"foo", "param%23"}}},

  TestCase{"C:\\foo", "C:\\", false, {}},
  TestCase{"C:\\foo", "C:\\foo", true, {}},
  TestCase{"C:\\foo", "C:\\foo\\", true, {}},
  TestCase{"C:\\foo", "C:\\bar", false, {}},
  TestCase{"C:\\foo", "C:\\foo\\bar", false, {}},

  TestCase{"C:\\:foo", "C:\\", false, {}},
  TestCase{"C:\\:foo", "C:\\x", true, {{"foo", "x"}}},
  TestCase{"C:\\:foo", "C:\\x\\", true, {{"foo", "x"}}},
  TestCase{"C:\\:foo", "C:\\x\\y", false, {}},

  TestCase{"C:\\:foo\\", "C:\\", false, {}},
  TestCase{"C:\\:foo\\", "C:\\x", true, {{"foo", "x"}}},
  TestCase{"C:\\:foo\\", "C:\\x\\", true, {{"foo", "x"}}},
  TestCase{"C:\\:foo\\", "C:\\x\\y", false, {}},

  TestCase{"/foo", "/FOO", false, {}, path_to_regex::case_sensitivity::case_sensitive},
  TestCase{"/foo", "/FOO", true, {}, path_to_regex::case_sensitivity::case_insensitive},
  TestCase{"/foo/bar", "/FOO/BAR", false, {}, path_to_regex::case_sensitivity::case_sensitive},
  TestCase{"/foo/bar", "/FOO/BAR", true, {}, path_to_regex::case_sensitivity::case_insensitive}
));
// clang-format on

} // namespace

GTEST_API_ int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
