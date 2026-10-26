#include ":angular/structs.glsl"
#include ":utils/post-processing/glow/structs.glsl"
#include ":utils/post-processing/rotate/structs.glsl"

#define coordinateRotation 0.

#define fragmentAngle 8.

#define visualiserMode 0
#define visualiserDirections 0

#define connectorColor vec4(0.4471, 0.8784, 1.0, 1.0)
#define circleColor vec4(0.4824, 0.0314, 1.0, 1.0)

void init()
{
    audioSettings.mode = 1;
    audioSettings.reverseLeft = 1;
    audioSettings.reverseRight = 1;
    bar.audio.multiplier = 70;
    particle.audio.multiplier = 0;
    particle.connector.left.enable = 1;
    particle.connector.right.enable = 1;

    circle.maxAngle = 320;
    circle.angleOffset = time;
    circle.startAnglePadding = 30;
    circle.endAnglePadding = 30;
    circle.restrictCircleAngle = 0;
}

void audioFetch(inout float fetchedAudio, float n, float lastN)
{
}

void setOffsets(float direction, inout vec2 particleOffset, inout vec2 barOffset, inout vec2 barSizeOffset,
                vec2 barAudio, vec2 particleAudio, float xCoordinate, float n, float lastN)
{
}

void setProps()
{

    circle.radius = int(resolution.x / 2);

    circle.innerSoftness = 85;

    particle.connector.loop = 0;
    particle.connector.left.height = 3.;
    particle.connector.left.borderSize = 8;
    particle.connector.left.color =
        mix(connectorColor, vec4(.90, .9, .9, 1.0), particle.fragment.currentAngle / circle.maxAngle);
    particle.connector.left.borderColor = vec4(0.1765, 0.3725, 0.7373, 1.0);
    particle.connector.jointMode = 1;
    particle.connector.left.innerSoftness = 2;
    particle.connector.left.outerSoftness = 30;

    circle.color = particle.connector.left.borderColor * vec4(1, 1, 1, 1. + 7.5 * texture(audioR, .1).x);

    particle.offset.y -= particle.connector.left.height + particle.connector.left.borderSize + 10;

    particle.connector.right = particle.connector.left;
}

void setParticleDownProps()
{
}

void modifySDFs()
{
}
