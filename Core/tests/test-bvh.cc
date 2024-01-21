#include <gtest/gtest.h>
#include <Core/bvh.h>

#include <Core/sphere.h>

#include <Core/primitive.h>

using namespace rdcraft;
struct SphereInfo {
  Real radius;
  Vec3 center;
  SphereInfo(Real radius, Real x, Real y, Real z)
    : radius(radius), center(x, y, z) {
  }
};

bool approx(const Vec3& a, const Vec3& b) {
  return std::abs(a.x - b.x) < EPS && std::abs(a.y - b.y) < EPS &&
         std::abs(a.z - b.z) < EPS;
}

TEST(BVHTest, IntersectsWithSpheres) {
  std::vector<SphereInfo> spheresInfo;
  spheresInfo.emplace_back(1.0, 0.0, 0.0, 0.0);
  spheresInfo.emplace_back(1.0, 1.0, 2.0, 0.0);
  spheresInfo.emplace_back(0.5, -2.0, 1.0, 0.0);
  spheresInfo.emplace_back(1.2, -1.5, -2.5, 0.0);
  PolymorphicVector<Primitive> spheres{};
  MemoryPool<Transform> transforms{};
  for (const auto& info : spheresInfo) {
    auto transform = transforms.construct(Transform::translate(info.center));
    auto invTransform = transforms.construct(transform->inverse());
    spheres.constructDerived<GeometryPrimitive>(
        std::make_unique<Sphere>(info.radius, invTransform, transform),
        static_cast<Material*>(nullptr));
  }
  auto aggregate = std::make_unique<Aggregate>(std::move(spheres));
  std::vector<Ray> rays;
  // randomly generate some rays
  rays.emplace_back(Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0));
  rays.emplace_back(Vec3(0.0, 0.0, 0.5), Vec3(0.0, 1.0, 0.0));
  rays.emplace_back(Vec3(0.0, 1.5, 0.0), Vec3(0.0, 0.0, 1.0));
  rays.emplace_back(Vec3(1.0+1e5, 0.0, 0.0), Vec3(1.0, 1.0, 0.0));
  std::vector<std::optional<Vec3>> answers;
  answers.emplace_back(std::make_optional<Vec3>(1.0, 0.0, 0.0));
  answers.emplace_back(std::make_optional<Vec3>(0.0, std::sqrt(3) * 0.5, 0.5));
  answers.emplace_back(std::nullopt);
  answers.emplace_back(std::nullopt);
  for (int i = 0; i < rays.size(); ++i) {
    auto ray = rays[i];
    auto answer = answers[i];
    std::optional<SurfaceInteraction> interaction;
    aggregate->intersect(ray, interaction);
    if (answer) {
      ASSERT_TRUE(interaction);
      ASSERT_TRUE(approx(interaction->pos, *answer));
    } else {
      ASSERT_FALSE(interaction);
    }
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}