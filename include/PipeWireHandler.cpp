#include <math.h>
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <iostream>

#include "CYAMLClasses.h"
#include "Errors.h"

using namespace std;

void PipeWireHandler::processAudioBuffer(void *userdata)
{
  PipeWireHandler *pipeWireSetting = (PipeWireHandler *)userdata;
  struct pw_buffer *pwBuffer;
  struct spa_buffer *spaBuffer;

  float *samples, max;
  uint32_t c, n, n_channels, n_samples;

  if ((pwBuffer = pw_stream_dequeue_buffer(pipeWireSetting->stream)) == NULL)
  {
    pw_log_warn("out of buffers: %m");
    return;
  }

  spaBuffer = pwBuffer->buffer;
  if ((samples = (float *)spaBuffer->datas[0].data) == NULL)
    return;

  pw_stream_queue_buffer(pipeWireSetting->stream, pwBuffer);

  pthread_mutex_lock(&pipeWireSetting->mutex);

  n_channels = pipeWireSetting->channels;
  n_samples = spaBuffer->datas[0].chunk->size / sizeof(float);

  int offset = pipeWireSetting->sampleSize / 4;

  for (n = 0; n < n_samples; n++)
  {

    if (pipeWireSetting->filledIdx <= pipeWireSetting->sampleSize / 2)
    {
      pipeWireSetting->audioBuffer[pipeWireSetting->filledIdx] = samples[n];
      pipeWireSetting->filledIdx++;
    }
    else
    {

      memmove(pipeWireSetting->br, &pipeWireSetting->br[pipeWireSetting->sampleSize / 4 + offset],
              (pipeWireSetting->fragmentSize - (pipeWireSetting->sampleSize / 4) - offset) * sizeof(float));
      memmove(pipeWireSetting->bl, &pipeWireSetting->bl[pipeWireSetting->sampleSize / 4 + offset],
              (pipeWireSetting->fragmentSize - (pipeWireSetting->sampleSize / 4) - offset) * sizeof(float));

      for (int k = 0, i = 0; i < pipeWireSetting->sampleSize / 2; i += 1 + (pipeWireSetting->applyFFT ? 1 : 0), k += 1 + (pipeWireSetting->applyFFT ? 1 : 0))
      {

        int idx = (pipeWireSetting->fragmentSize - (pipeWireSetting->sampleSize / 4) - offset) + k;

        if (n_channels == 1) // 1 channel
        {
          float sample = (pipeWireSetting->audioBuffer[i] + pipeWireSetting->audioBuffer[i + 1]) / 2;
          pipeWireSetting->br[idx] = sample;
          pipeWireSetting->bl[idx] = sample;
        }

        /* stereo storing channels in buffer */
        else if (n_channels == 2) // 2 channels
        {
          pipeWireSetting->br[idx] = pipeWireSetting->audioBuffer[i + 1];
          pipeWireSetting->bl[idx] = pipeWireSetting->audioBuffer[i];
        }

        if (pipeWireSetting->applyFFT)
        {
          pipeWireSetting->br[idx + 1] = 0;
          pipeWireSetting->bl[idx + 1] = 0;
        }
      }

      pipeWireSetting->filledIdx = 0;

      memcpy(pipeWireSetting->rb, (void *)pipeWireSetting->br, pipeWireSetting->fragmentSize * sizeof(float));
      memcpy(pipeWireSetting->lb, (void *)pipeWireSetting->bl, pipeWireSetting->fragmentSize * sizeof(float));

      pipeWireSetting->audioRData = pipeWireSetting->rb;
      pipeWireSetting->audioRSize = pipeWireSetting->fragmentSize;
      pipeWireSetting->audioLData = pipeWireSetting->lb;
      pipeWireSetting->audioLSize = pipeWireSetting->fragmentSize;

      pipeWireSetting->modified = true;
    }
  }

  pthread_mutex_unlock(&pipeWireSetting->mutex);
}

void PipeWireHandler::onStreamParameterChanged(void *_data, uint32_t id,
                                               const struct spa_pod *param)
{
  PipeWireHandler *data = (PipeWireHandler *)_data;

  if (param == NULL || id != SPA_PARAM_Format)
    return;

  if (spa_format_parse(param, &data->format.media_type,
                       &data->format.media_subtype) < 0)
    return;

  if (data->format.media_type != SPA_MEDIA_TYPE_audio ||
      data->format.media_subtype != SPA_MEDIA_SUBTYPE_raw)
    return;

  spa_format_audio_raw_parse(param, &data->format.info.raw);

  cout << "Starting Audio-capturer Thread " << data->audioName << ", Capturing Rate: " << data->format.info.raw.rate
       << ", Channels: " << data->format.info.raw.channels << endl;
}

void PipeWireHandler::onStateChanged(void *userdata, enum pw_stream_state old_state,
                                     enum pw_stream_state new_state, const char *error)
{

  PipeWireHandler *pipeWireData = (PipeWireHandler *)userdata;
  struct pw_buffer *pwBuffer;

