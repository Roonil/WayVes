struct Audio {
    // Represents Audio Settings and captured Audio Data.

    // Stores the captured Audio Data's left channel data in x, and the right audio channel data in y.
    vec2 value;
    // Example: -
    // Amplification for the audio value
    float multiplier;
    // Example: 100
} audio;

struct AudioSettings {
    // Represents the various Audio-transformation properties for each side.

    // Specifies the reversal of the left audio channel.
    int reverseLeft;
    // Example: true

    // Specifies the reversal of the right audio channel.
    int reverseRight;
    // Example: true

    // 0 for mirrored audio output, where the left half corresponds to the left audio channel and the right half corresponds to the right audio channel. 1 for linear audio output, where the top half represents the right audio channel and the bottom half represents the left audio channel.
    int mode;
    // Example: 1

    // Specifies whether the audio values should be combined from the left and right channels
    int combineChannels;
    // Example: 1
} audioSettings;

struct Chain {
    // Represents the `Chain` Object.

    // The Extent of the height of the `Chain`, compared to the total height of the Window
    float heightRatio;
    // Example: 1

    // The overall visibility of the `Particles` within the `Chain`. Set it to a very low value. Higher values make the displacements broader in height
    float strength;
    // Example: 0.175

    // The radius of the `Chain`
    float radius;
    // Example: 15

    // The number of `Particles` within a section of the `Chain`
    float density;
    // Example: 128

    // The index of the current `Particle` in the `Chain`.
    float index;
    // Example: -

    // Color of the `Chain`
    vec4 color;
    // Example: vec4(0,1,1,1)

    // The vertical distance between the top and lower halves of the `Chain`.
    float interChannelDistance;
    // Example: 12

    // The height of the center line of the `Chain`.
    float channelLineHeight;
    // Example: 10

    // Determines the color drop-off as `Particles` go vertically away from the `Chain's` center. Between 0 and 1.
    float verticalColorDropExtent;
    // Example: 0.02
} chain;