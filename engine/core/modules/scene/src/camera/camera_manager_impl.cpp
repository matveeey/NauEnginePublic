// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.


#include "camera/camera_manager_impl.h"

#include "camera/detached_camera.h"
#include "camera/readonly_camera.h"
#include "nau/memory/eastl_aliases.h"
#include "nau/scene/components/camera_component.h"
#include "nau/scene/scene_manager.h"
#include "nau/scene/world.h"

namespace nau::scene
{
    async::Task<> CameraManagerImpl::preInitService()
    {
        m_syncThreadId = std::this_thread::get_id();
        return async::Task<>::makeUninitialized();
    }

    ICameraManager::CameraCollection CameraManagerImpl::getCameras()
    {
        NAU_FATAL(m_syncThreadId == std::this_thread::get_id(), "Camera synchronization can be performed only from main thread");
        lock_(m_mutex);

        CameraCollection cameras;
        cameras.reserve(m_detachedCameras.size() + m_sceneCameras.size());

        m_detachedCameras.remove_if([&cameras](nau::WeakPtr<ICameraControl>& cameraWeakPtr) -> bool
        {
            nau::Ptr<ICameraControl> camera = cameraWeakPtr.lock();
            if (camera)
            {
                cameras.emplace_back(rtti::createInstance<ReadonlyCamera>(camera));
            }

            return !static_cast<bool>(camera);
        });

        m_sceneCameras.remove_if([&cameras](ObjectWeakRef<ICameraControl>& sceneCameraRef) -> bool
        {
            if (sceneCameraRef)
            {
                cameras.emplace_back(rtti::createInstance<ReadonlyCamera>(sceneCameraRef));
            }

            return !static_cast<bool>(sceneCameraRef);
        });

        return cameras;
    }

    void CameraManagerImpl::syncCameras(CameraCollection& cameras, SyncCameraCallback onCameraAdded, SyncCameraCallback onCameraRemoved)
    {
        NAU_FATAL(m_syncThreadId == std::this_thread::get_id(), "Camera synchronization can be performed only from main thread");
        lock_(m_mutex);

        cameras.reserve(m_detachedCameras.size() + m_sceneCameras.size());

        // sync existing camera properties and remove non existent
        eastl::erase_if(cameras, [&onCameraRemoved](nau::Ptr<ICameraProperties>& camera) -> bool
        {
            NAU_FATAL(camera);
            const bool syncOk = camera->as<ReadonlyCamera&>().syncCameraProperties();
            if (!syncOk && onCameraRemoved)
            {
                onCameraRemoved(*camera);
            }

            return !syncOk;
        });

        const auto containsCamera = [&cameras](const Uid cameraUid) -> bool
        {
            return eastl::any_of(cameras.begin(), cameras.end(), [cameraUid](const auto& camera)
            {
                return camera->getCameraUid() == cameraUid;
            });
        };

        // iterate over detached cameras and add that not exists in cameras collection:
        m_detachedCameras.remove_if([&](nau::WeakPtr<ICameraControl>& cameraWeakPtr) -> bool
        {
            nau::Ptr<ICameraControl> camera = cameraWeakPtr.lock();
            if (camera && !containsCamera(camera->getCameraUid()))
            {
                auto& newCamera = cameras.emplace_back(rtti::createInstance<ReadonlyCamera>(camera));
                if (onCameraAdded)
                {
                    onCameraAdded(*newCamera);
                }
            }

            return !static_cast<bool>(camera);
        });

        // iterate over scene cameras and add that not exists in cameras collection:
        m_sceneCameras.remove_if([&](ObjectWeakRef<ICameraControl>& sceneCameraRef) -> bool
        {
            if (sceneCameraRef && !containsCamera(sceneCameraRef->getCameraUid()))
            {
                auto& newCamera = cameras.emplace_back(rtti::createInstance<ReadonlyCamera>(sceneCameraRef));
                if (onCameraAdded)
                {
                    onCameraAdded(*newCamera);
                }
            }

            return !static_cast<bool>(sceneCameraRef);
        });
    }

    nau::Ptr<ICameraControl> CameraManagerImpl::createDetachedCamera(Uid worldUid)
    {
        lock_(m_mutex);

        if (worldUid == NullUid)
        {
            // getting Uid for the default world is thread safe operation
            worldUid = getServiceProvider().get<ISceneManager>().getDefaultWorld().getUid();
        }

        auto camera = rtti::createInstance<DetachedCamera>(worldUid);
        m_detachedCameras.emplace_back(camera);
        return camera;
    }

    Result<> CameraManagerImpl::activateComponents([[maybe_unused]] Uid worldUid, eastl::span<Component*> components)
    {
        NAU_ASSERT(m_syncThreadId == std::this_thread::get_id());

        for (Component* const component : components)
        {
            CameraComponent* const cameraComponent = component->as<CameraComponent*>();
            if (!cameraComponent)
            {
                continue;
            }

            lock_(m_mutex);
            m_sceneCameras.emplace_back(*cameraComponent);
        }

        return ResultSuccess;
    }

    void CameraManagerImpl::deactivateComponents([[maybe_unused]] Uid worldUid, eastl::span<Component*> components)
    {
        NAU_ASSERT(m_syncThreadId == std::this_thread::get_id());
    }

}  // namespace nau::scene
