#ifndef CYAML_TYPES_H
#define CYAML_TYPES_H

#include <cyaml/cyaml.h>
#include "CYAMLClasses.h"

static const cyaml_strval_t layerStrings[] = {
    {"Background", background},
    {"Bottom", bottom},
    {"Top", top},
    {"Overlay", overlay},

};

static const cyaml_schema_field_t super_gravity_settings_fields_schema[] = {

    CYAML_FIELD(UINT,
                "average-frames", CYAML_FLAG_OPTIONAL,
                GravitySettings, averageFrames, {.missing = 5}),
    CYAML_FIELD(FLOAT,
                "gravity-step", CYAML_FLAG_OPTIONAL,
                GravitySettings, gravityStep, {.missing = 4.2f}),

    CYAML_FIELD_END};

static const cyaml_schema_field_t overriden_gravity_settings_fields_schema[] = {

    CYAML_FIELD(INT,
                "average-frames", CYAML_FLAG_OPTIONAL,
                GravitySettings, averageFrames, {.missing = -1}),
    CYAML_FIELD(FLOAT,
                "gravity-step", CYAML_FLAG_OPTIONAL,
                GravitySettings, gravityStep, {.missing = -1}),

    CYAML_FIELD_END};

static const cyaml_schema_field_t super_smooth_settings_fields_schema[] = {

    CYAML_FIELD(UINT,
                "adjacent-sample-nums", CYAML_FLAG_OPTIONAL,
                SmoothSettings, adjacentSampleNums, {.missing = 1}),
    CYAML_FIELD(UINT,
                "sample-mode", CYAML_FLAG_OPTIONAL,
                SmoothSettings, sampleMode, {.missing = 0}),
    CYAML_FIELD(INT,
                "round-formula", CYAML_FLAG_OPTIONAL,
                SmoothSettings, roundFormula, {.missing = 0}),
    CYAML_FIELD(FLOAT,
                "sample-hybrid-weight", CYAML_FLAG_OPTIONAL,
                SmoothSettings, sampleHybridWeight, {.missing = 0.065f}),
    CYAML_FIELD(FLOAT,
                "sample-range", CYAML_FLAG_OPTIONAL,
                SmoothSettings, sampleRange, {.missing = 0.9f}),
    CYAML_FIELD(FLOAT,
                "sample-scale", CYAML_FLAG_OPTIONAL,
                SmoothSettings, sampleScale, {.missing = 8.0f}),
    CYAML_FIELD(FLOAT,
                "smooth-factor", CYAML_FLAG_OPTIONAL,
                SmoothSettings, smoothFactor, {.missing = 0.025f}),
    CYAML_FIELD_END};

static const cyaml_schema_field_t overriden_smooth_settings_fields_schema[] = {
    CYAML_FIELD(INT,
                "adjacent-sample-nums", CYAML_FLAG_OPTIONAL,
                SmoothSettings, adjacentSampleNums, {.missing = -1}),
    CYAML_FIELD(INT,
                "sample-mode", CYAML_FLAG_OPTIONAL,
                SmoothSettings, sampleMode, {.missing = -1}),
    CYAML_FIELD(INT,
                "round-formula", CYAML_FLAG_OPTIONAL,
                SmoothSettings, roundFormula, {.missing = -1}),
    CYAML_FIELD(FLOAT,
                "sample-hybrid-weight", CYAML_FLAG_OPTIONAL,
                SmoothSettings, sampleHybridWeight, {.missing = -1}),
    CYAML_FIELD(FLOAT,
                "sample-range", CYAML_FLAG_OPTIONAL,
                SmoothSettings, sampleRange, {.missing = -1}),
    CYAML_FIELD(FLOAT,
                "sample-scale", CYAML_FLAG_OPTIONAL,
                SmoothSettings, sampleScale, {.missing = -1}),
    CYAML_FIELD(FLOAT,
                "smooth-factor", CYAML_FLAG_OPTIONAL,
                SmoothSettings, smoothFactor, {.missing = -1}),
    CYAML_FIELD_END};

