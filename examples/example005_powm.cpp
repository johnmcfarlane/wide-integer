///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2018 - 2022.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

#include <examples/example_uintwide_t.h>
#include <math/wide_integer/uintwide_t.h>

auto math::wide_integer::example005_powm() -> bool
{
  using math::wide_integer::uint256_t;

  WIDE_INTEGER_CONSTEXPR uint256_t b("0xDA4033C9B1B0675C20B7879EA63FFFBEEBEC3F89F78D22C393FAD98E7AE9BF69");
  WIDE_INTEGER_CONSTEXPR uint256_t p("0xA4748AD2DAFEED29C73927BD0945EF45EFEC9DAA95CC59390D406FC27236A174");
  WIDE_INTEGER_CONSTEXPR uint256_t m("0xB6EC4DAB21E2856D488D669C210DC1FAD00366F92D602B1D42B88E24531F907E");

  const uint256_t c = powm(b, p, m);

  const bool result_is_ok =
    (c == "0x5231F0EF6BBB3E78B9D7B1FA5F86EFA932E71BABD8A1CFF2C9EE5C396284ED07");

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if 0

#include <iomanip>
#include <iostream>

int main()
{
  const bool result_is_ok = wide_integer::example005_powm();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif
