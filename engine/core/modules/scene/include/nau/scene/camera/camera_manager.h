// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.

#pragma once

#include "nau/memory/eastl_aliases.h"
#include "nau/rtti/ptr.h"
#include "nau/scene/camera/camera.h"
#include "nau/utils/functor.h"

namespace nau::scene
{
    /**
        @brief Camera management API

            Camera manager mostly is not thread-safe API, because it require properties syncronization with a scene objects/components
        which can only be done on the main/scene thread.
     */
    struct NAU_ABSTRACT_TYPE ICameraManager
    {
        NAU_TYPEID(nau::scene::ICameraManager)

        using CameraCollection = Vector<nau::Ptr<ICameraProperties>>;
        using SyncCameraCallback = Functor<void(ICameraProperties&)>;

        virtual ~ICameraManager() = default;

        /**
            @brief Get all in-game cameras including scene and detached cameras
            The method is not completely thread-safe and should only be called from the main/scene thread.
        */
        virtual CameraCollection getCameras() = 0;

        /**
            @brief Sync cameras collection with all in-game cameras including scene and detached cameras
            After the method completes, the camera collection will contain only existing in-game cameras:
            non-existent cameras are removed, newly created cameras will be added to the camera collection.
            This method is more efficient than getCameras() because cameras that remain unchanged only synchronize their properties with the corresponding camera objects.
            The method is not completely thread-safe and should only be called from the main/scene thread.

            @param cameras Collection of properties of all cameras
            @param onCameraAdded Callback that will be called when a new camera is added to the camera collection
            @param onCameraRemoved Callback that will be called when a camera is removed from the camera collection.
        */
        virtual void syncCameras(CameraCollection& cameras, SyncCameraCallback onCameraAdded = {}, SyncCameraCallback onCameraRemoved = {}) = 0;

        /**
            @brief Creating the detached camera
            The detached camera allows its properties to be read and written in a thread-safe manner.
            The method is thread-safe and can be called from any thread.
        */
        virtual nau::Ptr<ICameraControl> createDetachedCamera(Uid worldUid = NullUid) = 0;
    };

}  // namespace nau::scene
