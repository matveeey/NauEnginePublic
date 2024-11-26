// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.

#include "vfx_component.h"
#include "nau/vfx_manager.h"
#include "../scene/scene_manager.h"

namespace nau::vfx
{
    NAU_IMPLEMENT_DYNAMIC_OBJECT(VFXComponent)

    VFXComponent::~VFXComponent()
    {
        if (m_isVFXInScene)
        {
            getServiceProvider().get<VFXManager>().removeInstance(m_vfxInstance);
            m_isVFXInScene = false;
        }
    }

    nau::async::Task<> VFXComponent::activateComponentAsync()
    {
        //const bool paused = getServiceProvider().get<scene::ISceneManager>().getDefaultWorld().isSimulationPaused();
        //if (paused)
        //{
        //    co_return;
        //}

        if (!m_vfxInstance && getServiceProvider().has<VFXManager>())
        {
            MaterialAssetView::Ptr defaultMaterial = co_await defaultMaterialRef.getAssetViewTyped<MaterialAssetView>();
            m_vfxInstance = std::static_pointer_cast<modfx::VFXModFXInstance>(getServiceProvider().get<VFXManager>().addInstance(defaultMaterial));
            m_isVFXInScene = true;

            TextureAssetRef assetRef = AssetPath{"file:/content/textures/default.jpg"};
            auto texAsset = co_await assetRef.getReloadableAssetViewTyped<TextureAssetView>();
            m_vfxInstance->setTexture(texAsset);

            nau::DataBlock blk;
            if (blk.load(m_assetPath.c_str()))
                m_vfxInstance->deserialize(&blk);
        }
    }

    void VFXComponent::deactivateComponent()
    {
        if (m_isVFXInScene)
        {
            getServiceProvider().get<vfx::VFXManager>().removeInstance(m_vfxInstance);
            m_isVFXInScene = false;
        }
    }

    void VFXComponent::updateComponent(float dt)
    {
        const bool paused = getServiceProvider().get<scene::ISceneManager>().getDefaultWorld().isSimulationPaused();
        if (paused)
        {
            return;
        }
    }

    void VFXComponent::setAssetPath(const eastl::string& assetPath)
    {
        m_assetPath = assetPath;
        
        if (!m_vfxInstance)
            return;

        nau::DataBlock blk;
        if (blk.load(m_assetPath.c_str()))
            m_vfxInstance->deserialize(&blk);
    }

    void VFXComponent::forceUpdateTexture(ReloadableAssetView::Ptr texture)
    {
        m_vfxInstance->setTexture(texture);
    }

    void VFXComponent::forceBLKUpdate()
    {
        if (!m_vfxInstance || m_assetPath.empty())
            return;

        nau::DataBlock blk;
        if (blk.load(m_assetPath.c_str()))
            m_vfxInstance->deserialize(&blk);

        auto pos = getWorldTransform().getMatrix().getTranslation();
        m_vfxInstance->setTransform(getWorldTransform().getMatrix());
    }
}  // namespace nau::physics