  if ((pwBuffer = pw_stream_dequeue_buffer(pipeWireData->stream)) == NULL)
  {
    pw_log_warn("out of buffers: %m");
    return;
  }
  pw_stream_queue_buffer(pipeWireData->stream, pwBuffer);

  return;
}

void PipeWireHandler::onQuit(void *userdata, int signal_number)
{
  PipeWireHandler *data = (PipeWireHandler *)userdata;
  pw_main_loop_quit(data->loop);
}

void PipeWireHandler::connectStream()
{
  stream =
      pw_stream_new_simple(pw_main_loop_get_loop(loop), "audio-capture",
                           props, &stream_events, this);

  pw_stream_connect(stream, PW_DIRECTION_INPUT, PW_ID_ANY,
                    (enum pw_stream_flags)(PW_STREAM_FLAG_AUTOCONNECT |
                                           PW_STREAM_FLAG_MAP_BUFFERS |
                                           PW_STREAM_FLAG_RT_PROCESS),
                    podParameters, 1);
}

void PipeWireHandler::setProps()
{

  uint8_t buffer[sampleSize];

  const std::map<std::string, enum spa_audio_format> enumsMap = {
      {"UNKNOWN", SPA_AUDIO_FORMAT_UNKNOWN},
      {"ENCODED", SPA_AUDIO_FORMAT_ENCODED},
      {"START_Interleaved", SPA_AUDIO_FORMAT_START_Interleaved},
      {"S8", SPA_AUDIO_FORMAT_S8},
      {"U8", SPA_AUDIO_FORMAT_U8},
      {"S16_LE", SPA_AUDIO_FORMAT_S16_LE},
      {"S16_BE", SPA_AUDIO_FORMAT_S16_BE},
      {"U16_LE", SPA_AUDIO_FORMAT_U16_LE},
      {"U16_BE", SPA_AUDIO_FORMAT_U16_BE},
      {"S24_32_LE", SPA_AUDIO_FORMAT_S24_32_LE},
      {"S24_32_BE", SPA_AUDIO_FORMAT_S24_32_BE},
      {"U24_32_LE", SPA_AUDIO_FORMAT_U24_32_LE},
      {"U24_32_BE", SPA_AUDIO_FORMAT_U24_32_BE},
      {"S32_LE", SPA_AUDIO_FORMAT_S32_LE},
      {"S32_BE", SPA_AUDIO_FORMAT_S32_BE},
      {"U32_LE", SPA_AUDIO_FORMAT_U32_LE},
      {"U32_BE", SPA_AUDIO_FORMAT_U32_BE},
      {"S24_LE", SPA_AUDIO_FORMAT_S24_LE},
      {"S24_BE", SPA_AUDIO_FORMAT_S24_BE},
      {"U24_LE", SPA_AUDIO_FORMAT_U24_LE},
      {"U24_BE", SPA_AUDIO_FORMAT_U24_BE},
      {"S20_LE", SPA_AUDIO_FORMAT_S20_LE},
      {"S20_BE", SPA_AUDIO_FORMAT_S20_BE},
      {"U20_LE", SPA_AUDIO_FORMAT_U20_LE},
      {"U20_BE", SPA_AUDIO_FORMAT_U20_BE},
      {"S18_LE", SPA_AUDIO_FORMAT_S18_LE},
      {"S18_BE", SPA_AUDIO_FORMAT_S18_BE},
      {"U18_LE", SPA_AUDIO_FORMAT_U18_LE},
      {"U18_BE", SPA_AUDIO_FORMAT_U18_BE},
      {"F32_LE", SPA_AUDIO_FORMAT_F32_LE},
      {"F32_BE", SPA_AUDIO_FORMAT_F32_BE},
      {"F64_LE", SPA_AUDIO_FORMAT_F64_LE},
      {"F64_BE", SPA_AUDIO_FORMAT_F64_BE},
      {"ULAW", SPA_AUDIO_FORMAT_ULAW},
      {"ALAW", SPA_AUDIO_FORMAT_ALAW},
      {"START_Planar", SPA_AUDIO_FORMAT_START_Planar},
      {"U8P", SPA_AUDIO_FORMAT_U8P},
      {"S16P", SPA_AUDIO_FORMAT_S16P},
      {"S24_32P", SPA_AUDIO_FORMAT_S24_32P},
      {"S32P", SPA_AUDIO_FORMAT_S32P},
      {"S24P", SPA_AUDIO_FORMAT_S24P},
      {"F32P", SPA_AUDIO_FORMAT_F32P},
      {"F64P", SPA_AUDIO_FORMAT_F64P},
      {"S8P", SPA_AUDIO_FORMAT_S8P},
      {"START_Other", SPA_AUDIO_FORMAT_START_Other},
      {"DSP_S32", SPA_AUDIO_FORMAT_DSP_S32},
      {"DSP_F32", SPA_AUDIO_FORMAT_DSP_F32},
      {"DSP_F64", SPA_AUDIO_FORMAT_DSP_F64},
      {"S16", SPA_AUDIO_FORMAT_S16},
      {"U16", SPA_AUDIO_FORMAT_U16},
      {"S24_32", SPA_AUDIO_FORMAT_S24_32},
      {"U24_32", SPA_AUDIO_FORMAT_U24_32},
      {"S32", SPA_AUDIO_FORMAT_S32},
      {"U32", SPA_AUDIO_FORMAT_U32},
      {"S24", SPA_AUDIO_FORMAT_S24},
      {"U24", SPA_AUDIO_FORMAT_U24},
      {"S20", SPA_AUDIO_FORMAT_S20},
      {"U20", SPA_AUDIO_FORMAT_U20},
      {"S18", SPA_AUDIO_FORMAT_S18},
      {"U18", SPA_AUDIO_FORMAT_U18},
      {"F32", SPA_AUDIO_FORMAT_F32},
      {"F64", SPA_AUDIO_FORMAT_F64},
      {"S16_OE", SPA_AUDIO_FORMAT_S16_OE},
      {"U16_OE", SPA_AUDIO_FORMAT_U16_OE},
      {"S24_32_OE", SPA_AUDIO_FORMAT_S24_32_OE},
      {"U24_32_OE", SPA_AUDIO_FORMAT_U24_32_OE},
      {"S32_OE", SPA_AUDIO_FORMAT_S32_OE},
      {"U32_OE", SPA_AUDIO_FORMAT_U32_OE},
      {"S24_OE", SPA_AUDIO_FORMAT_S24_OE},
      {"U24_OE", SPA_AUDIO_FORMAT_U24_OE},
      {"S20_OE", SPA_AUDIO_FORMAT_S20_OE},
      {"U20_OE", SPA_AUDIO_FORMAT_U20_OE},
      {"S18_OE", SPA_AUDIO_FORMAT_S18_OE},
      {"U18_OE", SPA_AUDIO_FORMAT_U18_OE},
      {"F32_OE", SPA_AUDIO_FORMAT_F32_OE},
      {"F64_OE", SPA_AUDIO_FORMAT_F64_OE}};

  struct spa_pod_builder podBuilder =
      SPA_POD_BUILDER_INIT(buffer, (unsigned int)sizeof(buffer));

  props = pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio", PW_KEY_APP_NAME,
                            audioName,

                            PW_KEY_MEDIA_CATEGORY, "Capture", PW_KEY_MEDIA_ROLE,
                            "Music", NULL);

  pw_properties_set(props, PW_KEY_NODE_ALWAYS_PROCESS, "true");

  if (!captureMic)
    pw_properties_set(props, PW_KEY_STREAM_CAPTURE_SINK, "true");

  if (targetObject != NULL)
    pw_properties_set(props, PW_KEY_TARGET_OBJECT, targetObject);

  if (enumsMap.find(std::string(audioFormat)) == enumsMap.end())
    Errors::throwError("Audio Format " + std::string(audioFormat) + " does not exist", "", "In " + std::string(audioName));

  struct spa_audio_info_raw audioRawInfo =
      SPA_AUDIO_INFO_RAW_INIT(.format = enumsMap.at(std::string(audioFormat)),
                              .rate = sampleRate,
                              .channels = 2);

  podParameters[0] =
      spa_format_audio_raw_build(&podBuilder, SPA_PARAM_EnumFormat, &audioRawInfo);

  connectStream();
}

