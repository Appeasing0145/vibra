#include "audio/downsampler.h"

#include <cmath>
#include <cstring>

#include <algorithm>
#include <map>
#include <tuple>

#include "audio/byte_control.h"
#include "audio/wav.h"

namespace vibra {

double Downsampler::readSignedFrameAsMono(const void* src, std::uint32_t frame,
                                          std::uint32_t width,
                                          std::uint32_t channels) {
  double sample = 0.0;
  for (std::uint32_t channel = 0; channel < channels; channel++) {
    const std::uint32_t index = (frame * channels + channel) * width;
    sample +=
        GETSAMPLE64(width, src, index) >> (64 - LOW_QUALITY_SAMPLE_BIT_WIDTH);
  }
  return sample / channels;
}

template <typename T>
double Downsampler::readFloatFrameAsMono(const void* src, std::uint32_t frame,
                                         std::uint32_t width,
                                         std::uint32_t channels) {
  double sample = 0.0;
  for (std::uint32_t channel = 0; channel < channels; channel++) {
    const std::uint32_t index = (frame * channels + channel) * width;
    const std::uint64_t raw_sample =
        GETSAMPLE64(width, src, index) >> (64 - sizeof(T) * 8);
    T float_sample = 0;
    std::memcpy(&float_sample, &raw_sample, sizeof(T));
    sample += float_sample * LOW_QUALITY_SAMPLE_MAX;
  }
  return sample / channels;
}

LowQualityTrack Downsampler::GetLowQualityPCM(const Wav& wav,
                                              std::int32_t start_sec,
                                              std::int32_t end_sec) {
  LowQualityTrack low_quality_pcm;

  const auto channels = wav.num_channels();
  const auto sample_rate = wav.sample_rate_();
  const auto bits_per_sample = wav.bits_per_sample();
  const auto data_size = wav.data_size();
  const auto audio_format = wav.audio_format();
  const std::uint8_t* pcm_data = wav.data().get();

  if (channels == 1 && sample_rate == LOW_QUALITY_SAMPLE_RATE &&
      bits_per_sample == LOW_QUALITY_SAMPLE_BIT_WIDTH && start_sec == 0 &&
      end_sec == -1) {
    // no need to convert low quality pcm. just copy raw data
    low_quality_pcm.resize(data_size / sizeof(LowQualitySample));
    std::memcpy(low_quality_pcm.data(), wav.data().get(), data_size);
    return low_quality_pcm;
  }

  double downsample_ratio =
      sample_rate / static_cast<double>(LOW_QUALITY_SAMPLE_RATE);
  std::uint32_t width = bits_per_sample / 8;
  std::uint32_t sample_count = data_size / width;

  const void* src_raw_data =
      pcm_data + (start_sec * sample_rate * width * channels);

  std::uint32_t new_sample_count = sample_count / channels / downsample_ratio;

  if (end_sec != -1) {
    new_sample_count = (end_sec - start_sec) * LOW_QUALITY_SAMPLE_RATE;
  }

  low_quality_pcm.resize(new_sample_count);

  auto downsample_func = &Downsampler::signedMonoToMono;
  bool is_signed = audio_format == 1;

  downsample_func = getDownsampleFunc(is_signed, bits_per_sample, channels);
  downsample_func(&low_quality_pcm, src_raw_data, downsample_ratio,
                  new_sample_count, width, channels);
  return low_quality_pcm;
}

DownsampleFunc Downsampler::getDownsampleFunc(bool is_signed,
                                              std::uint32_t width,
                                              std::uint32_t channels) {
  channels = std::min(channels, 3u);
  width = is_signed ? 0 : width;

  static std::map<std::tuple<bool, std::uint32_t, std::uint32_t>,
                  DownsampleFunc>
      func_map{
          {std::make_tuple(true, 0, 1), &Downsampler::signedMonoToMono},
          {std::make_tuple(true, 0, 2), &Downsampler::signedMonoToMono},
          {std::make_tuple(true, 0, 3), &Downsampler::signedMonoToMono},
          {std::make_tuple(false, 32, 1), &Downsampler::floatMonoToMono<float>},
          {std::make_tuple(false, 32, 2), &Downsampler::floatMonoToMono<float>},
          {std::make_tuple(false, 32, 3), &Downsampler::floatMonoToMono<float>},
          {std::make_tuple(false, 64, 1),
           &Downsampler::floatMonoToMono<double>},
          {std::make_tuple(false, 64, 2),
           &Downsampler::floatMonoToMono<double>},
          {std::make_tuple(false, 64, 3),
           &Downsampler::floatMonoToMono<double>},
      };
  return func_map.at(std::make_tuple(is_signed, width, channels));
}

void Downsampler::signedMonoToMono(LowQualityTrack* dst, const void* src,
                                   double downsample_ratio,
                                   std::uint32_t new_sample_count,
                                   std::uint32_t width,
                                   std::uint32_t channels) {
  const std::uint32_t source_frame_count =
      std::max(1u, static_cast<std::uint32_t>(
                       std::ceil(new_sample_count * downsample_ratio)));
  for (std::uint32_t i = 0; i < new_sample_count; i++) {
    const double source_position = i * downsample_ratio;
    std::uint32_t frame = static_cast<std::uint32_t>(source_position);
    if (frame >= source_frame_count) {
      frame = source_frame_count - 1;
    }
    const std::uint32_t next_frame =
        std::min(frame + 1, source_frame_count - 1);
    const double fraction = source_position - frame;
    const double current_sample =
        readSignedFrameAsMono(src, frame, width, channels);
    const double next_sample =
        readSignedFrameAsMono(src, next_frame, width, channels);
    dst->at(i) = static_cast<LowQualitySample>(
        current_sample * (1.0 - fraction) + next_sample * fraction);
  }
}

void Downsampler::signedStereoToMono(LowQualityTrack* dst, const void* src,
                                     double downsample_ratio,
                                     std::uint32_t new_sample_count,
                                     std::uint32_t width,
                                     std::uint32_t channels) {
  signedMonoToMono(dst, src, downsample_ratio, new_sample_count, width,
                   channels);
}

void Downsampler::signedMultiToMono(LowQualityTrack* dst, const void* src,
                                    double downsample_ratio,
                                    std::uint32_t new_sample_count,
                                    std::uint32_t width,
                                    std::uint32_t channels) {
  signedMonoToMono(dst, src, downsample_ratio, new_sample_count, width,
                   channels);
}

template <typename T>
void Downsampler::floatMonoToMono(LowQualityTrack* dst, const void* src,
                                  double downsample_ratio,
                                  std::uint32_t new_sample_count,
                                  std::uint32_t width, std::uint32_t channels) {
  const std::uint32_t source_frame_count =
      std::max(1u, static_cast<std::uint32_t>(
                       std::ceil(new_sample_count * downsample_ratio)));
  for (std::uint32_t i = 0; i < new_sample_count; i++) {
    const double source_position = i * downsample_ratio;
    std::uint32_t frame = static_cast<std::uint32_t>(source_position);
    if (frame >= source_frame_count) {
      frame = source_frame_count - 1;
    }
    const std::uint32_t next_frame =
        std::min(frame + 1, source_frame_count - 1);
    const double fraction = source_position - frame;
    const double current_sample =
        readFloatFrameAsMono<T>(src, frame, width, channels);
    const double next_sample =
        readFloatFrameAsMono<T>(src, next_frame, width, channels);
    dst->at(i) = static_cast<LowQualitySample>(
        current_sample * (1.0 - fraction) + next_sample * fraction);
  }
}

template <typename T>
void Downsampler::floatStereoToMono(LowQualityTrack* dst, const void* src,
                                    double downsample_ratio,
                                    std::uint32_t new_sample_count,
                                    std::uint32_t width,
                                    std::uint32_t channels) {
  floatMonoToMono<T>(dst, src, downsample_ratio, new_sample_count, width,
                     channels);
}

template <typename T>
void Downsampler::floatMultiToMono(LowQualityTrack* dst, const void* src,
                                   double downsample_ratio,
                                   std::uint32_t new_sample_count,
                                   std::uint32_t width,
                                   std::uint32_t channels) {
  floatMonoToMono<T>(dst, src, downsample_ratio, new_sample_count, width,
                     channels);
}

}  // namespace vibra
