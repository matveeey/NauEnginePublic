// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.

#pragma once

#include "nau/meta/class_info.h"
#include "nau/rtti/ptr.h"
#include "nau/scene/components/component_life_cycle.h"
#include "nau/scene/components/scene_component.h"
#include "../vfx_mod_fx_instance.h"
#include "nau/assets/reloadable_asset_view.h"

namespace nau::vfx
{
    class NAU_VFX_EXPORT VFXComponent : public scene::SceneComponent,
                                        public scene::IComponentUpdate,
                                        public scene::IComponentActivation
    {
        NAU_OBJECT(nau::vfx::VFXComponent,
                   scene::SceneComponent,
                   scene::IComponentUpdate,
                   scene::IComponentActivation
        )
        NAU_DECLARE_DYNAMIC_OBJECT

        NAU_CLASS_FIELDS(
            CLASS_NAMED_FIELD(m_assetPath, "vfxAssetPath"),
       )

    public:
        VFXComponent() = default;
        ~VFXComponent();

    public:
        virtual async::Task<> activateComponentAsync() override;
        virtual void deactivateComponent() override;

        virtual void updateComponent(float dt) override;

    public:
        void setAssetPath(const eastl::string& assetPath);
        
        void forceUpdateTexture(ReloadableAssetView::Ptr texture);
        void forceBLKUpdate();

    private:
        std::shared_ptr<nau::vfx::modfx::VFXModFXInstance> m_vfxInstance;
        bool m_isVFXInScene = false;

        nau::MaterialAssetRef defaultMaterialRef{"file:/res/materials/vfx.nmat_json"};

        eastl::string m_assetPath;
    };
}  // namespace nau::vfx