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
