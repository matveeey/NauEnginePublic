// Copyright 2024 N-GINN LLC. All rights reserved.
// Use of this source code is governed by a BSD-3 Clause license that can be found in the LICENSE file.


#include "render_pipeline/billboards_manager.h"
#include "graphics_impl.h"

namespace nau
{
    BillboardsManager::BillboardsManager(nau::Ptr<nau::MaterialAssetView> material)
    {
        m_billboardMaterial = material;
    }


    nau::async::Task<eastl::shared_ptr<nau::BillboardHandle>> BillboardsManager::addBillboard(
        ReloadableAssetView::Ptr texture,
        nau::math::Vector3 position, 
        nau::Uid uid,
        float screenPercentageSize)
    {
        auto& graphics = getServiceProvider().get<GraphicsImpl>();
        ASYNC_SWITCH_EXECUTOR(graphics.getPreRenderExecutor());

        eastl::shared_ptr<nau::BillboardHandle> billboard = eastl::make_shared<nau::BillboardHandle>();
        billboard->setWorldPos(position);
        billboard->setTexture(texture);
        billboard->setScreenPercentageSize(screenPercentageSize);
        billboard->setUid(uid);
        
        m_billboards.emplace_back(billboard->m_billboard);

        co_return billboard;
    }

    void BillboardsManager::render(nau::math::Matrix4 viewProj)
    {
        if (m_billboards.size() == 0)
        {
            return;
        }

        NAU_ASSERT(m_billboardMaterial);

        nau::shader_globals::setVariable("vp", &viewProj);

        d3d::setvsrc(0, nullptr, 0);
        d3d::setind(nullptr);

        int posx, posy, width, height;
        float minz, maxz;
        d3d::getview(posx, posy, width, height, minz, maxz);
        float aspectRatio = 1.0f;
        if (height != 0)
        {
            aspectRatio = width / static_cast<float>(height);
        }

        m_billboardMaterial->setProperty("default", "aspectRatio", aspectRatio);

        for (const auto billboard : m_billboards)
        {
            if (const auto bill = billboard.lock())
            {
                NAU_ASSERT(bill->texture);
                nau::Ptr<TextureAssetView> textureView;
                bill->texture->getTyped<TextureAssetView>(textureView);
                NAU_ASSERT(textureView->getTexture());

                math::IVector4 vuid = {};
                memcpy(&vuid, &bill->uid, sizeof(vuid));

                m_billboardMaterial->setProperty("default", "uid", vuid);
                m_billboardMaterial->setProperty("default", "scPercentSize", bill->screenPercentageSize);
                m_billboardMaterial->setProperty("default", "worldPosition", bill->worldPosition);
                m_billboardMaterial->setTexture("default", "tex", textureView->getTexture());

                m_billboardMaterial->bind();

                d3d::draw(PRIM_TRISTRIP, 0, 2);
            }
            else
            {
                m_isBillboardsDirty = true;
            }
        }
    }

    RenderList::Ptr BillboardsManager::getRenderList(const nau::math::Vector3& viewerPosition,
        eastl::function<bool(const InstanceInfo&)>& filterFunc,
        eastl::function<bool(const nau::MaterialAssetView::Ptr)>& materialFilter)
    {
        NAU_FAILURE("Not implemented yet!");
        return RenderList::Ptr();
    }

    void BillboardsManager::update()
    {
        if (m_isBillboardsDirty)
        {
            eastl::erase_if(m_billboards, [](auto& ptr) {
                return ptr.expired();
            });

            m_isBillboardsDirty = false;
        }
    }

    BillboardHandle::BillboardHandle()
    {
        m_billboard = eastl::make_shared<BillboardsManager::BillboardInfo>();
    }

    bool BillboardHandle::isValid() const
    {
        return static_cast<bool>(m_billboard);
    }

    void BillboardHandle::setWorldPos(const nau::math::Vector3& position)
    {
        m_billboard->worldPosition = position;
    }

    nau::math::Vector3 BillboardHandle::getWorldPos()
    {
        return m_billboard->worldPosition;
    }

    void BillboardHandle::setScreenPercentageSize(float screenPercentageSize)
    {
        m_billboard->screenPercentageSize = screenPercentageSize;
    }

    float BillboardHandle::getScreenPercentageSize()
    {
        return m_billboard->screenPercentageSize;
    }

    void BillboardHandle::setTexture(ReloadableAssetView::Ptr texture)
    {
        m_billboard->texture = texture;
    }

    nau::Ptr<nau::TextureAssetView> BillboardHandle::getTexture()
    {
        nau::Ptr<TextureAssetView> textureView;
        m_billboard->texture->getTyped<TextureAssetView>(textureView);
        return textureView;
    }

    void BillboardHandle::setVisibility(bool isVisible)
    {
        m_billboard->isVisible = isVisible;
    }

    bool BillboardHandle::getVisibility() const
    {
        return m_billboard->isVisible;
    }

    void BillboardHandle::setUid(const nau::Uid& uid)
    {
        m_billboard->uid = uid;
    }

    nau::Uid BillboardHandle::getUid() const
    {
        return m_billboard->uid;
    }

    BillboardHandle::~BillboardHandle()
    {
        m_billboard.reset();
    }

} //namespace nau
