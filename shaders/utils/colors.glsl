#define rgba(r, g, b, a) (vec4(float(r), float(g), float(b), float(a)) / 255.0)

#define rgb(r, g, b) (vec3(float(r), float(g), float(b)) / 255.0)

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

#define hsv(h, s, v) (hsv2rgb(vec3(float(h) / 360., float(s) / 100., float(v) / 100.)))
#define hsva(h, s, v, a) (vec4(hsv2rgb(vec3(float(h) / 360., float(s) / 100., float(v) / 100.)), float(a) / 100.))

vec3 interpolateHue(vec3 startColor, float hueInterpolation, float bandIndex, float lastBandIndex)
{
    float hueOffset = bandIndex / max(lastBandIndex, 1);

    vec3 hsvValue = rgb2hsv(startColor);
    hsvValue.x = fract(hsvValue.x + hueInterpolation * hueOffset);

    return hsv2rgb(hsvValue);
}

vec4 interpolateHue(vec4 startColor, float hueInterpolation, float bandIndex, float lastBandIndex)
{
    return vec4(interpolateHue(startColor.xyz, hueInterpolation, bandIndex, lastBandIndex), startColor.w);
}
