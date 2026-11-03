#include "UniformAnimation.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <utility>

static double outBounce(double t)
{
    const double n1 = 7.5625;
    const double d1 = 2.75;

    if (t < 1.0 / d1)
        return n1 * t * t;

    if (t < 2.0 / d1) {
        t -= 1.5 / d1;
        return n1 * t * t + 0.75;
    }

    if (t < 2.5 / d1) {
        t -= 2.25 / d1;
        return n1 * t * t + 0.9375;
    }

    t -= 2.625 / d1;
    return n1 * t * t + 0.984375;
}

double Easing::sampleBezier(double a, double b, double s) const
{
    double inverse = 1.0 - s;
    return 3.0 * inverse * inverse * s * a + 3.0 * inverse * s * s * b + s * s * s;
}

double Easing::solveBezierX(double x) const
{
    double s = x;

    for (int i = 0; i < 8; i++) {
        double error = sampleBezier(bezier[0], bezier[2], s) - x;
        if (std::fabs(error) < 1e-6)
            return s;

        double inverse = 1.0 - s;
        double derivative = 3.0 * inverse * inverse * bezier[0] + 6.0 * inverse * s * (bezier[2] - bezier[0]) + 3.0 * s * s * (1.0 - bezier[2]);
        if (std::fabs(derivative) < 1e-6)
            break;

        s -= error / derivative;
    }

    double low = 0.0, high = 1.0;
    s = x;

    for (int i = 0; i < 32; i++) {
        double value = sampleBezier(bezier[0], bezier[2], s);
        if (std::fabs(value - x) < 1e-6)
            break;

        if (value < x)
            low = s;
        else
            high = s;

        s = (low + high) / 2.0;
    }

    return s;
}

double Easing::apply(double t) const
{
    t = std::clamp(t, 0.0, 1.0);

    const double c1 = 1.70158;
    const double c2 = c1 * 1.525;
    const double c3 = c1 + 1.0;
    const double pi = std::acos(-1.0);
    const double c4 = 2.0 * pi / 3.0;
    const double c5 = 2.0 * pi / 4.5;

    switch (type) {
    case EaseType::Linear:
        return t;
    case EaseType::InQuad:
        return t * t;
    case EaseType::OutQuad:
        return 1.0 - (1.0 - t) * (1.0 - t);
    case EaseType::InOutQuad:
        return t < 0.5 ? 2.0 * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 2.0) / 2.0;
    case EaseType::InCubic:
        return t * t * t;
    case EaseType::OutCubic:
        return 1.0 - std::pow(1.0 - t, 3.0);
    case EaseType::InOutCubic:
        return t < 0.5 ? 4.0 * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 3.0) / 2.0;
    case EaseType::InExpo:
        return t == 0.0 ? 0.0 : std::pow(2.0, 10.0 * t - 10.0);
    case EaseType::OutExpo:
        return t == 1.0 ? 1.0 : 1.0 - std::pow(2.0, -10.0 * t);
    case EaseType::InOutExpo:
        if (t == 0.0 || t == 1.0)
            return t;
        return t < 0.5 ? std::pow(2.0, 20.0 * t - 10.0) / 2.0 : (2.0 - std::pow(2.0, -20.0 * t + 10.0)) / 2.0;
    case EaseType::InBack:
        return c3 * t * t * t - c1 * t * t;
    case EaseType::OutBack:
        return 1.0 + c3 * std::pow(t - 1.0, 3.0) + c1 * std::pow(t - 1.0, 2.0);
    case EaseType::InOutBack:
        return t < 0.5
            ? (std::pow(2.0 * t, 2.0) * ((c2 + 1.0) * 2.0 * t - c2)) / 2.0
            : (std::pow(2.0 * t - 2.0, 2.0) * ((c2 + 1.0) * (t * 2.0 - 2.0) + c2) + 2.0) / 2.0;
    case EaseType::InSine:
        return 1.0 - std::cos(t * pi / 2.0);
    case EaseType::OutSine:
        return std::sin(t * pi / 2.0);
    case EaseType::InOutSine:
        return -(std::cos(pi * t) - 1.0) / 2.0;
    case EaseType::InQuart:
        return std::pow(t, 4.0);
    case EaseType::OutQuart:
        return 1.0 - std::pow(1.0 - t, 4.0);
    case EaseType::InOutQuart:
        return t < 0.5 ? 8.0 * std::pow(t, 4.0) : 1.0 - std::pow(-2.0 * t + 2.0, 4.0) / 2.0;
    case EaseType::InQuint:
        return std::pow(t, 5.0);
    case EaseType::OutQuint:
        return 1.0 - std::pow(1.0 - t, 5.0);
    case EaseType::InOutQuint:
        return t < 0.5 ? 16.0 * std::pow(t, 5.0) : 1.0 - std::pow(-2.0 * t + 2.0, 5.0) / 2.0;
    case EaseType::InCirc:
        return 1.0 - std::sqrt(1.0 - t * t);
    case EaseType::OutCirc:
        return std::sqrt(1.0 - (t - 1.0) * (t - 1.0));
    case EaseType::InOutCirc:
        return t < 0.5
            ? (1.0 - std::sqrt(1.0 - 4.0 * t * t)) / 2.0
            : (std::sqrt(1.0 - std::pow(-2.0 * t + 2.0, 2.0)) + 1.0) / 2.0;
    case EaseType::InElastic:
        if (t == 0.0 || t == 1.0)
            return t;
        return -std::pow(2.0, 10.0 * t - 10.0) * std::sin((10.0 * t - 10.75) * c4);
    case EaseType::OutElastic:
        if (t == 0.0 || t == 1.0)
            return t;
        return std::pow(2.0, -10.0 * t) * std::sin((10.0 * t - 0.75) * c4) + 1.0;
    case EaseType::InOutElastic:
        if (t == 0.0 || t == 1.0)
            return t;
        return t < 0.5
            ? -(std::pow(2.0, 20.0 * t - 10.0) * std::sin((20.0 * t - 11.125) * c5)) / 2.0
            : (std::pow(2.0, -20.0 * t + 10.0) * std::sin((20.0 * t - 11.125) * c5)) / 2.0 + 1.0;
    case EaseType::InBounce:
        return 1.0 - outBounce(1.0 - t);
    case EaseType::OutBounce:
        return outBounce(t);
    case EaseType::InOutBounce:
        return t < 0.5 ? (1.0 - outBounce(1.0 - 2.0 * t)) / 2.0 : (1.0 + outBounce(2.0 * t - 1.0)) / 2.0;
    case EaseType::CubicBezier:
        return sampleBezier(bezier[1], bezier[3], solveBezierX(t));
    }

    return t;
}

