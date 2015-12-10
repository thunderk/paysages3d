#include "BaseTestCase.h"

#include "Color.h"
#include "ColorHSL.h"

TEST(ColorHSL, colorFromHSL) {
    vector<Color> colors;
    colors.push_back(Color());
    colors.push_back(Color(1.0, 0.0, 0.0, 1.0));
    colors.push_back(Color(0.7, 0.5, 0.3, 1.0));
    // colors.push_back(Color(2.0, 0.0, 0.0, 1.0));  // FIXME not working yet
    colors.push_back(Color(3.0, 3.0, 3.0, 1.0));

    for (auto &color : colors) {
        ColorHSL hsl = colorToHSL(color);
        Color rgb = colorFromHSL(hsl);

        EXPECT_COLOR_RGBA(rgb, color.r, color.g, color.b, color.a);
    }
}
