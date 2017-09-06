// Boost.Geometry

// Copyright (c) 2017 Oracle and/or its affiliates.

// Contributed and/or modified by Vissarion Fysikopoulos, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_FORMULAS_ELLIPTIC_ARC_LENGTH_HPP
#define BOOST_GEOMETRY_FORMULAS_ELLIPTIC_ARC_LENGTH_HPP


#include <boost/math/constants/constants.hpp>

#include <boost/geometry/core/radius.hpp>
#include <boost/geometry/core/srs.hpp>

#include <boost/geometry/util/condition.hpp>
#include <boost/geometry/util/math.hpp>

#include <boost/geometry/formulas/flattening.hpp>


namespace boost { namespace geometry { namespace formula
{

/*!
\brief Compute the arc length of an ellipse.
*/

template <typename CT>
class elliptic_arc_length
{
    // Distance computation on meridians using series approximations
    // to elliptic integrals.
    // https://en.wikipedia.org/wiki/Meridian_arc
    // latitudes are assumed to be in radians
    template <unsigned int Order, typename T, typename Spheroid>
    CT series_expansion(T lat, Spheroid const& spheroid)
    {
        CT const a = get_radius<0>(spheroid);
        CT const f = formula::flattening<CT>(spheroid);
        CT n = f / (CT(2) - f);
        CT M = a/(1+n);
        CT C0 = 1;
        
        if (Order == 0)
        {
           return M * C0 * lat;
        }

        CT C2 = -1.5 * n;

        if (Order == 1)
        {
            return M * (C0 * lat + C2 * sin(2*lat));
        }

        CT n2 = n * n;
        C0 += .25 * n2;
        CT C4 = 0.9375 * n2;

        if (Order == 2)
        {
            return M * (C0 * lat + C2 * sin(2*lat) + C4 * sin(4*lat));
        }

        CT n3 = n2 * n;
        C2 += 0.1875 * n3;
        CT C6 = -0.729166667 * n3;

        if (Order == 3)
        {
            return M * (C0 * lat + C2 * sin(2*lat) + C4 * sin(4*lat)
                      + C6 * sin(6*lat));
        }

        CT n4 = n2 * n2;
        C4 -= 0.234375 * n4;
        CT C8 = 0.615234375 * n4;

        if (Order == 4)
        {
            return M * (C0 * lat + C2 * sin(2*lat) + C4 * sin(4*lat)
                      + C6 * sin(6*lat) + C8 * sin(8*lat));
        }

        CT n5 = n4 * n;
        C6 += 0.227864583 * n5;
        CT C10 = -0.54140625 * n5;

        // Order 5 otr higher
        return M * (C0 * lat + C2 * sin(2*lat) + C4 * sin(4*lat)
                  + C6 * sin(6*lat) + C8 * sin(8*lat) + C10 * sin(10*lat));

    }

public :

    template
    <
        unsigned int Order = 2,
        typename T1,
        typename T2,
        typename Spheroid
    >
    CT apply(T1 lat1, T2 lat2, Spheroid const& spheroid)
    {
        return series_expansion<Order>(lat2, spheroid)
              - series_expansion<Order>(lat1, spheroid);
    }

    // Iterative method to elliptic arc length based on
    // http://www.codeguru.com/cpp/cpp/algorithms/article.php/c5115/
    // Geographic-Distance-and-Azimuth-Calculations.htm
    // latitudes are assumed to be in radians
    template <typename T1, typename T2, typename Spheroid>
    CT interative_method(T1 lat1,
                         T2 lat2,
                         Spheroid const& spheroid)
    {
        CT result = 0;
        CT const zero = 0;
        CT const one = 1;
        CT const c1 = 2;
        CT const c2 = 0.5;
        CT const c3 = 4000;

        CT const a = get_radius<0>(spheroid);
        CT const f = formula::flattening<CT>(spheroid);

        // how many steps to use

        CT lat1_deg = lat1 * geometry::math::r2d<CT>();
        CT lat2_deg = lat2 * geometry::math::r2d<CT>();

        int steps = c1 + (c2 + (lat2_deg > lat1_deg) ? CT(lat2_deg - lat1_deg)
                                                     : CT(lat1_deg - lat2_deg));
        steps = (steps > c3) ? c3 : steps;

        //std::cout << "Steps=" << steps << std::endl;

        CT snLat1 = sin(lat1);
        CT snLat2 = sin(lat2);
        CT twoF   = 2 * f - f * f;

        // limits of integration
        CT x1 = a * cos(lat1) /
                sqrt(1 - twoF * snLat1 * snLat1);
        CT x2 = a * cos(lat2) /
                sqrt(1 - twoF * snLat2 * snLat2);

        CT dx = (x2 - x1) / (steps - one);
        CT x, y1, y2, dy, dydx;
        CT adx = (dx < zero) ? -dx : dx;    // absolute value of dx

        CT a2 = a * a;
        CT oneF = 1 - f;

        // now loop through each step adding up all the little
        // hypotenuses
        for (int i = 0; i < (steps - 1); i++){
            x = x1 + dx * i;
            dydx = ((a * oneF * sqrt((one - ((x+dx)*(x+dx))/a2))) -
                    (a * oneF * sqrt((one - (x*x)/a2)))) / dx;
            result += adx * sqrt(one + dydx*dydx);
        }

        return result;
    }
};

}}} // namespace boost::geometry::formula


#endif // BOOST_GEOMETRY_FORMULAS_ELLIPTIC_ARC_LENGTH_HPP
