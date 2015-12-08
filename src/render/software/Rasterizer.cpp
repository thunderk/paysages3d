#include "Rasterizer.h"

#include <cmath>
#include "SoftwareRenderer.h"
#include "CameraDefinition.h"
#include "CanvasPortion.h"
#include "CanvasFragment.h"
#include "Vector3.h"
#include "RenderProgress.h"

struct paysages::software::ScanPoint {
    int x;
    int y;
    struct {
        double x;
        double y;
        double z;
    } pixel;
    struct {
        double x;
        double y;
        double z;
    } location;
    int client;
    bool front_facing;
};

struct paysages::software::RenderScanlines {
    ScanPoint *up;
    ScanPoint *down;
    int left;
    int right;
};

Rasterizer::Rasterizer(SoftwareRenderer *renderer, RenderProgress *progress, int client_id, const Color &color)
    : renderer(renderer), progress(progress), client_id(client_id) {
    this->color = new Color(color);

    interrupted = false;
    backface_culling = false;
    perspective_correction = true;
    triangle_count = 0;
    auto_cut_limit = 0.01;

    setQuality(0.5);
}

Rasterizer::~Rasterizer() {
    delete color;
}

void Rasterizer::interrupt() {
    interrupted = true;
}

void Rasterizer::setQuality(double quality) {
    this->perspective_correction = (quality > 0.4);
}

void Rasterizer::setColor(const Color &color) {
    *this->color = color;
}

void Rasterizer::setBackFaceCulling(bool cull) {
    this->backface_culling = cull;
}

void Rasterizer::setPerspectiveCorrection(bool active) {
    this->perspective_correction = active;
}

void Rasterizer::setAutoCutLimit(double limit) {
    this->auto_cut_limit = limit;
}

void Rasterizer::resetTriangleCount() {
    triangle_count = 0;
}

bool Rasterizer::pushProjectedTriangle(CanvasPortion *canvas, const Vector3 &pixel1, const Vector3 &pixel2,
                                       const Vector3 &pixel3, const Vector3 &location1, const Vector3 &location2,
                                       const Vector3 &location3) {
    ScanPoint point1, point2, point3;
    double limit_width = (double)(canvas->getWidth() - 1);
    double limit_height = (double)(canvas->getHeight() - 1);

    Vector3 canvas_offset(canvas->getXOffset(), canvas->getYOffset(), 0.0);
    Vector3 dpixel1 = pixel1.sub(canvas_offset);
    Vector3 dpixel2 = pixel2.sub(canvas_offset);
    Vector3 dpixel3 = pixel3.sub(canvas_offset);

    double limit_near = renderer->render_camera->getPerspective().znear;
    if ((dpixel1.z < limit_near && dpixel2.z < limit_near && dpixel3.z < limit_near) ||
        (dpixel1.x < 0.0 && dpixel2.x < 0.0 && dpixel3.x < 0.0) ||
        (dpixel1.y < 0.0 && dpixel2.y < 0.0 && dpixel3.y < 0.0) ||
        (dpixel1.x > limit_width && dpixel2.x > limit_width && dpixel3.x > limit_width) ||
        (dpixel1.y > limit_height && dpixel2.y > limit_height && dpixel3.y > limit_height)) {
        // Fully outside screen
        return false;
    } else if (dpixel1.z < limit_near || dpixel2.z < limit_near || dpixel3.z < limit_near) {
        // Intersects the near frustum plane, needs cutting
        // ... except if the triangle is already small
        return location1.sub(location2).getNorm() > auto_cut_limit &&
               location2.sub(location3).getNorm() > auto_cut_limit &&
               location3.sub(location1).getNorm() > auto_cut_limit;
    }

    // Check the poylgon's facing (front-face or back-face)
    Vector3 normal = dpixel2.sub(dpixel1).crossProduct(dpixel3.sub(dpixel1));
    bool front_facing = (normal.z >= 0.0);
    if (backface_culling and not front_facing) {
        return false;
    }

    // Prepare vertices
    point1.pixel.x = dpixel1.x;
    point1.pixel.y = dpixel1.y;
    point1.pixel.z = dpixel1.z;
    point1.location.x = location1.x;
    point1.location.y = location1.y;
    point1.location.z = location1.z;
    point1.client = client_id;
    point1.front_facing = front_facing;

    point2.pixel.x = dpixel2.x;
    point2.pixel.y = dpixel2.y;
    point2.pixel.z = dpixel2.z;
    point2.location.x = location2.x;
    point2.location.y = location2.y;
    point2.location.z = location2.z;
    point2.client = client_id;
    point2.front_facing = front_facing;

    point3.pixel.x = dpixel3.x;
    point3.pixel.y = dpixel3.y;
    point3.pixel.z = dpixel3.z;
    point3.location.x = location3.x;
    point3.location.y = location3.y;
    point3.location.z = location3.z;
    point3.client = client_id;
    point3.front_facing = front_facing;

    // Prepare scanlines
    // TODO Don't create scanlines for each triangles (one by thread is more appropriate)
    RenderScanlines scanlines;
    int width = canvas->getWidth();
    scanlines.left = width;
    scanlines.right = -1;
    scanlines.up = new ScanPoint[width];
    scanlines.down = new ScanPoint[width];

    // Render edges in scanlines
    pushScanLineEdge(canvas, &scanlines, &point1, &point2);
    pushScanLineEdge(canvas, &scanlines, &point2, &point3);
    pushScanLineEdge(canvas, &scanlines, &point3, &point1);

    // Commit scanlines to area
    renderScanLines(canvas, &scanlines);

    // Free scalines
    delete[] scanlines.up;
    delete[] scanlines.down;

    triangle_count++;
    return false;
}

