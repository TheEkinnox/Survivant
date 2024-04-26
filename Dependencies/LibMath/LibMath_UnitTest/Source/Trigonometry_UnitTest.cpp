#include <Trigonometry.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <glm/glm.hpp>

using namespace LibMath::Literal;

TEST_CASE("Trigonometry", "[.all][trigonometry]")
{
    std::vector<LibMath::Radian> angles({ 1.2_rad, -1.2_rad, -360_deg, 360_deg, 295.67_deg, -295.67_deg, 770_deg, -770_deg });

    for (LibMath::Radian& radian : angles)
    {
        const float glmSin = glm::sin(radian.raw());
        const float glmCos = glm::cos(radian.raw());
        const float glmTan = glm::tan(radian.raw());

        CHECK(LibMath::floatEquals(LibMath::sin(radian), glmSin));
        CHECK(LibMath::floatEquals(LibMath::cos(radian), glmCos));
        CHECK(LibMath::floatEquals(LibMath::tan(radian), glmTan));

        CHECK(LibMath::asin(glmSin).radian() == Catch::Approx(glm::asin(glmSin)));
        CHECK(LibMath::acos(glmCos).radian() == Catch::Approx(glm::acos(glmCos)));
        CHECK(LibMath::atan(glmTan).radian() == Catch::Approx(glm::atan(glmTan)));
    }

    CHECK(LibMath::atan(1.f, 2.f).radian() == Catch::Approx(glm::atan(1.f, 2.f)));
    CHECK(LibMath::atan(1.f, -2.f).radian() == Catch::Approx(glm::atan(1.f, -2.f)));
    CHECK(LibMath::atan(-1.f, 2.f).radian() == Catch::Approx(glm::atan(-1.f, 2.f)));
    CHECK(LibMath::atan(-1.f, -2.f).radian() == Catch::Approx(glm::atan(-1.f, -2.f)));
}
