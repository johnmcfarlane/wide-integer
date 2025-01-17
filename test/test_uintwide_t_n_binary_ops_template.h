///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2019 - 2022.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TEST_UINTWIDE_T_N_BINARY_OPS_TEMPLATE_2019_12_19_H
  #define TEST_UINTWIDE_T_N_BINARY_OPS_TEMPLATE_2019_12_19_H

  #include <algorithm>
  #include <atomic>
  #include <cstddef>
  #include <random>
  #include <vector>

  #include <test/test_uintwide_t_n_binary_ops_base.h>

  template<const math::wide_integer::size_t MyDigits2,
           typename MyLimbType = std::uint32_t,
           typename AllocatorType = void>
  class test_uintwide_t_n_binary_ops_template : public test_uintwide_t_n_binary_ops_base // NOLINT(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
  {
  private:
    static constexpr math::wide_integer::size_t digits2 = MyDigits2;

    using boost_uint_backend_type =
      boost::multiprecision::cpp_int_backend<digits2,
                                             digits2,
                                             boost::multiprecision::unsigned_magnitude>;

    using boost_uint_type = boost::multiprecision::number<boost_uint_backend_type, boost::multiprecision::et_on>;

    using local_limb_type = MyLimbType;

    using local_uint_type = math::wide_integer::uintwide_t<digits2, local_limb_type, AllocatorType>;

  public:
    explicit test_uintwide_t_n_binary_ops_template(const std::size_t count)
      : test_uintwide_t_n_binary_ops_base(count),
        a_local(),
        b_local(),
        a_boost(),
        b_boost() { }

    ~test_uintwide_t_n_binary_ops_template() override = default;

    auto get_digits2() const -> math::wide_integer::size_t override { return digits2; }

    void initialize() override
    {
      a_local.clear();
      b_local.clear();

      a_boost.clear();
      b_boost.clear();

      a_local.resize(size());
      b_local.resize(size());

      a_boost.resize(size());
      b_boost.resize(size());

      get_equal_random_test_values_boost_and_local_n<local_uint_type, boost_uint_type, AllocatorType>(a_local.data(), a_boost.data(), size());
      get_equal_random_test_values_boost_and_local_n<local_uint_type, boost_uint_type, AllocatorType>(b_local.data(), b_boost.data(), size());
    }

    auto test_binary_add() const -> bool override
    {
      bool result_is_ok = true;

      std::atomic_flag test_lock = ATOMIC_FLAG_INIT;

      my_concurrency::parallel_for
      (
        std::size_t(0U),
        size(),
        [&test_lock, &result_is_ok, this](std::size_t i)
        {
          const boost_uint_type c_boost = a_boost[i] + b_boost[i];
          const local_uint_type c_local = a_local[i] + b_local[i];

          const std::string str_boost = hexlexical_cast(c_boost);
          const std::string str_local = hexlexical_cast(c_local);

          while(test_lock.test_and_set()) { ; }
          result_is_ok &= (str_boost == str_local);
          test_lock.clear();
        }
      );

      return result_is_ok;
    }

    auto test_binary_sub() const -> bool override
    {
      bool result_is_ok = true;

      std::atomic_flag test_lock = ATOMIC_FLAG_INIT;

      my_concurrency::parallel_for
      (
        std::size_t(0U),
        size(),
        [&test_lock, &result_is_ok, this](std::size_t i)
        {
          const boost_uint_type c_boost = a_boost[i] - b_boost[i];
          const local_uint_type c_local = a_local[i] - b_local[i];

          const std::string str_boost = hexlexical_cast(c_boost);
          const std::string str_local = hexlexical_cast(c_local);

          while(test_lock.test_and_set()) { ; }
          result_is_ok &= (str_boost == str_local);
          test_lock.clear();
        }
      );

      return result_is_ok;
    }

    auto test_binary_mul() const -> bool override
    {
      bool result_is_ok = true;

      std::atomic_flag test_lock = ATOMIC_FLAG_INIT;

      my_concurrency::parallel_for
      (
        std::size_t(0U),
        size(),
        [&test_lock, &result_is_ok, this](std::size_t i)
        {
          const boost_uint_type c_boost = a_boost[i] * b_boost[i];
          const local_uint_type c_local = a_local[i] * b_local[i];

          const std::string str_boost = hexlexical_cast(c_boost);
          const std::string str_local = hexlexical_cast(c_local);

          while(test_lock.test_and_set()) { ; }
          result_is_ok &= (str_boost == str_local);
          test_lock.clear();
        }
      );

      return result_is_ok;
    }

    auto test_binary_div() const -> bool override
    {
      std::atomic_flag test_lock = ATOMIC_FLAG_INIT;

      my_gen().seed(static_cast<typename random_generator_type::result_type>(std::clock()));
      std::uniform_int_distribution<> dis(1, static_cast<int>(digits2 - 1U));

      bool result_is_ok = true;

      my_concurrency::parallel_for
      (
        std::size_t(0U),
        size(),
        [&result_is_ok, this, &dis, &test_lock](std::size_t i)
        {
          while(test_lock.test_and_set()) { ; }
          const auto right_shift_amount = static_cast<std::size_t>(dis(my_gen()));
          test_lock.clear();

          const boost_uint_type c_boost = a_boost[i] / (std::max)(boost_uint_type(1U), boost_uint_type(b_boost[i] >> right_shift_amount));
          const local_uint_type c_local = a_local[i] / (std::max)(local_uint_type(1U),                (b_local[i] >> right_shift_amount));

          const std::string str_boost = hexlexical_cast(c_boost);
          const std::string str_local = hexlexical_cast(c_local);

          while(test_lock.test_and_set()) { ; }
          result_is_ok &= (str_boost == str_local);
          test_lock.clear();
        }
      );

      return result_is_ok;
    }

    auto test_binary_mod() const -> bool override
    {
      std::atomic_flag test_lock = ATOMIC_FLAG_INIT;

      my_gen().seed(static_cast<typename random_generator_type::result_type>(std::clock()));
      std::uniform_int_distribution<> dis(1, static_cast<int>(digits2 - 1U));

      bool result_is_ok = true;

      my_concurrency::parallel_for
      (
        std::size_t(0U),
        size(),
        [&result_is_ok, this, &dis, &test_lock](std::size_t i)
        {
          while(test_lock.test_and_set()) { ; }
          const auto right_shift_amount = static_cast<std::size_t>(dis(my_gen()));
          test_lock.clear();

          const boost_uint_type c_boost = a_boost[i] % (std::max)(boost_uint_type(1U), boost_uint_type(b_boost[i] >> right_shift_amount));
          const local_uint_type c_local = a_local[i] % (std::max)(local_uint_type(1U), (b_local[i] >> right_shift_amount));

          const std::string str_boost = hexlexical_cast(c_boost);
          const std::string str_local = hexlexical_cast(c_local);

          while(test_lock.test_and_set()) { ; }
          result_is_ok &= (str_boost == str_local);
          test_lock.clear();
        }
      );

      return result_is_ok;
    }

    auto test_binary_sqrt() const -> bool override
    {
      bool result_is_ok = true;

      std::atomic_flag test_lock = ATOMIC_FLAG_INIT;

      my_concurrency::parallel_for
      (
        std::size_t(0U),
        size(),
        [&test_lock, &result_is_ok, this](std::size_t i)
        {
          const boost_uint_type c_boost = sqrt(a_boost[i]);
          const local_uint_type c_local = sqrt(a_local[i]);

          const std::string str_boost = hexlexical_cast(c_boost);
          const std::string str_local = hexlexical_cast(c_local);

          while(test_lock.test_and_set()) { ; }
          result_is_ok &= (str_boost == str_local);
          test_lock.clear();
        }
      );

      return result_is_ok;
    }

  private:
    std::vector<local_uint_type> a_local;
    std::vector<local_uint_type> b_local;

    std::vector<boost_uint_type> a_boost;
    std::vector<boost_uint_type> b_boost;
  };

#endif // TEST_UINTWIDE_T_N_BINARY_OPS_TEMPLATE_2019_12_19_H
