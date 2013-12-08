#ifndef CLOUDSRENDERER_H
#define CLOUDSRENDERER_H

#include "software_global.h"

namespace paysages {
namespace software {

/*!
 * \brief Software renderer of a group of cloud layers.
 */
class SOFTWARESHARED_EXPORT CloudsRenderer
{
public:
    CloudsRenderer(SoftwareRenderer* parent);
    virtual ~CloudsRenderer();

    /*!
     * \brief Update the renderer with the bound scenery.
     *
     * Don't call this if another thread is currently using this renderer.
     */
    void update();

    /*!
     * \brief Get the layer renderer for a given layer.
     *
     * The returned renderer is managed by this object and should not be deleted.
     */
    virtual BaseCloudLayerRenderer* getLayerRenderer(unsigned int layer);

    /*!
     * \brief Get the cloud model for a given layer.
     *
     * The returned model is managed by this object and should not be deleted.
     */
    virtual BaseCloudsModel* getLayerModel(unsigned int layer);

    /*!
     * \brief Get the composited color, as applied on a base color and location.
     */
    virtual Color getColor(const Vector3 &eye, const Vector3 &location, const Color &base);

    /*!
     * \brief Alter a light, as if passed through all layers.
     *
     * Return true if the light was altered.
     */
    virtual bool alterLight(LightComponent* light, const Vector3 &eye, const Vector3 &location);
private:
    SoftwareRenderer* parent;

    std::vector<BaseCloudLayerRenderer*> layer_renderers;
    BaseCloudLayerRenderer* fake_renderer;

    std::vector<BaseCloudsModel*> layer_models;
    BaseCloudsModel* fake_model;
};

}
}

#endif // CLOUDSRENDERER_H
