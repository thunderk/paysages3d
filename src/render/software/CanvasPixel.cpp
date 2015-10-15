#include "CanvasPixel.h"

#include <cstring>

CanvasPixel::CanvasPixel()
{
    count = 0;
    composite = COLOR_BLACK;
}

const CanvasFragment *CanvasPixel::getFrontFragment() const
{
    if (count == 0)
    {
        return NULL;
    }
    else
    {
        return fragments + (count - 1);
    }
}

void CanvasPixel::reset()
{
    count = 0;
    composite = COLOR_BLACK;
}

void CanvasPixel::pushFragment(const CanvasFragment &fragment)
{
    if (count == 0)
    {
        fragments[0] = fragment;
        count = 1;
    }
    else
    {
        if (fragments[0].getOpaque() and fragment.getZ() <= fragments[0].getZ())
        {
            // behind opaque fragment, don't bother
            return;
        }

        // find expected position
        int i = 0;
        while (i < count and fragment.getZ() >= fragments[i].getZ())
        {
            i++;
        }

        if (i > 0 and fragments[i - 1].getZ() == fragment.getZ() and fragments[i - 1].getClient() == fragment.getClient())
        {
            // Pixel already pushed by same client, don't do anything
            return;
        }

        if (fragment.getOpaque())
        {
            // Discard fragments masked by the incoming opaque one
            if (i < count)
            {
                memmove(fragments + 1, fragments + i, sizeof(CanvasFragment) * (count - i));
                count -= i;
            }
            else
            {
                count = 1;
            }
            fragments[0] = fragment;
        }
        else
        {
            // Transparent pixel
            if (i < count)
            {
                // Need to make room for the incoming fragment
                if (count < MAX_FRAGMENTS_PER_PIXEL)
                {
                    memmove(fragments + i + 1, fragments + i, sizeof(CanvasFragment) * (count - i));
                    fragments[i] = fragment;
                    count++;
                }
            }
            else
            {
                if (count == MAX_FRAGMENTS_PER_PIXEL)
                {
                    // Replace nearest fragment
                    fragments[count - 1] = fragment;
                }
                else
                {
                    // Append
                    fragments[count] = fragment;
                    count++;
                }
            }
        }
    }

    updateComposite();
}

void CanvasPixel::updateComposite()
{
    Color result(0.0, 0.0, 0.0, 1.0);
    for (int i = 0; i < count; i++)
    {
        result.mask(fragments[i].getColor());
    }
    composite = result;
}

void CanvasPixel::setComposite(const Color &color)
{
    composite = color;
}
