// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.

#pragma once 

#include "audio_common.hpp"

#include <functional>
#include <memory>
#include <chrono>

#include <EASTL/vector.h>


NAU_AUDIO_BEGIN

// ** SoundCompletionCallback

using SoundCompletionCallback = std::function<void()>;


// ** IAudioSource

class IAudioSource
{
public:
	// Playback
	virtual void play() = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void seek(std::chrono::milliseconds ms) = 0;
	virtual void rewind();
	
	// State
	virtual std::chrono::milliseconds position() const = 0;
	virtual std::chrono::milliseconds duration() const = 0;
	virtual bool isAtEnd() const = 0;
	virtual bool isPlaying() const = 0;

	// Callbacks
	virtual void setEndCallback(SoundCompletionCallback callback) = 0;
	virtual void playNext(std::shared_ptr<IAudioSource> next);
};

using AudioSourcePtr = std::shared_ptr<IAudioSource>;
using AudioSourceList = eastl::vector<AudioSourcePtr>;

NAU_AUDIO_END
