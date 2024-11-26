// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.

#pragma once

#include "nau/rtti/rtti_impl.h"
#include "nau/scene/camera/camera_manager.h"
#include "nau/scene/scene_processor.h"
#include "nau/service/service.h"

namespace nau::scene
{
    class CameraManagerImpl final : public ICameraManager,
                                    public IComponentsActivator,
                                    public IServiceInitialization
    {
        NAU_RTTI_CLASS(nau::scene::CameraManagerImpl, ICameraManager, IComponentsActivator, IServiceInitialization)

    private:
        async::Task<> preInitService() override;

        CameraCollection getCameras() override;
        void syncCameras(CameraCollection& cameras, SyncCameraCallback onCameraAdded, SyncCameraCallback onCameraRemoved) override;
        nau::Ptr<ICameraControl> createDetachedCamera(Uid worldUid) override;
        Result<> activateComponents(Uid worldUid, eastl::span<Component*> components) override;
        void deactivateComponents(Uid worldUid, eastl::span<Component*> components) override;

        mutable std::mutex m_mutex;
        std::thread::id m_syncThreadId;
        eastl::list<ObjectWeakRef<ICameraControl>> m_sceneCameras;
        eastl::list<nau::WeakPtr<ICameraControl>> m_detachedCameras;
    };
}  // namespace nau::scene
