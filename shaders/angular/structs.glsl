struct Fragment {
    // Represents the underlying `Fragment` that can show one `Particle` and one `Bar`. None of the Attributes are meant to be modified.

    // Number of the current `Fragment`. Ranges from 0 to `lastN`.
    float n;
    // Example: -

    // Number of the last possible Fragment.
    float lastN;
    // Example: -

    // Span of the Fragment. Lies in `[-fragmentAngle/2.,fragmentAngle/2.]`
    float span;
    // Example: -

    // Specifies the absolute current Angle. Lies in `[0, TWOPI]`
    float currentAngle;
    // Example: -

    // Specifies the absolute current coordinates. Lies in `[vec2(0,0),vec2(resolution.xy)]`
    vec2 coords;
    // Example: -

    // Specifies the absolute Angle of the `current Fragment's` Center.
    float currentCenterAngle;
    // Example: -

    // Specifies the Vector from the Center of the `Circle`.
    vec2 vectorFromCenter;
    // Example: -

    // Corresponds to length of the Vector from the Center of the `Circle`.
    float distanceFromCenter;
    // Example: -
};

struct AudioSettings {
    // Represents the various Audio-transformation properties for each side.

    // Specifies the reversal of the left audio channel. Either 0 or 1.
    int reverseLeft;
    // Example: 0

    // Specifies the reversal of the right audio channel. Either 0 or 1.
    int reverseRight;
    // Example: 0

    // 0 for mirrored audio output, where the left half corresponds to the left audio channel and the right half corresponds to the right audio channel. 1 for linear audio output, where the top half represents the right audio channel and the bottom half represents the left audio channel.
    int mode;
    // Example: 1

    // Specifies whether the audio values should be combined from the left and right channels. Either 0 or 1.
    int combineChannels;
    // Example: 1
} audioSettings;

struct Audio {
    // Represents the captured Audio Data, and its multiplier.

    // Amplification for the audio value
    float multiplier;
    // Example: 100

    // Stores the `Current Fragment's` left audio channel data in x, and the right audio channel data in y.
    vec2 current;
    // Example: -

    // Stores the `Previous Fragment's` left audio channel data in x, and the right audio channel data in y.
    vec2 prev;
    // Example: -

    // Stores the `Next Fragment's` left audio channel data in x, and the right audio channel data in y.
    vec2 next;
    // Example: -
};

struct Cap {
    // Represents a `Cap` that can 'bounce' over a `Primitive` (`Bar` or `Particle`). Requires 1 `Image Texture` to be enabled when `visualiserMode` is either 0 or 1, and requires 2 when`visualiserMode` is 2.

    // Specifies whether the `Caps` should be enabled.
    bool enable;
    // Example: false

    // The Downwards fall rate for the `Cap`. Constant.
    float rate;
    // Example: 0.03

    // Bounciness of the `Cap` when encountering the Edge of the `Parent Primitive`.
    float elasticity;
    // Example: 0.03

    // The type of the `Cap`. 0 for `Angular Cap` that changes width based on its height, 1 for `Fixed-Width Cap`
    float type;
    // Example: 1

    // Minimum 'bounciness' value that is used to immediately put the `Cap` at rest on the `Parent Primitive's` Edge.
    float elasticityMinThreshold;
    // Example: 0.6

    // Upwards Velocity of the `Cap` when the `parent Primitive` moves upwards.
    float launchVelocity;
    // Example: 0.04

    // Size of the `Cap`.
    vec2 size;
    // Example: vec2(3, 5)

    // Acceleration for the downwards fall of the `Cap`.
    float acceleration;
    // Example: 0.5

    // Upwards Thrust of the `Cap` influenced by the current Audio value in the `Fragment`, when the `Parent Primitive` moves upwards.
    float launchFlingMultiplier;
    // Example: 0.5

    // Drag factor. Higher values yield a constant downwards terminal velocity for the `Cap`.
    float dragFactor;
    // Example: 0.01

    // Offset for the `Cap`. Will be added to the coordinates of the `Cap`.
    vec2 offset;
    // Example: vec2(0,12)

    // The audio value that will be used for the `Cap`. `x` stores the left channel for the current `Fragment` and `y` stores the right channel. Each channel is used according to the channel of the `Primitive`.
    vec2 audio;
    // Example: -

    // Softness of the `Cap`. `x, y and z` respectively represent softness values for the `top`, `left & right`, and `bottom` edges. Higher softness value yields a smoother and blurred edge.
    vec3 softness;
    // Example: vec3(1, 1.5, 1)

    // Color of the `Cap`.
    vec4 color;
    // Example: vec4(1, 0, 0, 0)
};

struct Bar {
    // Represents a `Bar`.

