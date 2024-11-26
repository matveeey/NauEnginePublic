// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.


#include "nau/audio/audio_component_emitter.hpp"

#include "nau/service/service_provider.h"
#include "nau/scene/scene_manager.h"
#include "nau/scene/world.h"

#include "nau/audio/audio_service.hpp"

#include <filesystem>


NAU_AUDIO_BEGIN

NAU_IMPLEMENT_DYNAMIC_OBJECT(AudioComponentEmitter)

void AudioComponentEmitter::updateComponent(float dt)
{
    // Temporary solution while we don't have proper play mode
    const auto paused = getServiceProvider().get<scene::ISceneManager>().getDefaultWorld().isSimulationPaused();
    if (paused) {
        if (state != Unloaded) {
            if (source) {
                source->stop();
                NAU_ASSERT(source.use_count() == 1);
                source.reset();
            }
            state = Unloaded;
        }
        return;
    }
    
    if (state == Unloaded) {
        if (playOnStart) {
            if (!container) {
                NAU_LOG_WARNING("Trying to start an audio emitter that doesn't have a container attached!");
                return;
            }
            source = container->instantiate();
            if (loop) {
                source->setEndCallback([this] {
                    source->stop();
                    source->rewind();
                    source->play();
                });
            }
            source->play();
            state = Playing;
        }
    }
}

void AudioComponentEmitter::activateComponent()
{
    NAU_LOG_DEBUG("Audio emmiter component activated");

    auto& engine = nau::getServiceProvider().get<nau::audio::AudioService>().engine();
    const auto containers = engine.containerAssets();
    const auto itContainer = std::find_if(containers.begin(), containers.end(), [this](nau::audio::AudioAssetContainerPtr container) {
        return std::filesystem::path(container->name().c_str()) == std::filesystem::path(path.c_str());
    });

    if (itContainer != containers.end()) {
        container = *itContainer;
    }

    // TODO: log
}

void AudioComponentEmitter::deactivateComponent()
{
    NAU_LOG_DEBUG("Audio emmiter component deactivated");

}

NAU_AUDIO_END