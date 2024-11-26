// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.

#pragma once 

#include "audio_common.hpp"
#include "audio_source.hpp"
#include "audio_subscribable.hpp"

#include <EASTL/string.h>
#include <EASTL/vector.h>


NAU_AUDIO_BEGIN

// ** IAudioAsset

using AssetChangeCallback = std::function<void()>;

class IAudioAsset : public Subscribable<AssetChangeCallback>
{
public:
    virtual AudioSourcePtr instantiate() = 0;
    virtual eastl::string name() const = 0;
};

using AudioAssetPtr = std::shared_ptr<IAudioAsset>;
using AudioAssetList = eastl::vector<AudioAssetPtr>;

NAU_AUDIO_END
