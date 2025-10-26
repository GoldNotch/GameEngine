#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <Utility/StaticString.hpp>
using namespace GameFramework;

TEST_CASE("Construct & Output", "[static_string]")
{
  constexpr static_string t = "Hello world";
  REQUIRE(t.length() == 11);
  REQUIRE(!t.empty());
  std::cout << t << std::endl;
}

TEST_CASE("Convert to std::string", "[static_string]")
{
  constexpr static_string t = "Hello world";
  REQUIRE(t.length() == 11);
  REQUIRE(!t.empty());
  auto new_str = static_cast<std::string>(t);
  new_str[4] = '5';
  std::cout << new_str << std::endl;
}

TEST_CASE("Concatenation", "[static_string]")
{
  constexpr static_string t1 = "Hello";
  constexpr static_string t2 = " world";
  static_string t("");
  constexpr auto new_str = t1.append(t2) + static_string(" test");
  std::cout << new_str << std::endl;
}


TEST_CASE("get substring", "[static_string]")
{
  constexpr static_string t = "Hello world";
  constexpr auto new_str = t.substring<0, 5>();
  constexpr auto suffix = t.suffix<6>();
  std::cout << new_str << suffix << std::endl;
}
