#ifndef UNIFORM_ANIMATION_H
#define UNIFORM_ANIMATION_H

#include <string>

enum class EaseType {
    Linear,
    InQuad,
    OutQuad,
    InOutQuad,
    InCubic,
    OutCubic,
    InOutCubic,
    InExpo,
    OutExpo,
    InOutExpo,
    InBack,
    OutBack,
    InOutBack,
    CubicBezier
};

class Easing {
public:
    EaseType type = EaseType::Linear;
    double bezier[4] = { 0, 0, 1, 1 };

    double apply(double t) const;
    static bool parse(std::string name, Easing* easing);

private:
    double sampleBezier(double a, double b, double s) const;
    double solveBezierX(double x) const;
};

class UniformAnimation {
public:
    std::string target;
    double from[4] = { 0, 0, 0, 0 };
    double to[4] = { 0, 0, 0, 0 };
    int components = 1;
    bool fromResolved = false;
    bool snap = false;
    double startTime = 0;
    double duration = 0;
    Easing easing;

    double progress(double now) const;
    void sample(double now, double* out) const;
};

int countValueComponents(std::string value);
std::string formatValueComponents(const double* values, int components, bool roundToInt);

#endif
