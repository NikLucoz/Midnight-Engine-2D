#pragma once

#include "engine/components/Component.h"
#include "engine/utils/math/Vector2.h"

struct CRenderable : public Component {
    
    Vec2f localScale_ = {1.0f, 1.0f};
    bool visible_ = true;
    int renderLayer_ = 0;
    int orderInLayer = 0;

public:
    CRenderable(const Vec2f& localScale = {1.0f, 1.0f}, int renderLayer = 0, int orderInLayer = 0)
        : localScale_(localScale), renderLayer_(renderLayer), orderInLayer(orderInLayer) {}

    void setLocalScale(const Vec2f& scale) { localScale_ = scale; }
    const Vec2f& getLocalScale() const { return localScale_; }

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

    int getRenderLayer() const { return renderLayer_; }
    void setRenderLayer(int renderLayer) { this->renderLayer_ = renderLayer; }
    int getOrderInLayer() const { return orderInLayer; }
    void setOrderInLayer(int orderInLayer) { this->orderInLayer = orderInLayer; }
};