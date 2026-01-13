struct Rotate {
    // Rotates the final output of the Shader

    // The angle (in degrees) by which to rotate the output
    float angle;
    // Example: 90

    // The coordinates around which the rotation should take place
    vec2 center;
    // Example: vec2(resolution.xy / 2)

    // The coordinates of the current Pixel/Fragment being processed. Can be modified.
    vec2 coords;
    // Example: -

    // Additional transformation matrix for complex transformations
    mat2 transform;
    // Example: IDENTITY_MATRIX
};

#define IDENTITY_MATRIX mat2(vec2(1, 0), vec2(0, 1))
