struct Glow {
    // Adds a Glowing Effect to the final output of the Shader.

    // Blend mode for the glow effect. 0 = Additive, 1 = Overlay
    float blendMode;
    // Example: 0

    // Whether to use the Alpha Channel for the Glow Effect
    float mixAlpha;
    // Example: 1

    // Offset (in degrees) for Glow directions
    float offsetAngle;
    // Example: 90

    // Size of the Glow
    float size;
    // Example: 10

    // Intensity of the Glow
    float intensity;
    // Example: 0.5

    // Number of directions that are sampled radially. Higher is expensive.
    float directions;

    // The coordinates of the current Pixel/Fragment being processed. Can be modified.
    vec2 coords;
    // Example: -

    // Quality of Glow. Higher is expensive.
    float quality;

    // Overlay color for the Glow Effect
    vec4 color;
    // Example: vec4(1.0, 1.0, 1.0, 1.0)

    // Adjusts brightness of the final Color
    float brightnessOffset;
    // Example: 0.

    // Brightens or darkens the opacity of the final Color
    float lightStrength;
    // Example: .5
};