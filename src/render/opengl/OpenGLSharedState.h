#ifndef OPENGLSHAREDSTATE_H
#define OPENGLSHAREDSTATE_H

#include "opengl_global.h"

#include <map>
#include "OpenGLVariable.h"

namespace paysages {
namespace opengl {

/*!
 * \brief OpenGL variables that can be shared between shaders.
 */
class OPENGLSHARED_EXPORT OpenGLSharedState
{
public:
    OpenGLSharedState();

    /*!
     * \brief Apply the stored variables to the bound program.
     */
    void apply(OpenGLShaderProgram* program, int &texture_unit);

    /*!
     * \brief Get or create a variable in the state.
     */
    OpenGLVariable *get(const std::string &name);

    // Shortcuts
    inline void set(const std::string &name, const Texture2D *texture) {get(name)->set(texture);}
    inline void set(const std::string &name, const Texture3D *texture) {get(name)->set(texture);}
    inline void set(const std::string &name, const Texture4D *texture) {get(name)->set(texture);}
    inline void set(const std::string &name, float value) {get(name)->set(value);}
    inline void set(const std::string &name, const Vector3 &vector) {get(name)->set(vector);}
    inline void set(const std::string &name, const QVector3D &vector) {get(name)->set(vector);}
    inline void set(const std::string &name, const Matrix4 &matrix) {get(name)->set(matrix);}
    inline void set(const std::string &name, const QMatrix4x4 &matrix) {get(name)->set(matrix);}
    inline void set(const std::string &name, const Color &color) {get(name)->set(color);}

private:
    std::map<std::string, OpenGLVariable*> variables;
};

}
}

#endif // OPENGLSHAREDSTATE_H
