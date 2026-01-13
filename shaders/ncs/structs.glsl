struct BaseForm {
    // Represents the `Grid` of the `Particles`.

    // Type of the `Grid` to use. 0 for `Rectangular`, and 1 for `Spherical Grid` arranged as `Concentric Circles`
    int type;
    // Example: 0

    // The overall Scaling Factor for the displayed `Grid` in X, Y and Z directions
    vec3 scale;
    // Example: vec3(2)

    // The number of `Particles` to render. Maximum number of `Particles` in each Dimension is the size of the window in that dimension
    vec3 numParticles;
    // Example: resolution.xy

    // The size of the Window in the Z Axis
    float zSize;
    // Example: 100

    // The rotations to apply to the `BaseForm`
    mat3 rotations;
    // Example: IDENTITY_MATRIX

    // The center around which the rotations will be applied
    vec3 rotationCenter;
    // Example: vec3(resolution.xy / 2, 0)
} baseForm;

struct Audio {
    // Represents various Audio Settings and captured Audio Data.

    // Determines the strength by which the audio stream affects the inner `Fractal Field` displacements
    float multiplier;
    // Example: 7

    // Determines the `Sphere's` size change by the audio's lower frequencies
    float bassMultiplier;
    // Example: 5

    // Between 0 and 1. Greater value means stronger reaction to beats or tonal changes in audio
    float mixing;
    // Example: 0.5

    // Audio value corresponding to the lower frequencies is stored here
    float bass;
    // Example: -

    // The Factor that is used to raise the power of the intermediate Audio Data Points, for the 'quickness' of the reactions when there is a sudden change in the playing Frequencies
    float exponentiationFactor;
    // Example: 1.02

    // The Normalised Points at which Audio Samples are taken for an Intermediate Audio Value that is used to derive the final Audio output value.
    float samplePoints[9];
    // Example: {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9}

    // The Step for the next Audio Sample Point for the current Intermediate Audio Value, which will be used to find the maximum of the Audio Data at this stepped Point and the Point before the difference.
    float samplePointsDifferences[9];
    // Example: {0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05}

    // Intermediate Audio Values, stored in Ascending Order of the Intensity of the Frequencies. Used to derive the final driving audio value
    float intermediateAudios[8];
    // Example: -

    // The Final Audio Value that is derived from the Intermediate Audio Values
    float value;
    // Example: -

} audio;

struct Particle {
    // Represents individual `Particles`.

    // Color of the `Particles`
    vec4 color;
    // Example: vec4(0, 0, 1, 1)

    // Use this value to affect the Particle's opacity with various parameters
    float opacityMultiplier;
    // Example: 1

    // Size of the `Particles`
    int size;
    // Example: 3

    // Feathering or Smoothing of the `Particles`. Between 0 and 1
    float feather;
    // Example: 0.5

    // Uses `Add Color Blend mode` to mix colors when `Particles` overlap. Between 0 and 1
    float colorIntensityAddStrength;
    // Example: 0.1

    // Change this value in case some color combination produces jagged spherical edges. Greater than 0
    float antiAlias;
    // Example: 4.5

    //  The position of the `Particle`
    vec3 position;
    // Example: -
} particle;

struct FractalField {
    // Represents the Underlying `Noise Field`.

    // Defines the multiplier value with which a `Noise Octave` gets added in to the final `Noise Output`. Higher values produce more peaks and valleys
    float octaveMultiplier;
    // Example: 0.5

    // Defines the scale of each `Noise Octave` that constitutes the final `Noise Output`
    float octaveScale;
    // Example: 1.5

    // Defines the number of `Octaves`, or the number of values that add up in the final `Noise Output`. Higher is slower.
    int complexity;
    // Example: 3

    // Defines the scale of the overall noise-displaced `Particle Grid`. Higher value gives a 'zoomed-in' view, providing more displaced output
    float fScale;
    // Example: 10

    // The dimensions of the `Fractal Field` in `X, Y, and Z` directions along with `time`
    vec4 dimensions;
    // Example: vec4(1000)