static const cyaml_schema_field_t paintable_shader_config_fields_schema[] = {
    CYAML_FIELD_STRING_PTR(
        "class-name", CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
        PaintableShaderProps, className, 0, CYAML_UNLIMITED),

    CYAML_FIELD(BOOL,
                "anchor-left", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, anchorLeft, {.missing = false}),

    CYAML_FIELD(BOOL,
                "anchor-right", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, anchorRight, {.missing = false}),

    CYAML_FIELD(BOOL,
                "anchor-top", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, anchorTop, {.missing = false}),

    CYAML_FIELD(BOOL,
                "exclusive-layer", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, exclusiveZone, {.missing = false}),

    CYAML_FIELD(BOOL,
                "anchor-bottom", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, anchorBottom, {.missing = false}),

    CYAML_FIELD(INT,
                "margin-left", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, marginLeft, {.missing = 0}),

    CYAML_FIELD(INT,
                "margin-right", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, marginRight, {.missing = 0}),

    CYAML_FIELD(INT,
                "margin-top", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, marginTop, {.missing = 0}),

    CYAML_FIELD(INT,
                "margin-bottom", CYAML_FLAG_OPTIONAL,
                PaintableShaderProps, marginBottom, {.missing = 0}),

    CYAML_FIELD(ENUM,
                "layer", CYAML_FLAG_POINTER, PaintableShaderProps, layer,
                {.strings = layerStrings, .count = 4}),

    CYAML_FIELD(UINT,
                "window-width", CYAML_FLAG_POINTER,
                PaintableShaderProps, windowWidth, {}),

    CYAML_FIELD(UINT,
                "window-height", CYAML_FLAG_POINTER,
                PaintableShaderProps, windowHeight, {}),
    CYAML_FIELD_END};

static const cyaml_schema_field_t post_processing_shader_config_fields_schema[] = {
    CYAML_FIELD_STRING_PTR(
        "name", CYAML_FLAG_POINTER,
        PostProcessingShader, shaderName, 0, CYAML_UNLIMITED),

    CYAML_FIELD(UINT,
                "passes", CYAML_FLAG_OPTIONAL,
                PostProcessingShader, passes, {.missing = 1}),

    CYAML_FIELD_END};

static const cyaml_schema_value_t paintable_shader_config_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, PaintableShaderProps, paintable_shader_config_fields_schema),
};

static const cyaml_schema_value_t post_processing_shader_config_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, PostProcessingShader, post_processing_shader_config_fields_schema),
};

static const cyaml_schema_field_t super_audio_map_fields_schema[] = {

    CYAML_FIELD(INT,
                "left", CYAML_FLAG_OPTIONAL,
                AudioMap, left, {.missing = 0}),

    CYAML_FIELD(INT,
                "right", CYAML_FLAG_OPTIONAL,
                AudioMap, right, {.missing = 1}),

    CYAML_FIELD_END};

static const cyaml_schema_field_t overriden_audio_map_fields_schema[] = {

    CYAML_FIELD(INT,
                "left", CYAML_FLAG_OPTIONAL,
                AudioMap, left, {.missing = -1}),

    CYAML_FIELD(INT,
                "right", CYAML_FLAG_OPTIONAL,
                AudioMap, right, {.missing = -1}),

    CYAML_FIELD_END};

static const cyaml_schema_field_t super_audio_overrides_fields_schema[] = {

    CYAML_FIELD_BOOL_PTR(
        "smooth-audio", CYAML_FLAG_OPTIONAL,
        AudioOverride, smoothAudio),

    CYAML_FIELD_BOOL_PTR(
        "strict-frequency-bounds", CYAML_FLAG_OPTIONAL,
        AudioOverride, strictFrequencyBounds),

    CYAML_FIELD(UINT,
                "min-frequency", CYAML_FLAG_OPTIONAL,
                AudioOverride, minFrequency, {.missing = 0}),

    CYAML_FIELD(UINT,
                "max-frequency", CYAML_FLAG_OPTIONAL,
                AudioOverride, maxFrequency, {.missing = 22000}),

    CYAML_FIELD_MAPPING_PTR(
        "channels-map",
        CYAML_FLAG_OPTIONAL,
        AudioOverride, audioMap, super_audio_map_fields_schema),

    CYAML_FIELD_MAPPING_PTR(
        "smooth-settings",
        CYAML_FLAG_OPTIONAL,
        AudioOverride, smoothSettings, super_smooth_settings_fields_schema),
    CYAML_FIELD_MAPPING_PTR(
        "gravity-settings",
        CYAML_FLAG_OPTIONAL,
        AudioOverride, gravitySettings, super_gravity_settings_fields_schema),

    CYAML_FIELD_END};

