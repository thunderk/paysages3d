#ifndef CLOUDSRENDERER_H
#define CLOUDSRENDERER_H

#include "software_global.h"

#include "LightFilter.h"

namespace paysages {
namespace software {

/*!
 * \brief Software renderer of a group of cloud layers.
 */
class SOFTWARESHARED_EXPORT CloudsRenderer: public LightFilter
{
public:
    CloudsRenderer(SoftwareRenderer* parent);
    virtual ~CloudsRenderer();

    /**
     * Set the quality factor (0.0-1.0) for clouds rendering.
     */
    void setQuality(double factor);

    /**
     * Enable or disable the whole cloud rendering.
     */
    void setEnabled(bool enabled);

    /*!
     * \brief Update the renderer with the bound scenery.
     *
     * Don't call this if another thread is currently using this renderer.
     */
    virtual void update();

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
     * \brief Override de default density model for a given layer.
     *
     * This must be called after each update().
     * Ownership of the model is taken.
     */
    virtual void setLayerModel(unsigned int layer, BaseCloudsModel* model, bool delete_old=true);

    /*!
     * \brief Get the composited color, as applied on a base color and location.
     */
    virtual Color getColor(const Vector3 &eye, const Vector3 &location, const Color &base);

    /*!
     * \brief Alter a light, as if passed through all layers.
     *
     * Return true if the light was altered.
     */
    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override;
private:
    double quality;

    bool enabled;
    SoftwareRenderer* parent;

    std::vector<BaseCloudLayerRenderer*> layer_renderers;
    BaseCloudLayerRenderer* fake_renderer;

    std::vector<BaseCloudsModel*> layer_models;
    BaseCloudsModel* fake_model;
};

}
}

#endif // CLOUDSRENDERER_H
