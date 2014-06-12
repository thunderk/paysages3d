#include "Rasterizer.h"

#include "SoftwareRenderer.h"
#include "CameraDefinition.h"
#include "CanvasPortion.h"
#include "CanvasFragment.h"
#include "Vector3.h"

struct paysages::software::ScanPoint
{
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
};

struct paysages::software::RenderScanlines
{
    ScanPoint* up;
    ScanPoint* down;
    int left;
    int right;
};

Rasterizer::Rasterizer(SoftwareRenderer* renderer, int client_id):
    renderer(renderer), client_id(client_id)
{
}

Rasterizer::~Rasterizer()
{
}

void Rasterizer::pushProjectedTriangle(CanvasPortion *canvas, const Vector3 &pixel1, const Vector3 &pixel2, const Vector3 &pixel3, const Vector3 &location1, const Vector3 &location2, const Vector3 &location3)
{
    ScanPoint point1, point2, point3;
    double limit_width = (double)(canvas->getWidth() - 1);
    double limit_height = (double)(canvas->getHeight() - 1);

    /* Filter if outside screen */
    if (pixel1.z < 1.0 || pixel2.z < 1.0 || pixel3.z < 1.0 || (pixel1.x < 0.0 && pixel2.x < 0.0 && pixel3.x < 0.0) || (pixel1.y < 0.0 && pixel2.y < 0.0 && pixel3.y < 0.0) || (pixel1.x > limit_width && pixel2.x > limit_width && pixel3.x > limit_width) || (pixel1.y > limit_height && pixel2.y > limit_height && pixel3.y > limit_height))
    {
        return;
    }

    /* Prepare vertices */
    // FIXME Apply canvas portion offset
    point1.pixel.x = pixel1.x;
    point1.pixel.y = pixel1.y;
    point1.pixel.z = pixel1.z;
    point1.location.x = location1.x;
    point1.location.y = location1.y;
    point1.location.z = location1.z;
    point1.client = client_id;

    point2.pixel.x = pixel2.x;
    point2.pixel.y = pixel2.y;
    point2.pixel.z = pixel2.z;
    point2.location.x = location2.x;
    point2.location.y = location2.y;
    point2.location.z = location2.z;
    point2.client = client_id;

    point3.pixel.x = pixel3.x;
    point3.pixel.y = pixel3.y;
    point3.pixel.z = pixel3.z;
    point3.location.x = location3.x;
    point3.location.y = location3.y;
    point3.location.z = location3.z;
    point3.client = client_id;

    /* Prepare scanlines */
    // TODO Don't create scanlines for each triangles (one by thread is more appropriate)
    RenderScanlines scanlines;
    int width = canvas->getWidth();
    scanlines.left = width;
    scanlines.right = -1;
    scanlines.up = new ScanPoint[width];
    scanlines.down = new ScanPoint[width];

    /* Render edges in scanlines */
    pushScanLineEdge(canvas, &scanlines, &point1, &point2);
    pushScanLineEdge(canvas, &scanlines, &point2, &point3);
    pushScanLineEdge(canvas, &scanlines, &point3, &point1);

    /* Commit scanlines to area */
    renderScanLines(canvas, &scanlines);

    /* Free scalines */
    delete[] scanlines.up;
    delete[] scanlines.down;
}

void Rasterizer::pushTriangle(CanvasPortion *canvas, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
    Vector3 p1, p2, p3;

    p1 = getRenderer()->projectPoint(v1);
    p2 = getRenderer()->projectPoint(v2);
    p3 = getRenderer()->projectPoint(v3);

    pushProjectedTriangle(canvas, p1, p2, p3, v1, v2, v3);
}

void Rasterizer::pushQuad(CanvasPortion *canvas, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &v4)
{
    pushTriangle(canvas, v2, v3, v1);
    pushTriangle(canvas, v4, v1, v3);
}

void Rasterizer::pushDisplacedTriangle(CanvasPortion *canvas, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &ov1, const Vector3 &ov2, const Vector3 &ov3)
{
    Vector3 p1, p2, p3;

    p1 = getRenderer()->projectPoint(v1);
    p2 = getRenderer()->projectPoint(v2);
    p3 = getRenderer()->projectPoint(v3);

    pushProjectedTriangle(canvas, p1, p2, p3, ov1, ov2, ov3);
}

void Rasterizer::pushDisplacedQuad(CanvasPortion *canvas, const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &v4, const Vector3 &ov1, const Vector3 &ov2, const Vector3 &ov3, const Vector3 &ov4)
{
    pushDisplacedTriangle(canvas, v2, v3, v1, ov2, ov3, ov1);
    pushDisplacedTriangle(canvas, v4, v1, v3, ov4, ov1, ov3);
}

void Rasterizer::rasterizeToCanvas(CanvasPortion *)
{
}

void Rasterizer::scanGetDiff(ScanPoint* v1, ScanPoint* v2, ScanPoint* result)
{
    result->pixel.x = v2->pixel.x - v1->pixel.x;
    result->pixel.y = v2->pixel.y - v1->pixel.y;
    result->pixel.z = v2->pixel.z - v1->pixel.z;
    result->location.x = v2->location.x - v1->location.x;
    result->location.y = v2->location.y - v1->location.y;
    result->location.z = v2->location.z - v1->location.z;
    result->client = v1->client;
}

