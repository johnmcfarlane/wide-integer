///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2021.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdint>
#include <ctime>
#include <random>
#include <sstream>
#include <string>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#if (defined(__clang__) && (__clang_major__ > 9)) && !defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif

#include <boost/multiprecision/cpp_int.hpp>

#include <math/wide_integer/uintwide_t.h>
#include <test/test_uintwide_t.h>

namespace local_float_convert
{
  auto engine_man() -> std::mt19937&                                                         { static std::mt19937                                                         my_engine_man; return my_engine_man; } // NOLINT(cert-msc32-c,cert-msc51-cpp)
  auto engine_sgn() -> std::ranlux24_base&                                                   { static std::ranlux24_base                                                   my_engine_sgn; return my_engine_sgn; } // NOLINT(cert-msc32-c,cert-msc51-cpp)
  auto engine_e10() -> std::linear_congruential_engine<std::uint32_t, 48271, 0, 2147483647>& { static std::linear_congruential_engine<std::uint32_t, 48271, 0, 2147483647> my_engine_e10; return my_engine_e10; } // NOLINT(cert-msc32-c,cert-msc51-cpp)

  template<typename FloatingPointType,
           const std::int32_t LoExp10,
           const std::int32_t HiExp10>
  auto get_random_float() -> FloatingPointType
  {
    using local_builtin_float_type = FloatingPointType;

    static std::uniform_real_distribution<local_builtin_float_type>
    dist_man
    (
      local_builtin_float_type(0.0F),
      local_builtin_float_type(1.0F)
    );

    static std::uniform_int_distribution<unsigned>
    dist_sgn
    (
      0,
      1
    );

    static std::uniform_int_distribution<std::int32_t>
    dist_e10
    (
      LoExp10,
      HiExp10
    );

    using std::pow;

    const std::int32_t             p10 = dist_e10(engine_e10());
    const local_builtin_float_type e10 = pow(local_builtin_float_type(10.0F),
                                             local_builtin_float_type(p10));

    const local_builtin_float_type a = dist_man(engine_man()) * e10;

    const bool is_neg = (dist_sgn(engine_sgn()) != 0);

    const local_builtin_float_type f = ((!is_neg) ? a : -a);

    return f;
  }

  template<const std::size_t MaxDigitsToGet,
           const std::size_t MinDigitsToGet = 2U>
  void get_random_digit_string(std::string& str)
  {
    static_assert(MinDigitsToGet >=  2U, "Error: The minimum number of digits to get must be  2 or more");

    static std::uniform_int_distribution<unsigned>
    dist_sgn
    (
      0,
      1
    );

    static std::uniform_int_distribution<unsigned>
    dist_len
    (
      MinDigitsToGet,
      MaxDigitsToGet
    );

    static std::uniform_int_distribution<unsigned>
    dist_first
    (
      1,
      9
    );

    static std::uniform_int_distribution<unsigned>
    dist_following
    (
      0,
      9
    );

    const bool is_neg = (dist_sgn(engine_sgn()) != 0);

    const auto len = static_cast<std::string::size_type>(dist_len(engine_e10()));

    std::string::size_type pos = 0U;

    if(is_neg)
    {
      str.resize(len + 1U);

      str.at(pos) = char('-');

      ++pos;
    }
    else
    {
      str.resize(len);
    }

    str.at(pos) = static_cast<char>(dist_first(engine_man()) + 0x30U);

    ++pos;

    while(pos < str.length())
    {
      str.at(pos) = static_cast<char>(dist_following(engine_man()) + 0x30U);

      ++pos;
    }
  }

  template<typename UnsignedIntegralType>
  static auto hexlexical_cast(const UnsignedIntegralType& u) -> std::string
  {
    std::stringstream ss;

    ss << std::hex << u;

    return ss.str();
  }
} // namespace local_float_convert

