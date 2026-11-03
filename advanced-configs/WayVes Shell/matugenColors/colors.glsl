#include ":utils/colors.glsl"
#include "colors_uniforms.glsl"

struct MatugenColors
{
    vec4 primary, sourceColor, onPrimary, primaryContainer, onPrimaryContainer, secondary, tertiary, surface, onSurface,
        surfaceContainer, outline, disabled, onDisabled, surfaceDisabled;
    bool isDarkMode;
};

MatugenColors matugenColors =
    MatugenColors(primary, sourceColor, onPrimary, primaryContainer, onPrimaryContainer, secondary, tertiary, surface,
                  onSurface, surfaceContainer, outline, disabled, onDisabled, surfaceDisabled, isDarkMode);