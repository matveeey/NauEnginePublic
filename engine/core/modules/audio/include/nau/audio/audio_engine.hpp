// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.

#pragma once

#include "audio_source.hpp"
#include "audio_container.hpp"
#include "audio_asset.hpp"


NAU_AUDIO_BEGIN

// ** IAudioEngine

class NAU_AUDIO_EXPORT IAudioEngine
{
public:

	enum Backend {
		Miniaudio = 0,
	};

	static std::unique_ptr<IAudioEngine> create(Backend backend);

	virtual ~IAudioEngine() = default;
	virtual void initialize() = 0;
	virtual void deinitialize() = 0;
	virtual void update() = 0;

	// Asset creation
	virtual AudioAssetPtr loadSound(const eastl::string& path) = 0;
	virtual AudioAssetPtr loadStream(const eastl::string& path) = 0;
	virtual AudioAssetContainerPtr createContainer(const eastl::string& name);

	// Asset access
	virtual AudioAssetList audioAssets() = 0;
	virtual AudioAssetContainerList containerAssets();
	virtual AudioAssetList assets();

private:
	AudioAssetContainerList m_containers;
};

using AudioEnginePtr = std::unique_ptr<IAudioEngine>;

NAU_AUDIO_END
