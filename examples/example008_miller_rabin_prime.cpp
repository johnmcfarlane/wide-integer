///////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2018 - 2020.                 //
//  Distributed under the Boost Software License,                //
//  Version 1.0. (See accompanying file LICENSE_1_0.txt          //
//  or copy at http://www.boost.org/LICENSE_1_0.txt)             //
///////////////////////////////////////////////////////////////////

// This Miller-Rabin primality test is loosely based on
// an adaptation of some code from Boost.Multiprecision.
// The Boost.Multiprecision code can be found here:
// https://www.boost.org/doc/libs/1_73_0/libs/multiprecision/doc/html/boost_multiprecision/tut/primetest.html

#include <random>

#include <math/wide_integer/uintwide_t.h>

bool math::wide_integer::example008_miller_rabin_prime()
{
  using wide_integer_type  = math::wide_integer::uintwide_t<256U>;
  using distribution_type  = math::wide_integer::uniform_int_distribution<wide_integer_type::my_digits, typename wide_integer_type::limb_type>;
  using random_engine_type = std::linear_congruential_engine<std::uint32_t, UINT32_C(48271), UINT32_C(0), UINT32_C(2147483647)>;

  // Use a fixed seed in order to obtain deterministic
  // and reproducible result for this test.

  random_engine_type generator1(305419969ULL);
  random_engine_type generator2;

  distribution_type distribution1;
  distribution_type distribution2;

  wide_integer_type p0;
  wide_integer_type p1;

  for(;;)
  {
    p0 = distribution1(generator1);

    const bool miller_rabin_result = miller_rabin(p0,
                                                  25U,
                                                  distribution2,
                                                  generator2);

    if(miller_rabin_result)
    {
      break;
    }
  }

  for(;;)
  {
    p1 = distribution1(generator1);

    const bool miller_rabin_result = miller_rabin(p1,
                                                  25U,
                                                  distribution2,
                                                  generator2);

    if(miller_rabin_result)
    {
      break;
    }
  }

  const wide_integer_type d = gcd(p0, p1);

  const bool result_is_ok = (   (p0 != 0U)
                             && (p1 != 0U)
                             && (p0 != p1)
                             && (d  == 1U));

  return result_is_ok;
}

// Enable this if you would like to activate this main() as a standalone example.
#if 0

#include <iomanip>
#include <iostream>

int main()
{
  const bool result_is_ok = wide_integer::example008_miller_rabin_prime();

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
}

#endif