void PipeWireHandler::runLoop()
{
  pw_main_loop_run(loop);
}

PipeWireHandler::~PipeWireHandler()
{
  pw_stream_destroy(stream);
  pw_main_loop_destroy(loop);
  pw_deinit();
}

void PipeWireHandler::createLoop()
{
  pw_init(NULL, NULL);
  loop = pw_main_loop_new(NULL);
}

void *PipeWireHandler::runAudio(PipeWireHandler *pipeWireHandler)
{
  sigset_t set;
  sigaddset(&set, SIGSTOP);
  sigaddset(&set, SIGTSTP);

  int threadID = pthread_sigmask(SIG_BLOCK, &set, NULL);

  pipeWireHandler->createLoop();

  pipeWireHandler->setProps();

  pipeWireHandler->runLoop();

  return NULL;
}

void PipeWireHandler::createPipeWireThread()
{
  audioBuffer = new float[sampleSize / 2]();

  br = new float[sampleSize * 4]();
  bl = new float[sampleSize * 4]();

  rb = new float[fragmentSize * sizeof(float)]();
  lb = new float[fragmentSize * sizeof(float)]();

  pthread_t pThread;
  int threadID;
  pthread_mutex_init(&mutex, NULL);

  {
    sigset_t set;
    sigfillset(&set);

    threadID = pthread_sigmask(SIG_SETMASK, &set, NULL);
    threadID = pthread_create(&pThread, NULL, (void *(*)(void *))(runAudio),
                              (void *)this);

    sigemptyset(&set);

    pthread_sigmask(SIG_SETMASK, &set, NULL);
    pthread_detach(pThread);
  }
}