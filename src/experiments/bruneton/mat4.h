/**
 * Precomputed Atmospheric Scattering
 * Copyright (c) 2008 INRIA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MAT4_H_
#define MAT4_H_

#include "vec3.h"

#ifndef M_PI
#define M_PI 3.141592657
#endif

/**
 * A 4x4 matrix.
 */
template <typename type> class mat4
{
protected:
    union {
        type m[4][4];
        type _m[16];
    };

public:
    /**
     * Creates a new, uninitialized matrix.
     */
    mat4();

    /**
     * Creates a new matrix with the given components. The first index is the
     * row index, the second one is the column index.
     */
    mat4(type m00, type m01, type m02, type m03,
                type m10, type m11, type m12, type m13,
                type m20, type m21, type m22, type m23,
                type m30, type m31, type m32, type m33);

    /**
     * Returns the coefficients of this matrix.
     */
    const type* coefficients() const;

    /**
     * Returns the row of this matrix whose index is given.
     */
    const type* operator[](int iRow) const;

    /**
     * Returns true is this matrix is equal to the given matrix.
     */
    bool operator==(const mat4& m2) const;

    /**
     * Returns true is this matrix is different from the given matrix.
     */
    bool operator!=(const mat4& m2) const;

    /**
     * Returns the sum of this matrix and of the given matrix.
     */
    mat4 operator+(const mat4& m2) const;

    /**
     * Returns the difference of this matrix and of the given matrix.
     */
    mat4 operator-(const mat4& m2) const;

    /**
     * Returns the product of this matrix and of the given matrix.
     */
    mat4 operator*(const mat4& m2) const;

    /**
     * Returns the product of this matrix and of the given vector. The given
     * vector w coordinate is set to 1, and the 4 vector result is converted
     * to a 3 vector by dividing its xyz components by its w component.
     */
    vec3<type> operator*(const vec3<type>& v) const;

    /**
     * Returns the product of this matrix and of the given scalar.
     */
    mat4 operator*(type f) const;

    /**
     * Returns the transpose of this matrix.
     */
    mat4 transpose(void) const;

    /**
     * Returns the adjoint of this matrix.
     */
    mat4 adjoint() const;

    /**
     * Returns the inverse of this matrix.
     */
    mat4 inverse() const;

    /**
     * Returns the determinant of this matrix.
     */
    float determinant() const;

    /**
     * Returns the translation matrix corresponding to the given translation
     * vector.
     */
    static mat4 translate(const vec3<type>& v);

    /**
     * Returns the perspective projection matrix corresponding to the given
     * projection parameters.
     *
     * @param fovy vertical field of view in degrees.
     * @param aspect aspect ratio of the projection window.
     * @param zNear near clipping plane.
     * @param zFar far clipping plane.
     */
    static mat4 perspectiveProjection(type fovy, type aspect, type zNear, type zFar);
};

/**
 * A 4x4 matrix with float components.
 */
typedef mat4<float> mat4f;

/**
 * A 4x4 matrix with double components.
 */
typedef mat4<double> mat4d;

template <typename type>
inline mat4<type>::mat4()
{
}

template <typename type>
inline mat4<type>::mat4(type m00, type m01, type m02, type m03,
                        type m10, type m11, type m12, type m13,
                        type m20, type m21, type m22, type m23,
                        type m30, type m31, type m32, type m33)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;
    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}

template <typename type>
inline const type* mat4<type>::coefficients() const
{
    return _m;
}

template <typename type>
inline const type* mat4<type>::operator[](int iRow) const
{
    //assert(iRow < 4);
    return m[iRow];
}