    // Stores the current `Fragment`.
    Fragment fragment;
    // Example: -

    // Stores captured Audio Data.
    Audio audio;
    // Example: -

    // Stores the `Top Cap` for the `Bar`.
    Cap upCap;
    // Example: -

    // Stores the `Bottom Cap for the Bar`.
    Cap downCap;
    // Example: -

    // Specifies the type of `Bars` to draw: 0 for `Angular`, 1 for `Rectangular` and 2 for `Rounded`. Only `Angular` and `Rectangular Bars` can be merged.
    int type;
    // Example: 1

    // Specifies merging of the `Bar` on the Left. Either 0 or 1.
    int mergeLeftBar;
    // Example: 1

    // Specifies merging of the `Bar` on the Right. Either 0 or 1.
    int mergeRightBar;
    // Example: 1

    // 1 to specify whether the first and last `Bars` should be merged, when `mergeLeftBar` is 1 or mergeRightBar is 1 for the last Bar. `clampLeftMergeBorder` and `clampRightMergeBorder` should both be 0 for this setting to take effect.
    int mergeEnds;
    // Example: 1

    // 1 to specify left Border to be always present on the very first `Bar`, when `mergeLeftBar` is 1.
    int clampLeftMergeBorder;
    // Example: 1

    // 1 to specify right Border to be always present on the very last `Bar`, when `mergeRightBar` is 1.
    int clampRightMergeBorder;
    // Example: 1

    // Specifies the offset of the `Bar`. Added to `Bar's` coordinates.
    vec2 offset;
    // Example: vec2(0,-12)

    // Specifies the Size of the `Bar`. `x, y and z` represent the minimum upwards height, horizontal size, and the minimum downwards height.
    vec3 size;
    // Example: vec3(3, 12, 3)

    // Specifies the BorderSize of the `Bar`. `x, y and z` represent the top, left & right, down side Borders.
    vec3 borderSize;
    // Example: vec3(1, 2, 1)

    // Softness of the Inner Edge of the `Bar`. `x, y and z` respectively represent softness values for the `top`, `left & right`, and `bottom` edges. Higher softness value yields a smoother and blurred edge.
    vec3 innerSoftness;
    // Example: vec3(0.5)

    // Softness of the Outer Edge of the `Bar`. `x, y and z` respectively represent softness values for the `top`, `left & right`, and `bottom` edges. Higher softness value yields a smoother and blurred edge.
    vec3 outerSoftness;
    // Example: vec3(0.5)

    // Specifies the Inner Color of the `Bar`.
    vec4 color;
    // Example: vec4(0,1,1,1)

    // Specifies the Outer Color of the `Bar`.
    vec4 borderColor;
    // Example: vec4(0.0, 1.0, 0.5, 1.0)

    // Specifies the background Color over which `Bars` are displayed.
    vec4 bgColor;
    // Example: vec4(0.5)
} bar;

struct Circle {
    // Represents the `Circle` at the Center.

    // The Maximum Angle at which the `Circle` should be drawn.
    float maxAngle;
    // Example: 360

    // Specifies whether to also restrict the `Circle` when `maxAngle` is smaller than 360. Set to 0 to disable the restriction, or 1 to enable it.
    float restrictCircleAngle;
    // Example: 1

    // Specifies the offset Angle at which to place the `Circle`.
    float angleOffset;
    // Example: 30

    // Specifies the radius of the `Circle`.
    int radius;
    // Example: 100

    // Specifies the size of the Border of the `Circle`.
    int borderSize;
    // Example: 4

    // Inner Softness of the `Circle`. Higher softness value yields a smoother and blurred edge.
    float innerSoftness;
    // Example: 1.5

    // Outer Softness of the `Circle`. Higher softness value yields a smoother and blurred edge.
    float outerSoftness;
    // Example: 1.5

    // Absolute Coordinates of the Center of the `Circle`. Defaults to the center of the Window, that is, `vec2(resolution.xy/2.)`
    vec2 center;
    // Example: vec2(resolution.xy)

    // Inner Color of the `Circle`.
    vec4 color;
    // Example: vec4(0.0, 1.0, 1.0, 1.0)

    // Outer Color of the `Circle`.
    vec4 borderColor;
    // Example: vec4(1.0, 0.0, 0.5, 1.0)

} circle;

struct ConnectorHalf {
    // Represents the left and right sides of a `Connector`.

    // Specifies whether the `ConnectorHalf` should be enabled.
    int enable;
    // Example: 0

    // Height of the `ConnectorHalf`.
    float height;
    // Example: 3

    // BorderSize of the `ConnectorHalf`.
    float borderSize;
    // Example: 2

    // Inner Softness of the `ConnectorHalf`. Higher softness value yields a smoother and blurred edge.
    float innerSoftness;
    // Example: 0.5

