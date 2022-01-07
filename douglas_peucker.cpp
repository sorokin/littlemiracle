// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "douglas_peucker.h"
#include <cmath>

namespace
{
    double norm(QPointF v)
    {
        return std::sqrt(QPointF::dotProduct(v, v));
    }

    double mixed_product(QPointF v1, QPointF v2)
    {
        return v1.x() * v2.y() - v2.x() * v1.y();
    }

    double point_line_distance(QPointF p, QPointF line_p1, QPointF line_p2)
    {
        QPointF v1 = p - line_p1;
        QPointF v2 = line_p2 - line_p1;
        return std::abs(mixed_product(v1, v2) / norm(v2));
    }

    void simplify_rec(QPointF const* in, size_t n, double eps, std::vector<QPointF>& out)
    {
        double max_d = 0.;
        size_t max_index = 0;

        for (size_t i = 1; i < n - 1; ++i)
        {
            double d = point_line_distance(in[i], in[0], in[n - 1]);
            if (d > max_d)
            {
                max_d = d;
                max_index = i;
            }
        }

        if (max_d > eps)
        {
            simplify_rec(in, max_index + 1, eps, out);
            simplify_rec(in + max_index, n - max_index, eps, out);
        }
        else
        {
            out.push_back(in[n - 1]);
        }
    }
}

std::vector<QPointF> simplify_polyline(std::vector<QPointF> const& polyline, double eps)
{
    std::vector<QPointF> res;
    res.push_back(polyline[0]);
    simplify_rec(polyline.data(), polyline.size(), eps, res);
    return res;
}