bool Easing::parse(std::string name, Easing* easing)
{
    std::string lowered = name;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);

    if (lowered.rfind("cubicbezier(", 0) == 0 && lowered.back() == ')') {
        std::stringstream points(lowered.substr(12, lowered.size() - 13));
        std::string point;
        double values[4];
        int count = 0;

        while (std::getline(points, point, ',')) {
            if (count == 4)
                return false;
            try {
                values[count++] = std::stod(point);
            } catch (const std::exception&) {
                return false;
            }
        }

        if (count != 4)
            return false;

        easing->type = EaseType::CubicBezier;
        easing->bezier[0] = std::clamp(values[0], 0.0, 1.0);
        easing->bezier[1] = values[1];
        easing->bezier[2] = std::clamp(values[2], 0.0, 1.0);
        easing->bezier[3] = values[3];
        return true;
    }

    static const std::pair<const char*, EaseType> names[] = {
        { "linear", EaseType::Linear },
        { "inquad", EaseType::InQuad },
        { "outquad", EaseType::OutQuad },
        { "inoutquad", EaseType::InOutQuad },
        { "incubic", EaseType::InCubic },
        { "outcubic", EaseType::OutCubic },
        { "inoutcubic", EaseType::InOutCubic },
        { "inexpo", EaseType::InExpo },
        { "outexpo", EaseType::OutExpo },
        { "inoutexpo", EaseType::InOutExpo },
        { "inback", EaseType::InBack },
        { "outback", EaseType::OutBack },
        { "inoutback", EaseType::InOutBack },
        { "insine", EaseType::InSine },
        { "outsine", EaseType::OutSine },
        { "inoutsine", EaseType::InOutSine },
        { "inquart", EaseType::InQuart },
        { "outquart", EaseType::OutQuart },
        { "inoutquart", EaseType::InOutQuart },
        { "inquint", EaseType::InQuint },
        { "outquint", EaseType::OutQuint },
        { "inoutquint", EaseType::InOutQuint },
        { "incirc", EaseType::InCirc },
        { "outcirc", EaseType::OutCirc },
        { "inoutcirc", EaseType::InOutCirc },
        { "inelastic", EaseType::InElastic },
        { "outelastic", EaseType::OutElastic },
        { "inoutelastic", EaseType::InOutElastic },
        { "inbounce", EaseType::InBounce },
        { "outbounce", EaseType::OutBounce },
        { "inoutbounce", EaseType::InOutBounce },
    };

    for (const auto& entry : names) {
        if (lowered == entry.first) {
            easing->type = entry.second;
            return true;
        }
    }

    return false;
}

double UniformAnimation::progress(double now) const
{
    if (duration <= 0.0)
        return 1.0;

    return std::max((now - startTime) / duration, 0.0);
}

void UniformAnimation::sample(double now, double* out) const
{
    double eased = easing.apply(progress(now));

    for (int i = 0; i < components; i++)
        out[i] = from[i] + (to[i] - from[i]) * eased;
}

int countValueComponents(std::string value)
{
    std::string::size_type startIdx = value.find("(");
    if (startIdx == std::string::npos)
        return 1;

    std::string::size_type endIdx = value.find(")", startIdx);
    std::string inner = value.substr(startIdx + 1, endIdx == std::string::npos ? std::string::npos : endIdx - startIdx - 1);

    return std::min((int)std::count(inner.begin(), inner.end(), ',') + 1, 4);
}

std::string formatValueComponents(const double* values, int components, bool roundToInt)
{
    std::string result = components > 1 ? "(" : "";
    char buffer[32];

    for (int i = 0; i < components; i++) {
        double value = roundToInt ? std::round(values[i]) : values[i];
        std::snprintf(buffer, sizeof(buffer), "%.9g", value);

        if (i > 0)
            result += ",";
        result += buffer;
    }

    if (components > 1)
        result += ")";

    return result;
}
