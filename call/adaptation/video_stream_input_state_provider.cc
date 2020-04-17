/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "call/adaptation/video_stream_input_state_provider.h"

namespace webrtc {

VideoStreamInputStateProvider::VideoStreamInputStateProvider(
    VideoStreamEncoderObserver* frame_rate_provider)
    : frame_rate_provider_(frame_rate_provider) {}

void VideoStreamInputStateProvider::OnHasInputChanged(bool has_input) {
  rtc::CritScope lock(&crit_);
  input_state_.set_has_input(has_input);
}

void VideoStreamInputStateProvider::OnFrameSizeObserved(int frame_size_pixels) {
  RTC_DCHECK_GT(frame_size_pixels, 0);
  rtc::CritScope lock(&crit_);
  input_state_.set_frame_size_pixels(frame_size_pixels);
}

void VideoStreamInputStateProvider::OnEncoderSettingsChanged(
    EncoderSettings encoder_settings) {
  rtc::CritScope lock(&crit_);
  input_state_.set_video_codec_type(
      encoder_settings.encoder_config().codec_type);
  input_state_.set_min_pixels_per_frame(
      encoder_settings.encoder_info().scaling_settings.min_pixels_per_frame);
}

VideoStreamInputState VideoStreamInputStateProvider::InputState() {
  // GetInputFrameRate() is thread-safe.
  int input_fps = frame_rate_provider_->GetInputFrameRate();
  rtc::CritScope lock(&crit_);
  input_state_.set_frames_per_second(
      input_fps >= 0 ? absl::optional<int>(input_fps) : absl::nullopt);
  return input_state_;
}

}  // namespace webrtc
