/************************************************************************
*                                                                       *
*               Copyright (C) 2002-2005  3Dlabs Inc. Ltd.               *
*                                                                       *
*                        All rights reserved.                           *
*                                                                       *
* Redistribution and use in source and binary forms, with or without    *
* modification, are permitted provided that the following conditions    *
* are met:                                                              *
*                                                                       *
*     Redistributions of source code must retain the above copyright    *
*     notice, this list of conditions and the following disclaimer.     *
*                                                                       *
*     Redistributions in binary form must reproduce the above           *
*     copyright notice, this list of conditions and the following       *
*     disclaimer in the documentation and/or other materials provided   *
*     with the distribution.                                            *
*                                                                       *
*     Neither the name of 3Dlabs Inc. Ltd. nor the names of its         *
*     contributors may be used to endorse or promote products derived   *
*     from this software without specific prior written permission.     *
*                                                                       *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
* COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, *
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                           *
*                                                                       *
************************************************************************/

#include "SGSurfaces.h"
#include "SGFrame.h"
#define _USE_MATH_DEFINES
#include "UtilityFunctions.h"
#include <cmath>

static const float pi = 3.14159265358979323846f;
static const float twopi = 6.28318530717958647692f;

TParametricSurface::TParametricSurface()
  : slices(100)
{
}

TParametricSurface::~TParametricSurface()
{
}

// Draw a parametric surface.  'slices' is the tesselation factor.  Returns the
// number of vertices.
int
TParametricSurface::Draw()
{
    int totalVerts = 0;
    int stacks = slices / 2;
    du = 1.0f / (float)(slices - 1);
    dv = 1.0f / (float)(stacks - 1);

    bool isNormalize = glIsEnabled(GL_NORMALIZE);

    for (int i = 0; i < slices; i++) {
        float u = i * du;
        glBegin(GL_QUAD_STRIP);
        flipped = flip(QVector2D(u, 0));

        for (int j = 0; j < stacks; j++) {
            float v = j * dv;
            QVector3D normal, p0;
            QVector2D domain = flipped ? QVector2D(u + du, v) : QVector2D(u, v);
            vertex(domain, normal, p0, isNormalize);
            if (isNormalize)
                glNormal(normal);
            for (int i = 0; i < 5; i++) {
                glMultiTexCoord(domain, GL_TEXTURE0 + i);
            }
            glVertex(p0);
            domain = flipped ? QVector2D(u, v) : QVector2D(u + du, v);
            vertex(domain, normal, p0, isNormalize);
            if (isNormalize)
                glNormal(normal);
            for (int i = 0; i < 5; i++) {
                glMultiTexCoord(domain, GL_TEXTURE0 + i);
            }

            glVertex(p0);
            totalVerts += 2;
        }
        glEnd();
    }

    PrintOpenGLError();
    return totalVerts;
}

// Send out a normal, texture coordinate, vertex coordinate, and an optional
// custom attribute.
void
TParametricSurface::vertex(QVector2D& domain, QVector3D& normal, QVector3D& p0, bool isNormalize)
{
    QVector3D p1, p2, p3;
    float u = domain.x();
    float v = domain.y();

    eval(domain, p0);
    QVector2D z1(u + du / 2, v);
    eval(z1, p1);
    QVector2D z2(u + du / 2 + du, v);
    eval(z2, p3);

    if (flipped) {
        QVector2D z3(u + du / 2, v - dv);
        eval(z3, p2);
    } else {
        QVector2D z4(u + du / 2, v + dv);
        eval(z4, p2);
    }
    if (isNormalize) {
        normal = QVector3D::crossProduct(p3 - p1, p2 - p1);
        if (normal.length() < 0.00001f) {
            normal = p0;
        }
        normal.normalize();
    }
}