static const cyaml_schema_field_t overriden_audio_overrides_fields_schema[] = {

    CYAML_FIELD_BOOL_PTR(
        "smooth-audio", CYAML_FLAG_OPTIONAL,
        AudioOverride, smoothAudio),

    CYAML_FIELD_BOOL_PTR(
        "strict-frequency-bounds", CYAML_FLAG_OPTIONAL,
        AudioOverride, strictFrequencyBounds),

    CYAML_FIELD(INT,
                "min-frequency", CYAML_FLAG_OPTIONAL,
                AudioOverride, minFrequency, {.missing = -1}),

    CYAML_FIELD(INT,
                "max-frequency", CYAML_FLAG_OPTIONAL,
                AudioOverride, maxFrequency, {.missing = -1}),

    CYAML_FIELD_MAPPING_PTR(
        "channels-map",
        CYAML_FLAG_OPTIONAL,
        AudioOverride, audioMap, overriden_audio_map_fields_schema),

    CYAML_FIELD_MAPPING_PTR(
        "smooth-settings",
        CYAML_FLAG_OPTIONAL,
        AudioOverride, smoothSettings, overriden_smooth_settings_fields_schema),
    CYAML_FIELD_MAPPING_PTR(
        "gravity-settings",
        CYAML_FLAG_OPTIONAL,
        AudioOverride, gravitySettings, overriden_gravity_settings_fields_schema),

    CYAML_FIELD_END};

static const cyaml_schema_value_t string_ptr_schema = {
    CYAML_VALUE_STRING(CYAML_FLAG_POINTER, char, 0, CYAML_UNLIMITED),
};

static const cyaml_schema_field_t shader_fields_schema[] = {
    CYAML_FIELD_STRING_PTR(
        "name", CYAML_FLAG_POINTER,
        ShaderProps, shaderName, 0, CYAML_UNLIMITED),

    CYAML_FIELD_STRING_PTR(
        "class-name", CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
        ShaderProps, className, 0, CYAML_UNLIMITED),

    CYAML_FIELD_STRING_PTR(
        "config-file", CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
        ShaderProps, configFileName, 0, CYAML_UNLIMITED),

    CYAML_FIELD_MAPPING_PTR("audio-overrides", CYAML_FLAG_OPTIONAL,
                            ShaderProps, audioOverrides,
                            overriden_audio_overrides_fields_schema),

    CYAML_FIELD_SEQUENCE(
        "paintables", CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
        ShaderProps, paintableShaderProps,
        &paintable_shader_config_schema, 0, CYAML_UNLIMITED),

    CYAML_FIELD_SEQUENCE(
        "post-processing", CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
        ShaderProps, postProcessingShaders,
        &post_processing_shader_config_schema, 0, CYAML_UNLIMITED),

    CYAML_FIELD_SEQUENCE_COUNT(
        "overrides", CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
        ShaderProps, shaderOverrides, shaderOverrides_count,
        &string_ptr_schema, 0, CYAML_UNLIMITED),

    CYAML_FIELD(BOOL,
                "anchor-left", CYAML_FLAG_OPTIONAL,
                ShaderProps, anchorLeft, {.missing = false}),
    CYAML_FIELD(BOOL,
                "anchor-right", CYAML_FLAG_OPTIONAL,
                ShaderProps, anchorRight, {.missing = false}),
    CYAML_FIELD(BOOL,
                "anchor-top", CYAML_FLAG_OPTIONAL,
                ShaderProps, anchorTop, {.missing = false}),
    CYAML_FIELD(BOOL,
                "anchor-bottom", CYAML_FLAG_OPTIONAL,
                ShaderProps, anchorBottom, {.missing = false}),

    CYAML_FIELD(BOOL,
                "exclusive-layer", CYAML_FLAG_OPTIONAL,
                ShaderProps, exclusiveZone, {.missing = false}),

    CYAML_FIELD(INT,
                "margin-left", CYAML_FLAG_OPTIONAL,
                ShaderProps, marginLeft, {.missing = 0}),
    CYAML_FIELD(INT,
                "margin-right", CYAML_FLAG_OPTIONAL,
                ShaderProps, marginRight, {.missing = 0}),
    CYAML_FIELD(INT,
                "margin-top", CYAML_FLAG_OPTIONAL,
                ShaderProps, marginTop, {.missing = 0}),
    CYAML_FIELD(INT,
                "margin-bottom", CYAML_FLAG_OPTIONAL,
                ShaderProps, marginBottom, {.missing = 0}),

    CYAML_FIELD(UINT, "fps", CYAML_FLAG_OPTIONAL, ShaderProps, fps, {.missing = 60}),

    CYAML_FIELD(ENUM,
                "layer", CYAML_FLAG_DEFAULT, ShaderProps, layer,
                {.strings = layerStrings, .count = 4}),

    CYAML_FIELD(UINT,
                "atomic-textures", CYAML_FLAG_OPTIONAL,
                ShaderProps, atomicTextures, {.missing = 0}),

    CYAML_FIELD(UINT,
                "image-textures", CYAML_FLAG_OPTIONAL,
                ShaderProps, imageTextures, {.missing = 0}),

    CYAML_FIELD(UINT,
                "window-width", CYAML_FLAG_DEFAULT,
                ShaderProps, windowWidth, {}),
    CYAML_FIELD(UINT,
                "window-height", CYAML_FLAG_DEFAULT,
                ShaderProps, windowHeight, {}),

    CYAML_FIELD_END};