void Rasterizer::scanInterpolate(CameraDefinition* camera, ScanPoint* v1, ScanPoint* diff, double value, ScanPoint* result)
{
    Vector3 vec1(v1->pixel.x, v1->pixel.y, v1->pixel.z);
    Vector3 vecdiff(diff->pixel.x, diff->pixel.y, diff->pixel.z);
    double v1depth = camera->getRealDepth(vec1);
    double v2depth = camera->getRealDepth(vec1.add(vecdiff));
    double factor = ((1.0 - value) / v1depth + value / v2depth);

    result->pixel.x = v1->pixel.x + diff->pixel.x * value;
    result->pixel.y = v1->pixel.y + diff->pixel.y * value;
    result->pixel.z = v1->pixel.z + diff->pixel.z * value;
    result->location.x = ((1.0 - value) * (v1->location.x / v1depth) + value * (v1->location.x + diff->location.x) / v2depth) / factor;
    result->location.y = ((1.0 - value) * (v1->location.y / v1depth) + value * (v1->location.y + diff->location.y) / v2depth) / factor;
    result->location.z = ((1.0 - value) * (v1->location.z / v1depth) + value * (v1->location.z + diff->location.z) / v2depth) / factor;
    result->client = v1->client;
}

void Rasterizer::pushScanPoint(CanvasPortion* canvas, RenderScanlines* scanlines, ScanPoint* point)
{
    point->x = (int)floor(point->pixel.x);
    point->y = (int)floor(point->pixel.y);

    if (point->x < 0 || point->x >= canvas->getWidth())
    {
        // Point outside scanline range
        return;
    }
    else if (scanlines->right < 0)
    {
        // First point pushed
        scanlines->left = point->x;
        scanlines->right = point->x;
        scanlines->up[point->x] = *point;
        scanlines->down[point->x] = *point;
    }
    else if (point->x > scanlines->right)
    {
        // Grow scanlines to right
        for (int x = scanlines->right + 1; x < point->x; x++)
        {
            scanlines->up[x].y = -1;
            scanlines->down[x].y = canvas->getHeight();
        }
        scanlines->right = point->x;
        scanlines->up[point->x] = *point;
        scanlines->down[point->x] = *point;
    }
    else if (point->x < scanlines->left)
    {
        // Grow scanlines to left
        for (int x = point->x + 1; x < scanlines->left; x++)
        {
            scanlines->up[x].y = -1;
            scanlines->down[x].y = canvas->getHeight();
        }
        scanlines->left = point->x;
        scanlines->up[point->x] = *point;
        scanlines->down[point->x] = *point;
    }
    else
    {
        // Expand existing scanline
        if (point->y > scanlines->up[point->x].y)
        {
            scanlines->up[point->x] = *point;
        }
        if (point->y < scanlines->down[point->x].y)
        {
            scanlines->down[point->x] = *point;
        }
    }
}

void Rasterizer::pushScanLineEdge(CanvasPortion *canvas, RenderScanlines *scanlines, ScanPoint *point1, ScanPoint *point2)
{
    double dx, fx;
    ScanPoint diff, point;
    int startx = lround(point1->pixel.x);
    int endx = lround(point2->pixel.x);
    int curx;

    if (endx < startx)
    {
        pushScanLineEdge(canvas, scanlines, point2, point1);
    }
    else if (endx < 0 || startx >= canvas->getWidth())
    {
        return;
    }
    else if (startx == endx)
    {
        pushScanPoint(canvas, scanlines, point1);
        pushScanPoint(canvas, scanlines, point2);
    }
    else
    {
        if (startx < 0)
        {
            startx = 0;
        }
        if (endx >= canvas->getWidth())
        {
            endx = canvas->getWidth() - 1;
        }

        dx = point2->pixel.x - point1->pixel.x;
        scanGetDiff(point1, point2, &diff);
        for (curx = startx; curx <= endx; curx++)
        {
            fx = (double)curx + 0.5;
            if (fx < point1->pixel.x)
            {
                fx = point1->pixel.x;
            }
            else if (fx > point2->pixel.x)
            {
                fx = point2->pixel.x;
            }
            fx = fx - point1->pixel.x;
            scanInterpolate(renderer->render_camera, point1, &diff, fx / dx, &point);

            /*point.pixel.x = (double)curx;*/

            pushScanPoint(canvas, scanlines, &point);
        }
    }
}

void Rasterizer::renderScanLines(CanvasPortion *canvas, RenderScanlines* scanlines)
{
    int x, starty, endy, cury;
    ScanPoint diff;
    double dy, fy;
    ScanPoint up, down, current;

    if (scanlines->right > 0)
    {
        for (x = scanlines->left; x <= scanlines->right; x++)
        {
            up = scanlines->up[x];
            down = scanlines->down[x];

            starty = down.y;
            endy = up.y;

            if (endy < 0 || starty >= canvas->getHeight())
            {
                continue;
            }

            if (starty < 0)
            {
                starty = 0;
            }
            if (endy >= canvas->getHeight())
            {
                endy = canvas->getHeight() - 1;
            }

            dy = up.pixel.y - down.pixel.y;
            scanGetDiff(&down, &up, &diff);

            current.x = x;
            for (cury = starty; cury <= endy; cury++)
            {
                fy = (double)cury + 0.5;
                if (fy < down.pixel.y)
                {
                    fy = down.pixel.y;
                }
                else if (fy > up.pixel.y)
                {
                    fy = up.pixel.y;
                }
                fy = fy - down.pixel.y;

                current.y = cury;
                scanInterpolate(renderer->render_camera, &down, &diff, fy / dy, &current);

                CanvasFragment fragment(current.pixel.z, Vector3(current.location.x, current.location.y, current.location.z), current.client);
                fragment.setColor((cury == starty || cury == endy) ? COLOR_GREY : COLOR_WHITE);
                canvas->pushFragment(current.x, current.y, fragment);
            }
        }
    }
}