void
TKlein::eval(QVector2D& domain, QVector3D& range)
{
    float u = (1 - domain.x()) * twopi;
    float v = domain.y() * twopi;

    float x0 = 3 * cosf(u) * (1 + sinf(u)) + (2 * (1 - cosf(u) / 2)) * cosf(u) * cosf(v);
    float y0 = 8 * sinf(u) + (2 * (1 - cosf(u) / 2)) * sinf(u) * cosf(v);

    float x1 = 3 * cosf(u) * (1 + sinf(u)) + (2 * (1 - cosf(u) / 2)) * cosf(v + pi);
    float y1 = 8 * sinf(u);

    range.setX(u < pi ? x0 : x1);
    range.setY(u < pi ? y0 : y1);
    range.setZ((2 * (1 - cosf(u) / 2)) * sinf(v));
    range = range / 10;
    range.setY(-range.y());

    // Tweak the texture coordinates.
    domain.setX(domain.x() * 4);
}

// Flip the normals along a segment of the Klein bottle so that we don't need
// two-sided lighting.
bool
TKlein::flip(const QVector2D& domain)
{
    return (domain.x() < .125);
}

void
TTrefoil::eval(QVector2D& domain, QVector3D& range)
{
    const float a = 0.5f;
    const float b = 0.3f;
    const float c = 0.5f;
    const float d = 0.1f;
    float u = (1 - domain.x()) * twopi * 2;
    float v = domain.y() * twopi;

    float r = a + b * cosf(1.5f * u);
    float x = r * cosf(u);
    float y = r * sinf(u);
    float z = c * sinf(1.5f * u);

    QVector3D dv;
    dv.setX(-1.5f * b * sinf(1.5f * u) * cosf(u) - (a + b * cosf(1.5f * u)) * sinf(u));
    dv.setY(-1.5f * b * sinf(1.5f * u) * sinf(u) + (a + b * cosf(1.5f * u)) * cosf(u));
    dv.setZ(1.5f * c * cosf(1.5f * u));

    QVector3D q = dv;
    q.normalize();
    QVector3D qvn(q.y(), -q.x(), 0);
    qvn.normalize();
    QVector3D ww = QVector3D::crossProduct(q, qvn);

    range.setX(x + d * (qvn.x() * cosf(v) + ww.x() * sinf(v)));
    range.setY(y + d * (qvn.y() * cosf(v) + ww.y() * sinf(v)));
    range.setZ(z + d * ww.z() * sinf(v));

    // Tweak the texture coordinates.
    domain.setX(domain.x() * 20);
    domain /= 3;
}

void
TConic::eval(QVector2D& domain, QVector3D& range)
{
    const float a = 0.2f;
    const float b = 1.5f;
    const float c = 0.1f;
    const float n = 2;

    float u = domain.x() * twopi;
    float v = domain.y() * twopi;

    range.setX(a * (1 - v / twopi) * cosf(n * v) * (1 + cosf(u)) + c * cosf(n * v));
    range.setZ(a * (1 - v / twopi) * sinf(n * v) * (1 + cosf(u)) + c * sinf(n * v));
    range.setY(b * v / twopi + a * (1 - v / twopi) * sinf(u) - 0.7f);
    range *= 1.25;
    range.setY(range.y() + 0.125);

    // Tweak the texture coordinates.
    domain.setY(domain.y() * 4);
}

void
TTorus::eval(QVector2D& domain, QVector3D& range)
{
    const float major = 0.8f;
    const float minor = 0.2f;
    float u = domain.x() * twopi;
    float v = domain.y() * twopi;

    range.setX((major + minor * cosf(v)) * cosf(u));
    range.setY((major + minor * cosf(v)) * sinf(u));
    range.setZ(minor * sinf(v));

    // Tweak the texture coordinates.
    domain.setX(domain.x() * 4);
}

void
TSphere::eval(QVector2D& domain, QVector3D& range)
{
    const float radius = 1;
    float u = fabsf(domain.y() * pi);
    float v = fabsf(domain.x() * twopi);

    range.setX(radius * cosf(v) * sinf(u));
    range.setZ(radius * sinf(v) * sinf(u));
    range.setY(radius * cosf(u));

    domain.setY(1 - domain.y());
    domain.setX(1 - domain.x());
}

void
TPlane::eval(QVector2D& domain, QVector3D& range)
{
    if (z < 0) {
        range.setX(-width * (domain.x() - 0.5f));
        range.setY(width * (domain.y() - 0.5f));
    } else {
        range.setX(width * (domain.x() - 0.5f));
        range.setY(width * (domain.y() - 0.5f));
    }
    range.setZ(z);
}
