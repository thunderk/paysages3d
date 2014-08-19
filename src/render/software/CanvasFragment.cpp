#include "CanvasFragment.h"

CanvasFragment::CanvasFragment()
{
}

CanvasFragment::CanvasFragment(double z, const Vector3 &location, int client, bool opaque):
    opaque(opaque), z(z), location(location), client(client)
{
    color = COLOR_WHITE;
}

void CanvasFragment::setColor(const Color &col)
{
    color = col;
}