static const cyaml_schema_value_t shaders_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, ShaderProps, shader_fields_schema),
};

static const cyaml_schema_field_t pipewire_settings_fields_schema[] = {

    CYAML_FIELD(UINT,
                "sample-rate", CYAML_FLAG_OPTIONAL,
                PipeWireHandler, sampleRate, {.missing = 22050}),
    CYAML_FIELD(UINT,
                "channels",
                CYAML_FLAG_OPTIONAL,
                PipeWireHandler, channels, {.missing = 2}),

    CYAML_FIELD(FLOAT,
                "fft-scale", CYAML_FLAG_OPTIONAL,
                PipeWireHandler, fftScale, {.missing = 10.2f}),

    CYAML_FIELD(FLOAT,
                "fft-cutOff", CYAML_FLAG_OPTIONAL,
                PipeWireHandler, fftCutOff, {.missing = 0.3f}),
    CYAML_FIELD(
        UINT,
        "sample-size", CYAML_FLAG_OPTIONAL,
        PipeWireHandler, sampleSize, {.missing = 1024}),
    CYAML_FIELD(
        UINT,
        "fragment-size", CYAML_FLAG_OPTIONAL,
        PipeWireHandler, fragmentSize, {.missing = 4096}),

    CYAML_FIELD(BOOL, "capture-mic", CYAML_FLAG_OPTIONAL,
                PipeWireHandler, captureMic, {.missing = false}),
    CYAML_FIELD_STRING_PTR(
        "target-object", CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
        PipeWireHandler, targetObject, 0, CYAML_UNLIMITED),

    CYAML_FIELD(BOOL, "apply-fft", CYAML_FLAG_OPTIONAL,
                PipeWireHandler, applyFFT, {.missing = true}),
    CYAML_FIELD_STRING_PTR(
        "audio-format", CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
        PipeWireHandler, audioFormat, 0, CYAML_UNLIMITED),

    CYAML_FIELD_END};

static const cyaml_schema_field_t audio_fields_schema[] = {

    CYAML_FIELD_STRING_PTR(
        "name", CYAML_FLAG_POINTER,
        Audio, audioName, 0, CYAML_UNLIMITED),

    CYAML_FIELD_MAPPING_PTR("pipewire-settings",
                            CYAML_FLAG_OPTIONAL,
                            Audio, pipewireSettings, pipewire_settings_fields_schema),

    CYAML_FIELD_PTR(MAPPING, "audio-overrides",
                    CYAML_FLAG_OPTIONAL,
                    Audio, audioOverrides, {.fields = super_audio_overrides_fields_schema}),

    CYAML_FIELD_SEQUENCE(
        "shaders", CYAML_FLAG_POINTER,
        Audio, shaderProps,
        &shaders_schema, 1, CYAML_UNLIMITED),

    CYAML_FIELD_END};

static const cyaml_schema_value_t audio_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, Audio, audio_fields_schema),
};

static const cyaml_schema_field_t config_fields_schema[] = {

    CYAML_FIELD_SEQUENCE(
        "audios", CYAML_FLAG_POINTER,
        YAMLConfig, audios,
        &audio_schema, 1, CYAML_UNLIMITED),

    CYAML_FIELD_END

};

static const cyaml_schema_value_t config_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER,
                        YAMLConfig, config_fields_schema),
};

static const cyaml_config_t libCYAMLConfig = {
    .log_fn = cyaml_log,            /* Use the default logging function. */
    .mem_fn = cyaml_mem,            /* Use the default memory allocator. */
    .log_level = CYAML_LOG_WARNING, /* Logging errors and warnings only. */
};

#endif