void Rasterizer::pushTriangle(CanvasPortion *canvas, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) {
    Vector3 p1, p2, p3;

    p1 = getRenderer()->projectPoint(v1);
    p2 = getRenderer()->projectPoint(v2);
    p3 = getRenderer()->projectPoint(v3);

    if (pushProjectedTriangle(canvas, p1, p2, p3, v1, v2, v3)) {
        // Cutting needed
        Vector3 vm1 = v1.midPointTo(v2);
        Vector3 vm2 = v2.midPointTo(v3);
        Vector3 vm3 = v3.midPointTo(v1);
        pushTriangle(canvas, v1, vm1, vm3);
        pushTriangle(canvas, v2, vm1, vm2);
        pushTriangle(canvas, v3, vm3, vm2);
        pushTriangle(canvas, vm1, vm2, vm3);
    }
}

void Rasterizer::pushDisplacedTriangle(CanvasPortion *canvas, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3,
                                       const Vector3 &ov1, const Vector3 &ov2, const Vector3 &ov3) {
    Vector3 p1, p2, p3;

    // TODO v1, v2 and v3 are lost, but may be useful (avoid need to unproject)
    p1 = getRenderer()->projectPoint(v1);
    p2 = getRenderer()->projectPoint(v2);
    p3 = getRenderer()->projectPoint(v3);

    if (pushProjectedTriangle(canvas, p1, p2, p3, ov1, ov2, ov3)) {
        // Cutting needed
        Vector3 vm1 = v1.midPointTo(v2);
        Vector3 vm2 = v2.midPointTo(v3);
        Vector3 vm3 = v3.midPointTo(v1);
        Vector3 ovm1 = ov1.midPointTo(ov2);
        Vector3 ovm2 = ov2.midPointTo(ov3);
        Vector3 ovm3 = ov3.midPointTo(ov1);
        pushDisplacedTriangle(canvas, v1, vm1, vm3, ov1, ovm1, ovm3);
        pushDisplacedTriangle(canvas, v2, vm1, vm2, ov2, ovm1, ovm2);
        pushDisplacedTriangle(canvas, v3, vm3, vm2, ov3, ovm3, ovm2);
        pushDisplacedTriangle(canvas, vm1, vm2, vm3, ovm1, ovm2, ovm3);
    }
}