template <typename type>
inline bool mat4<type>::operator==(const mat4<type>& m2) const
{
    if (
        m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
        m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
        m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
        m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
        return false;

    return true;
}

template <typename type>
inline bool mat4<type>::operator!=(const mat4<type>& m2) const
{
    if (
        m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
        m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
        m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
        m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
        return true;

    return false;
}

template <typename type>
inline mat4<type> mat4<type>::operator+(const mat4<type>& m2) const
{
    mat4<type> r;

    r.m[0][0] = m[0][0] + m2.m[0][0];
    r.m[0][1] = m[0][1] + m2.m[0][1];
    r.m[0][2] = m[0][2] + m2.m[0][2];
    r.m[0][3] = m[0][3] + m2.m[0][3];

    r.m[1][0] = m[1][0] + m2.m[1][0];
    r.m[1][1] = m[1][1] + m2.m[1][1];
    r.m[1][2] = m[1][2] + m2.m[1][2];
    r.m[1][3] = m[1][3] + m2.m[1][3];

    r.m[2][0] = m[2][0] + m2.m[2][0];
    r.m[2][1] = m[2][1] + m2.m[2][1];
    r.m[2][2] = m[2][2] + m2.m[2][2];
    r.m[2][3] = m[2][3] + m2.m[2][3];

    r.m[3][0] = m[3][0] + m2.m[3][0];
    r.m[3][1] = m[3][1] + m2.m[3][1];
    r.m[3][2] = m[3][2] + m2.m[3][2];
    r.m[3][3] = m[3][3] + m2.m[3][3];

    return r;
}

template <typename type>
inline mat4<type> mat4<type>::operator-(const mat4<type>& m2) const
{
    mat4 r;
    r.m[0][0] = m[0][0] - m2.m[0][0];
    r.m[0][1] = m[0][1] - m2.m[0][1];
    r.m[0][2] = m[0][2] - m2.m[0][2];
    r.m[0][3] = m[0][3] - m2.m[0][3];

    r.m[1][0] = m[1][0] - m2.m[1][0];
    r.m[1][1] = m[1][1] - m2.m[1][1];
    r.m[1][2] = m[1][2] - m2.m[1][2];
    r.m[1][3] = m[1][3] - m2.m[1][3];

    r.m[2][0] = m[2][0] - m2.m[2][0];
    r.m[2][1] = m[2][1] - m2.m[2][1];
    r.m[2][2] = m[2][2] - m2.m[2][2];
    r.m[2][3] = m[2][3] - m2.m[2][3];

    r.m[3][0] = m[3][0] - m2.m[3][0];
    r.m[3][1] = m[3][1] - m2.m[3][1];
    r.m[3][2] = m[3][2] - m2.m[3][2];
    r.m[3][3] = m[3][3] - m2.m[3][3];

    return r;
}

template <typename type>
inline mat4<type> mat4<type>::operator*(const mat4<type>& m2) const
{
    mat4 r;
    r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
    r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
    r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
    r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

    r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
    r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
    r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
    r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

    r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
    r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
    r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
    r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

    r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
    r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
    r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
    r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

    return r;
}

template <typename type>
inline vec3<type> mat4<type>::operator*(const vec3<type>& v) const
{
    vec3<type> r;

    float fInvW = 1.0 / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);

    r.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * fInvW;
    r.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * fInvW;
    r.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * fInvW;

    return r;
}

template <typename type>
inline mat4<type> mat4<type>::operator*(type f) const
{
    mat4<type> r;

    r.m[0][0] = m[0][0] * f;
    r.m[0][1] = m[0][1] * f;
    r.m[0][2] = m[0][2] * f;
    r.m[0][3] = m[0][3] * f;

    r.m[1][0] = m[1][0] * f;
    r.m[1][1] = m[1][1] * f;
    r.m[1][2] = m[1][2] * f;
    r.m[1][3] = m[1][3] * f;

    r.m[2][0] = m[2][0] * f;
    r.m[2][1] = m[2][1] * f;
    r.m[2][2] = m[2][2] * f;
    r.m[2][3] = m[2][3] * f;

    r.m[3][0] = m[3][0] * f;
    r.m[3][1] = m[3][1] * f;
    r.m[3][2] = m[3][2] * f;
    r.m[3][3] = m[3][3] * f;

    return r;
}

template <typename type>
inline mat4<type> mat4<type>::transpose(void) const
{
    return mat4(m[0][0], m[1][0], m[2][0], m[3][0],
                m[0][1], m[1][1], m[2][1], m[3][1],
                m[0][2], m[1][2], m[2][2], m[3][2],
                m[0][3], m[1][3], m[2][3], m[3][3]);
}

template <typename type>
inline static float
MINOR(const mat4<type>& m, int r0, int r1, int r2, int c0, int c1, int c2)
{
    return m[r0][c0] *(m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
           m[r0][c1] *(m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
           m[r0][c2] *(m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
}

template <typename type>
mat4<type> mat4<type>::adjoint() const
{
    return mat4(MINOR(*this, 1, 2, 3, 1, 2, 3),
                -MINOR(*this, 0, 2, 3, 1, 2, 3),
                MINOR(*this, 0, 1, 3, 1, 2, 3),
                -MINOR(*this, 0, 1, 2, 1, 2, 3),

                -MINOR(*this, 1, 2, 3, 0, 2, 3),
                MINOR(*this, 0, 2, 3, 0, 2, 3),
                -MINOR(*this, 0, 1, 3, 0, 2, 3),
                MINOR(*this, 0, 1, 2, 0, 2, 3),

                MINOR(*this, 1, 2, 3, 0, 1, 3),
                -MINOR(*this, 0, 2, 3, 0, 1, 3),
                MINOR(*this, 0, 1, 3, 0, 1, 3),
                -MINOR(*this, 0, 1, 2, 0, 1, 3),

                -MINOR(*this, 1, 2, 3, 0, 1, 2),
                MINOR(*this, 0, 2, 3, 0, 1, 2),
                -MINOR(*this, 0, 1, 3, 0, 1, 2),
                MINOR(*this, 0, 1, 2, 0, 1, 2));
}

template <typename type>
mat4<type> mat4<type>::inverse() const
{
    return adjoint() * (1.0f / determinant());
}

template <typename type>
float mat4<type>::determinant() const
{
    return m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
           m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
           m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
           m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
}

template <typename type>
inline mat4<type> mat4<type>::translate(const vec3<type> &v)
{
    return mat4<type>(1, 0, 0, v.x,
                      0, 1, 0, v.y,
                      0, 0, 1, v.z,
                      0, 0, 0, 1);
}

template <typename type>
inline mat4<type> mat4<type>::perspectiveProjection(type fovy, type aspect, type zNear, type zFar)
{
    type f = (type) 1 / tan(fovy * M_PI / 180.0 / 2);
    return mat4<type>(f / aspect, 0, 0,                         0,
                      0,        f, 0,                         0,
                      0,        0, (zFar + zNear) / (zNear - zFar), (2*zFar*zNear) / (zNear - zFar),
                      0,        0, -1,                        0);
}

#endif /*MAT4_H_*/