auto math::wide_integer::test_uintwide_t_float_convert() -> bool
{
  constexpr unsigned digits2 = 256U;

  using boost_uint_backend_type =
    boost::multiprecision::cpp_int_backend<digits2,
                                           digits2,
                                           boost::multiprecision::unsigned_magnitude>;
  using boost_sint_backend_type =
    boost::multiprecision::cpp_int_backend<digits2,
                                           digits2,
                                           boost::multiprecision::signed_magnitude>;

  using boost_uint_type = boost::multiprecision::number<boost_uint_backend_type, boost::multiprecision::et_on>;
  using boost_sint_type = boost::multiprecision::number<boost_sint_backend_type, boost::multiprecision::et_on>;

  #if defined(WIDE_INTEGER_HAS_LIMB_TYPE_UINT64)
  using local_limb_type = std::uint64_t;
  #else
  using local_limb_type = std::uint32_t;
  #endif

  using local_uint_type = math::wide_integer::uintwide_t<digits2, local_limb_type, void>;
  using local_sint_type = math::wide_integer::uintwide_t<digits2, local_limb_type, void, true>;

  local_float_convert::engine_man().seed(static_cast<typename std::mt19937::result_type>                                                        (std::clock()));
  local_float_convert::engine_sgn().seed(static_cast<typename std::ranlux24_base::result_type>                                                  (std::clock()));
  local_float_convert::engine_e10().seed(static_cast<typename std::linear_congruential_engine<std::uint32_t, 48271, 0, 2147483647>::result_type>(std::clock()));

  bool result_is_ok = true;

  for(auto i = std::size_t(0U); i < std::size_t(0x80000U); ++i)
  {
    const auto f = local_float_convert::get_random_float<float, -1, 27>();

    boost_sint_type n_boost = boost_sint_type(f);
    local_sint_type n_local = local_sint_type(f);

    const std::string str_boost_signed = local_float_convert::hexlexical_cast(static_cast<boost_uint_type>(n_boost));
    const std::string str_local_signed = local_float_convert::hexlexical_cast(static_cast<local_uint_type>(n_local));

    result_is_ok &= (str_boost_signed == str_local_signed);
  }

  for(auto i = std::size_t(0U); i < std::size_t(0x80000U); ++i)
  {
    const auto d = local_float_convert::get_random_float<double, -1, 75>();

    boost_sint_type n_boost = boost_sint_type(d);
    local_sint_type n_local = local_sint_type(d);

    const std::string str_boost_signed = local_float_convert::hexlexical_cast(static_cast<boost_uint_type>(n_boost));
    const std::string str_local_signed = local_float_convert::hexlexical_cast(static_cast<local_uint_type>(n_local));

    result_is_ok &= (str_boost_signed == str_local_signed);
  }

  local_float_convert::engine_man().seed(static_cast<typename std::mt19937::result_type>                                                        (std::clock()));
  local_float_convert::engine_sgn().seed(static_cast<typename std::ranlux24_base::result_type>                                                  (std::clock()));
  local_float_convert::engine_e10().seed(static_cast<typename std::linear_congruential_engine<std::uint32_t, 48271, 0, 2147483647>::result_type>(std::clock()));

  for(auto i = std::size_t(0U); i < std::size_t(0x100000U); ++i)
  {
    std::string str_digits;

    local_float_convert::get_random_digit_string<31U>(str_digits);

    const boost_sint_type n_boost = boost_sint_type(str_digits.c_str());
    const local_sint_type n_local = local_sint_type(str_digits.c_str());

    const auto f_boost = static_cast<float>(n_boost);
    const auto f_local = static_cast<float>(n_local);

    using std::fabs;

    constexpr auto cast_tol_float = float(std::numeric_limits<float>::epsilon() * 2.0F);

    const float closeness      = fabs(1.0F - fabs(f_boost / f_local));
    const bool  result_f_is_ok = (closeness < cast_tol_float);

    result_is_ok &= result_f_is_ok;
  }

  for(auto i = std::size_t(0U); i < std::size_t(0x40000U); ++i)
  {
    std::string str_digits;

    local_float_convert::get_random_digit_string<71U>(str_digits);

    const boost_sint_type n_boost = boost_sint_type(str_digits.c_str());
    const local_sint_type n_local = local_sint_type(str_digits.c_str());

    const auto d_boost = static_cast<double>(n_boost);
    const auto d_local = static_cast<double>(n_local);

    using std::fabs;

    constexpr auto cast_tol_double = double(std::numeric_limits<double>::epsilon() * 2.0);

    const double closeness      = fabs(1.0 - fabs(d_boost / d_local));
    const bool   result_f_is_ok = (closeness < cast_tol_double);

    result_is_ok &= result_f_is_ok;
  }

  return result_is_ok;
}

#if (defined(__clang__) && (__clang_major__ > 9)) && !defined(__APPLE__)
#pragma GCC diagnostic pop
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#endif