    // Adjusts the difference between the valleys and peaks of the `Noise Output`, similar to gamma for brightness / contrast values
    float gamma;
    // Example: 1

    // Determines the minimum possible output value of the `Noise` function
    float minVal;
    // Example: -1

    // Determines the maximum possible output value of the `Noise` function
    float maxVal;
    // Example: 1

    // The offset to add to the overall `Noise` produced
    float offset;
    // Example: 0

    // Multiplier for generated `Noise`
    float noiseMultiplier;
    // Example: 1

    // Value that gets added along with the Audio Value to the overall `Noise` generated; can be used to have a non-zero `Noise Output`
    float constantNoiseMultiplier;
    // Example: 0

    // Determines the extent to which the `Fractal Field` affects the `Opacity` of the `Particles`
    float affectOpacity;
    // Example: 0

    // Determines the extent to which the `Fractal Field` affects the `Size` of the `Particles`
    float affectSize;
    // Example: 0

    // Whether to loop the `Fractal Field` after the specified amount of `loopFrames` have passed
    int loop;
    // Example: 0

    // The Number of Frames after which the `Fractal Field` will loop. `fScale >= loopTime * min(flow.x, flow.y, flow.z, flow.w) / dimensions` should be satisfied for the looping to take place, or you might see unexpected `Noise` values
    int loopFrames;
    // Example: 200

    // The type of the displacement to use. 0 is `Normal Displacement`, and 1 is `Radial Displacement` (strength of the displacement is determined by `displacements.x`)
    int displacementType;
    // Example: 0

    // Displacements in `X,Y and Z` directions
    vec3 displacements;
    // Example: vec3(100)

    // `Flow` of the `Fractal Field` in `X, Y and Z` directions, along with `Flow` in `Time` (`Flow Evolution`)
    vec4 flows;
    // Example: vec4(0, 0, 0, 2)

    // The output noise value for each spatial direction `X, Y and Z`
    vec3 noise;
    // Example: -
} fractalField;

struct Sphere {
    // Represents the `Sphere` that can displace the `Particles` Radially.

    // Radius of the `Sphere`
    float radius;
    // Example: 0

    //  Determines the size of the "band" around the `Sphere`. Between 0 and 1
    float feather;
    // Example: 0

    // The Strength with which the particles get pushed (or pulled) from the center of the `Sphere`
    float strength;
    // Example: 1

    // Coordinates of the Center of the `Sphere`
    vec3 center;
    // Example: vec3(resolution.xy / 2, 0)

    // Scale of the `Sphere` in each Direction
    vec3 scale;
    // Example: vec3(1)
} sphere;

#ifndef TWOPI
#define TWOPI (6.2831853071794)
#endif
#ifndef PI
#define PI (3.1415926535897)
#endif
#ifndef RAD_PI
#define RAD_PI (PI / 180.)
#endif

mat3 rotateX(float angle)
{

    float angleRads = angle * RAD_PI;
    float sinAngle = sin(angleRads), cosAngle = cos(angleRads);

    mat3x3 rotationMatrix = mat3x3(
        vec3(1, 0, 0),
        vec3(0, cosAngle, sinAngle),
        vec3(0, -sinAngle, cosAngle));

    return rotationMatrix;
}

mat3 rotateY(float angle)
{

    float angleRads = angle * RAD_PI;
    float sinAngle = sin(angleRads), cosAngle = cos(angleRads);

    mat3x3 rotationMatrix = mat3x3(
        vec3(cosAngle, 0, -sinAngle),
        vec3(0, 1, 0),
        vec3(sinAngle, 0, cosAngle));

    return rotationMatrix;
}

mat3 rotateZ(float angle)
{

    float angleRads = angle * RAD_PI;
    float sinAngle = sin(angleRads), cosAngle = cos(angleRads);

    mat3x3 rotationMatrix = mat3x3(
        vec3(cosAngle, sinAngle, 0),
        vec3(-sinAngle, cosAngle, 0),
        vec3(0, 0, 1));

    return rotationMatrix;
}

#define IDENTITY_MATRIX mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1))