void Rasterizer::pushQuad(CanvasPortion *canvas, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3,
                          const Vector3 &v4) {
    pushTriangle(canvas, v2, v3, v1);
    pushTriangle(canvas, v4, v1, v3);
}

void Rasterizer::pushDisplacedQuad(CanvasPortion *canvas, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3,
                                   const Vector3 &v4, const Vector3 &ov1, const Vector3 &ov2, const Vector3 &ov3,
                                   const Vector3 &ov4) {
    pushDisplacedTriangle(canvas, v2, v3, v1, ov2, ov3, ov1);
    pushDisplacedTriangle(canvas, v4, v1, v3, ov4, ov1, ov3);
}

void Rasterizer::scanGetDiff(ScanPoint *v1, ScanPoint *v2, ScanPoint *result) {
    result->pixel.x = v2->pixel.x - v1->pixel.x;
    result->pixel.y = v2->pixel.y - v1->pixel.y;
    result->pixel.z = v2->pixel.z - v1->pixel.z;
    result->location.x = v2->location.x - v1->location.x;
    result->location.y = v2->location.y - v1->location.y;
    result->location.z = v2->location.z - v1->location.z;
    result->client = v1->client;
}

void Rasterizer::scanInterpolate(CameraDefinition *camera, ScanPoint *v1, ScanPoint *diff, double value,
                                 ScanPoint *result) {
    result->pixel.x = v1->pixel.x + diff->pixel.x * value;
    result->pixel.y = v1->pixel.y + diff->pixel.y * value;
    result->pixel.z = v1->pixel.z + diff->pixel.z * value;

    if (perspective_correction) {
        Vector3 vec1(v1->pixel.x, v1->pixel.y, v1->pixel.z);
        Vector3 vecdiff(diff->pixel.x, diff->pixel.y, diff->pixel.z);
        double v1depth = 1.0 / camera->getRealDepth(vec1);
        double v2depth = 1.0 / camera->getRealDepth(vec1.add(vecdiff));
        double factor = 1.0 / ((1.0 - value) * v1depth + value * v2depth);

        result->location.x =
            ((1.0 - value) * (v1->location.x * v1depth) + value * (v1->location.x + diff->location.x) * v2depth) *
            factor;
        result->location.y =
            ((1.0 - value) * (v1->location.y * v1depth) + value * (v1->location.y + diff->location.y) * v2depth) *
            factor;
        result->location.z =
            ((1.0 - value) * (v1->location.z * v1depth) + value * (v1->location.z + diff->location.z) * v2depth) *
            factor;
    } else {
        result->location.x = v1->location.x + diff->location.x * value;
        result->location.y = v1->location.y + diff->location.y * value;
        result->location.z = v1->location.z + diff->location.z * value;
    }

    result->client = v1->client;
    result->front_facing = v1->front_facing;
}

void Rasterizer::pushScanPoint(CanvasPortion *canvas, RenderScanlines *scanlines, ScanPoint *point) {
    point->x = (int)floor(point->pixel.x);
    point->y = (int)floor(point->pixel.y);

    if (point->x < 0 || point->x >= canvas->getWidth()) {
        // Point outside scanline range
        return;
    } else if (scanlines->right < 0) {
        // First point pushed
        scanlines->left = point->x;
        scanlines->right = point->x;
        scanlines->up[point->x] = *point;
        scanlines->down[point->x] = *point;
    } else if (point->x > scanlines->right) {
        // Grow scanlines to right
        for (int x = scanlines->right + 1; x < point->x; x++) {
            scanlines->up[x].y = -1;
            scanlines->down[x].y = canvas->getHeight();
        }
        scanlines->right = point->x;
        scanlines->up[point->x] = *point;
        scanlines->down[point->x] = *point;
    } else if (point->x < scanlines->left) {
        // Grow scanlines to left
        for (int x = point->x + 1; x < scanlines->left; x++) {
            scanlines->up[x].y = -1;
            scanlines->down[x].y = canvas->getHeight();
        }
        scanlines->left = point->x;
        scanlines->up[point->x] = *point;
        scanlines->down[point->x] = *point;
    } else {
        // Expand existing scanline
        if (point->y > scanlines->up[point->x].y) {
            scanlines->up[point->x] = *point;
        }
        if (point->y < scanlines->down[point->x].y) {
            scanlines->down[point->x] = *point;
        }
    }
}

