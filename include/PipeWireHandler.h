#ifndef PIPE_WIRE_HANDLER_H
#define PIPE_WIRE_HANDLER_H

#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <pthread.h>
#include <map>

class PipeWireHandler
{
private:
    float *br, *bl, *audioBuffer, *rb, *lb;
    int filledIdx = 0;

    pw_main_loop *loop;
    struct pw_stream *stream;
    struct pw_properties *props;
    struct spa_audio_info format;

    const struct spa_pod *podParameters[1];

    static void processAudioBuffer(void *userdata);

    static void onStreamParameterChanged(void *_data, uint32_t id,
                                         const struct spa_pod *param);

    static void onStateChanged(void *userdata, enum pw_stream_state old_state,
                               enum pw_stream_state new_state, const char *error);

    static void onQuit(void *userdata, int signal_number);

    inline static const struct pw_stream_events stream_events = {
        .version = PW_VERSION_STREAM_EVENTS,
        .state_changed = onStateChanged,
        .param_changed = onStreamParameterChanged,
        .process = processAudioBuffer};

    static void *runAudio(PipeWireHandler *pipeWireSetting);

    void createLoop();
    void connectStream();
    void setProps();
    void runLoop();

public:
    char *audioFormat, *audioName, *targetObject = NULL;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    int channels, sampleSize, fragmentSize;
    float fftScale, fftCutOff;
    bool captureMic, applyFFT, modified;
    unsigned int audioLSize = 0, audioRSize = 0, sampleRate;
    float *audioLData = new float, *audioRData = new float;

    ~PipeWireHandler();

    void createPipeWireThread();
};

#endif