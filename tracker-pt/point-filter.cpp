#include "point-filter.hpp"
#include <QDebug>

namespace pt_point_filter_impl {

void point_filter::reset()
{
    t = std::nullopt;
}

const PointOrder& point_filter::operator()(const PointOrder& input)
{
    if (!s.enable_point_filter)
    {
        t = std::nullopt;
        state_ = input;
        return state_;
    }

    if (!t)
    {
        t.emplace();
        state_ = input;
        return state_;
    }

    constexpr f E = (f)1.75;
    const f C = progn(
        constexpr int A = 1'000'000;
        double K = *s.point_filter_coefficient;
        f log10_pos = -2 + (int)K, rest = (f)(.999-fmod(K, 1.)*.9);
        return A * pow((f)10, (f)-log10_pos) * rest;
    );
    f dist[3], norm = 0;

    for (unsigned i = 0; i < 3; i++)
    {
        vec2 tmp = input[i] - state_[i];
        f x = sqrt(tmp.dot(tmp));
        dist[i] = x;
        norm += x;
    }

    if (norm < (f)1e-6)
        return state_;

    f dt = (f)t->elapsed_seconds(); t->start();
    f delta = pow(norm, E) * C * dt; // gain

    for (unsigned i = 0; i < 3; i++)
    {
        f x = std::clamp(delta * dist[i] / norm, (f)0, (f)1);
        state_[i] += x*(input[i] - state_[i]);
    }

    return state_;
}

point_filter::point_filter(const pt_settings& s) : s{s} {}

} // ns pt_point_filter_impl
