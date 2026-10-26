// Copyright (C) 2026 Treston Cuzzort.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/numeric/conversion/converter_policies.hpp>
#include <boost/numeric/conversion/converter.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_same.hpp>

template<class Constant, std::float_round_style Style>
void test_constant()
{
    BOOST_STATIC_ASSERT((boost::is_same<typename Constant::value_type,
                                       std::float_round_style>::value));
    BOOST_STATIC_ASSERT((boost::mpl::equal_to<Constant,
        boost::integral_constant<std::float_round_style, Style> >::value));

    Constant constant;
    BOOST_TEST_EQ(Constant::value, Style);
    BOOST_TEST_EQ(static_cast<std::float_round_style>(constant), Style);
}

template<class Source>
void test_policies()
{
    using namespace boost::numeric;

    test_constant<typename Trunc<Source>::round_style, std::round_toward_zero>();
    test_constant<typename RoundEven<Source>::round_style, std::round_to_nearest>();
    test_constant<typename Ceil<Source>::round_style, std::round_toward_infinity>();
    test_constant<typename Floor<Source>::round_style, std::round_toward_neg_infinity>();

    typedef converter<boost::int8_t, Source,
        conversion_traits<boost::int8_t, Source>,
        def_overflow_handler, Floor<Source> > floor_converter;

    BOOST_TEST_EQ(floor_converter::convert(Source(-1.25)), -2);
    BOOST_TEST_EQ(floor_converter::convert(Source(1.25)), 1);
    BOOST_TEST_EQ(floor_converter::convert(Source(-128)), -128);
    BOOST_TEST_EQ(floor_converter::convert(Source(127.75)), 127);
    BOOST_TEST_EQ(floor_converter::out_of_range(Source(-128.25)), cNegOverflow);
    BOOST_TEST_EQ(floor_converter::out_of_range(Source(128)), cPosOverflow);
    BOOST_TEST_THROWS(floor_converter::convert(Source(-128.25)), negative_overflow);
    BOOST_TEST_THROWS(floor_converter::convert(Source(128)), positive_overflow);
}

int main()
{
    using namespace boost::numeric::convdetail;

    test_constant<round2zero_c, std::round_toward_zero>();
    test_constant<round2nearest_c, std::round_to_nearest>();
    test_constant<round2inf_c, std::round_toward_infinity>();
    test_constant<round2neg_inf_c, std::round_toward_neg_infinity>();
    test_policies<float>();
    test_policies<double>();
    test_policies<long double>();

    return boost::report_errors();
}
