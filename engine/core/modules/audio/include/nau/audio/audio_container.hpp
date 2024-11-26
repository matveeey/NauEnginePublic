// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.

#pragma once 

#include "audio_asset.hpp"
#include "audio_source.hpp"

#include "nau/utils/enum/enum_reflection.h"

NAU_AUDIO_BEGIN

// ** AudioAssetContainer

NAU_DEFINE_ENUM_(AudioContainerKind, Sequence, Random, Shuffle);
// TODO: Add mix

class NAU_AUDIO_EXPORT AudioAssetContainer : public IAudioAsset
{
public:
	AudioAssetContainer(const eastl::string& name);
	~AudioAssetContainer();

	// From IAudioAsset
	AudioSourcePtr instantiate() override;
	eastl::string name() const override;

	AudioAssetList::iterator begin();
	AudioAssetList::iterator end();

	AudioContainerKind kind() const;
	void setKind(AudioContainerKind kind);

	void add(AudioAssetPtr asset);
	void remove(AudioAssetPtr asset);
	
	bool empty() const;

private:
	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};

using AudioAssetContainerPtr = std::shared_ptr<AudioAssetContainer>;
using AudioAssetContainerList = eastl::vector<AudioAssetContainerPtr>;


// ** AudioContainer

class AudioContainer : public IAudioSource
{
public:
	AudioContainer() = default;

	// From IAudioSource
	void play() override;
	void stop() override;
	void pause() override;
	void seek(std::chrono::milliseconds ms) override;

	std::chrono::milliseconds duration() const override;
	std::chrono::milliseconds position() const override;
	bool isAtEnd() const override;
	bool isPlaying() const override;

	void setEndCallback(SoundCompletionCallback callback) override;
	void addSource(AudioSourcePtr source);

private:
	// TODO: pimpl
	AudioSourceList  m_sources;
	AudioSourcePtr   m_current;
};

using AudioContainerPtr = std::shared_ptr<AudioContainer>;

NAU_AUDIO_END