    // Outer Softness of the `ConnectorHalf`. Higher softness value yields a smoother and blurred edge.
    float outerSoftness;
    // Example: 0.5

    // Inner Color of the `ConnectorHalf`.
    vec4 color;
    // Example: vec4(1.0, 1.0, 0.0, 1.0)

    // Outer Color of the `ConnectorHalf`.
    vec4 borderColor;
    // Example: vec4(1.0, 1.0, 1.0, 1.0)
};

struct Connector {
    // Represents the combination of left and right `Connectors`.

    // Left `ConnectorHalf`
    ConnectorHalf left;
    // Example: -

    // Right `Connector Half`
    ConnectorHalf right;
    // Example: -

    // Joint Mode
    int jointMode;
    // Example: 1

    // Joint Color Mode
    int jointColorMode;
    // Example: 0

    // Stores the coordinates of the `Particle` in the `Left Fragment`.
    vec2 leftCenter;
    // Example: -

    // Stores the coordinates of the `Particle` in the `Current Fragment`.
    vec2 currentCenter;
    // Example: -

    // Stores the coordinates of the `Particle` in the `Right Fragment`.
    vec2 rightCenter;
    // Example: -

    // Whether to loop the `Connectors` for the first and last `Particles`, when `left ConnectorHalf` is enabled or `right ConnectorHalf` is enabled for the last `Particle`. Does not necessarily create merged `Connectors` at all `fragmentAngle` values.
    int loop;
    // Example: 1

    // 1 to specify whether the first and last `Connectors` should be merged, when `left ConnectorHalf` is enabled or `right ConnectorHalf` is enabled for the last `Particle`.
    int mergeEnds;
    // Example: 1
};

struct Particle {
    // Represents a `Particle`.

    // Stores the current `Fragment`.
    Fragment fragment;
    // Example: -

    // Stores captured Audio Data.
    Audio audio;
    // Example: -

    // Stores the `Cap` for the `Particle`.
    Cap cap;
    // Example: -

    // Stores `Connector` Settings.
    Connector connector;
    // Example: -

    // Specifies whether the bottom offset value provided should be inverted for the `Bottom Particles`.
    int reverseBottomOffset;
    // Example: 1

    // Specifies the radius of the `Particle`.
    float radius;
    // Example: 3

    // Specifies the borderSize of the `Particle`.
    float borderSize;
    // Example: 2

    // Specifies the vertical distance between the `Top and Bottom Particles`
    float interChannelDistance;
    // Example: 12

    // Specifies the inner Softness of the `Particle`. Higher softness value yields a smoother and blurred edge.
    float innerSoftness;
    // Example: 0.5

    // Specifies the inner Softness of the `Particle`. Higher softness value yields a smoother and blurred edge.
    float outerSoftness;
    // Example: 0.5

    // Specifies the offset of the `Particle`. Added to the coordinates of the `Particle`. Use `setOffsets()` to properly show connectors if enabled.
    vec2 offset;
    // Example: vec2(1,12)

    // Inner Color of the `Particle`.
    vec4 color;
    // Example: vec4(1, 0, 0, 1)

    // Outer Color of the `Particle`.
    vec4 borderColor;
    // Example: vec4(1, 1, 1, 1)
} particle;

#define CIRCLE_INNER_SDF 0
#define CIRCLE_OUTER_SDF 1
#define BAR_BG_SDF 2
#define BAR_INNER_SDF 3
#define BAR_OUTER_SDF 4
#define BAR_UP_CAP_SDF 5
#define BAR_DOWN_CAP_SDF 6
#define PARTICLE_UP_INNER_SDF 7
#define PARTICLE_UP_OUTER_SDF 8
#define PARTICLE_UP_LEFT_CONNECTOR_INNER_SDF 9
#define PARTICLE_UP_RIGHT_CONNECTOR_INNER_SDF 10
#define PARTICLE_UP_LEFT_CONNECTOR_OUTER_SDF 11
#define PARTICLE_UP_RIGHT_CONNECTOR_OUTER_SDF 12
#define PARTICLE_UP_CAP_SDF 13
#define PARTICLE_DOWN_INNER_SDF 14
#define PARTICLE_DOWN_OUTER_SDF 15
#define PARTICLE_DOWN_LEFT_CONNECTOR_INNER_SDF 16
#define PARTICLE_DOWN_RIGHT_CONNECTOR_INNER_SDF 17
#define PARTICLE_DOWN_LEFT_CONNECTOR_OUTER_SDF 18
#define PARTICLE_DOWN_RIGHT_CONNECTOR_OUTER_SDF 19
#define PARTICLE_DOWN_CAP_SDF 20

int zOrder[21] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
float sdfs[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };