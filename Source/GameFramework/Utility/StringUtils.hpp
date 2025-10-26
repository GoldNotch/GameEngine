#pragma once

#include <string>
#include <string_view>

namespace GameFramework::Utils
{

constexpr std::string_view Trim(std::string_view text, std::string_view exclude = " \n\r\t\v\0")
{
  if (const size_t leftShift = text.find_first_not_of(exclude); leftShift != std::string_view::npos)
  {
    text.remove_prefix(leftShift);
  }
  else
  {
    return {};
  }

  if (const size_t rightShift = text.find_last_not_of(exclude);
      rightShift != std::string_view::npos)
  {
    text.remove_suffix(text.size() - rightShift - 1);
  }
  else
  {
    return {};
  }

  return text;
}

constexpr std::vector<std::string_view> Split(std::string_view text, char delimiter)
{
  std::vector<std::string_view> result;
  size_t begin = 0, end = -1;
  do
  {
    begin = end + 1;
    end = text.find(delimiter, begin);
    if (end != std::string::npos)
      result.push_back(text.substr(begin, end - begin));
    else
      result.push_back(text.substr(begin, text.size() - begin));
  } while (end != std::string::npos);
  return result;
}

} // namespace GameFramework::utils