void Rasterizer::pushScanLineEdge(CanvasPortion *canvas, RenderScanlines *scanlines, ScanPoint *point1,
                                  ScanPoint *point2) {
    double dx, fx;
    ScanPoint diff, point;
    int startx = lround(point1->pixel.x);
    int endx = lround(point2->pixel.x);
    int curx;

    if (endx < startx) {
        pushScanLineEdge(canvas, scanlines, point2, point1);
    } else if (endx < 0 || startx >= canvas->getWidth()) {
        return;
    } else if (startx == endx) {
        pushScanPoint(canvas, scanlines, point1);
        pushScanPoint(canvas, scanlines, point2);
    } else {
        if (startx < 0) {
            startx = 0;
        }
        if (endx >= canvas->getWidth()) {
            endx = canvas->getWidth() - 1;
        }

        dx = point2->pixel.x - point1->pixel.x;
        scanGetDiff(point1, point2, &diff);
        for (curx = startx; curx <= endx; curx++) {
            fx = (double)curx + 0.5;
            if (fx < point1->pixel.x) {
                fx = point1->pixel.x;
            } else if (fx > point2->pixel.x) {
                fx = point2->pixel.x;
            }
            fx = fx - point1->pixel.x;
            scanInterpolate(renderer->render_camera, point1, &diff, fx / dx, &point);

            /*point.pixel.x = (double)curx;*/

            pushScanPoint(canvas, scanlines, &point);
        }
    }
}

void Rasterizer::renderScanLines(CanvasPortion *canvas, RenderScanlines *scanlines) {
    int x, starty, endy, cury;
    ScanPoint diff;
    double dy, fy;
    ScanPoint up, down, current;

    if (scanlines->right > 0) {
        for (x = scanlines->left; x <= scanlines->right; x++) {
            up = scanlines->up[x];
            down = scanlines->down[x];

            starty = down.y;
            endy = up.y;

            if (endy < 0 || starty >= canvas->getHeight()) {
                continue;
            }

            if (starty < 0) {
                starty = 0;
            }
            if (endy >= canvas->getHeight()) {
                endy = canvas->getHeight() - 1;
            }

            dy = up.pixel.y - down.pixel.y;
            scanGetDiff(&down, &up, &diff);

            current.x = x;
            for (cury = starty; cury <= endy; cury++) {
                if (dy == 0) {
                    // Down and up are the same
                    current = down;
                } else {
                    fy = (double)cury + 0.5;
                    if (fy < down.pixel.y) {
                        fy = down.pixel.y;
                    } else if (fy > up.pixel.y) {
                        fy = up.pixel.y;
                    }
                    fy = fy - down.pixel.y;

                    current.y = cury;
                    scanInterpolate(renderer->render_camera, &down, &diff, fy / dy, &current);
                }

                Vector3 pixel(current.pixel.x + canvas->getXOffset(), current.pixel.y + canvas->getYOffset(),
                              current.pixel.z);
                Vector3 location(current.location.x, current.location.y, current.location.z);
                CanvasFragment fragment(current.front_facing, pixel, location, current.client, color->a == 1.0);

                Color frag_color = *color;
                frag_color.a = 1.0;
                if (cury == starty || cury == endy) {
                    frag_color.mask(Color(0.0, 0.0, 0.0, 0.3));
                }
                frag_color.a = color->a;
                fragment.setColor(frag_color);

                canvas->pushFragment(current.x, current.y, fragment);
            }
        }
    }
}
