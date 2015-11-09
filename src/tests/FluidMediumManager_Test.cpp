#include "BaseTestCase.h"

#include "SpaceSegment.h"
#include "Scenery.h"
#include "SoftwareRenderer.h"
#include "FluidMediumManager.h"
#include "FluidMediumInterface.h"

class FluidMediumManager_Test : public BaseTestCase {
  protected:
    virtual void setUp() {
        scenery = new Scenery();
        renderer = new SoftwareRenderer(scenery);
    }

    virtual void tearDown() {
        delete renderer;
        delete scenery;
    }

    Scenery *scenery;
    SoftwareRenderer *renderer;
};

TEST_F(FluidMediumManager_Test, getTraversedMedia) {
    FluidMediumManager manager(renderer);
    int traversed;
    FluidMediumSegment segments[10];
    class FakeMedium : public FluidMediumInterface {
      public:
        void setCut(double ymin, double ymax) {
            this->ymin = ymin;
            this->ymax = ymax;
        }

        virtual bool checkInfluence(SpaceSegment &segment) const override {
            return segment.intersectYInterval(this->ymin, this->ymax);
        }

      private:
        double ymin = -2.0;
        double ymax = -1.0;
    };
    SpaceSegment ray(Vector3(0.0, 0.0, 0.0), Vector3(8.0, 10.0, -4.0));

    // Empty manager
    traversed = manager.getTraversedMedia(segments, ray, 10);
    ASSERT_EQ(0, traversed);

    // Testing with 1 medium outside the range
    FakeMedium m1;
    manager.registerMedium(&m1);
    traversed = manager.getTraversedMedia(segments, ray, 10);
    ASSERT_EQ(0, traversed);

    // Setting the medium in range
    m1.setCut(1.0, 2.0);
    traversed = manager.getTraversedMedia(segments, ray, 10);
    ASSERT_EQ(1, traversed);
    EXPECT_EQ((FluidMediumInterface *)&m1, segments[0].medium);
    EXPECT_VECTOR3_COORDS(segments[0].segment.getStart(), 0.8, 1.0, -0.4);
    EXPECT_VECTOR3_COORDS(segments[0].segment.getEnd(), 1.6, 2.0, -0.8);

    // Testing with 2 media
    FakeMedium m2;
    m2.setCut(4.0, 12.0);
    manager.registerMedium(&m2);
    traversed = manager.getTraversedMedia(segments, ray, 10);
    ASSERT_EQ(2, traversed);
    EXPECT_EQ((FluidMediumInterface *)&m1, segments[0].medium);
    EXPECT_VECTOR3_COORDS(segments[0].segment.getStart(), 0.8, 1.0, -0.4);
    EXPECT_VECTOR3_COORDS(segments[0].segment.getEnd(), 1.6, 2.0, -0.8);
    EXPECT_EQ((FluidMediumInterface *)&m2, segments[1].medium);
    EXPECT_VECTOR3_COORDS(segments[1].segment.getStart(), 3.2, 4.0, -1.6);
    EXPECT_VECTOR3_COORDS(segments[1].segment.getEnd(), 8.0, 10.0, -4.0);

    // Testing with overlapping media
    FakeMedium m3;
    m3.setCut(3.0, 4.5);
    manager.registerMedium(&m3);
    traversed = manager.getTraversedMedia(segments, ray, 10);
    ASSERT_EQ(3, traversed);
    EXPECT_EQ((FluidMediumInterface *)&m1, segments[0].medium);
    EXPECT_VECTOR3_COORDS(segments[0].segment.getStart(), 0.8, 1.0, -0.4);
    EXPECT_VECTOR3_COORDS(segments[0].segment.getEnd(), 1.6, 2.0, -0.8);
    EXPECT_EQ((FluidMediumInterface *)&m2, segments[1].medium);
    EXPECT_VECTOR3_COORDS(segments[1].segment.getStart(), 3.2, 4.0, -1.6);
    EXPECT_VECTOR3_COORDS(segments[1].segment.getEnd(), 8.0, 10.0, -4.0);
    EXPECT_EQ((FluidMediumInterface *)&m3, segments[2].medium);
    EXPECT_VECTOR3_COORDS(segments[2].segment.getStart(), 2.4, 3.0, -1.2);
    EXPECT_VECTOR3_COORDS(segments[2].segment.getEnd(), 3.6, 4.5, -1.8);

    // Testing the segment count limit
    traversed = manager.getTraversedMedia(segments, ray, 2);
    ASSERT_EQ(2, traversed);
    EXPECT_EQ((FluidMediumInterface *)&m1, segments[0].medium);
    EXPECT_EQ((FluidMediumInterface *)&m2, segments[1].medium);

    // Testing after clear
    manager.clearMedia();
    traversed = manager.getTraversedMedia(segments, ray, 10);
    ASSERT_EQ(0, traversed);
}
