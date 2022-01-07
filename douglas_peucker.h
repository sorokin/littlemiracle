// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <QPoint>
#include <vector>

std::vector<QPointF> simplify_polyline(std::vector<QPointF> const& polyline, double eps);
