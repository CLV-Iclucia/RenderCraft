//
// Created by creeper on 12/21/23.
//
#include <Core/integrators.h>
#include <Core/scene-parser.h>
#include <gtest/gtest.h>
#include <format>
std::string cbox = std::format("{}/cbox/cbox.xml", RDCRAFT_SCENE_DIR);
std::unique_ptr<rdcraft::Scene> scene;
std::unique_ptr<rdcraft::Integrator> integrator;
// before all the tests, init [scene, integrator] = loadScene(cbox)
class SceneParserTest : public ::testing::Test {
  protected:
    void SetUp() override {
      std::tie(scene, integrator) = rdcraft::loadScene(cbox.c_str());
    }
};

TEST(SceneParserTest, LoadScene) {
  ASSERT_NE(scene, nullptr);
  ASSERT_NE(integrator, nullptr);
}

TEST(SceneParserTest, Integrator) {
  auto path_tracer = dynamic_cast<rdcraft::PathTracer*>(integrator.get());
  ASSERT_NE(path_tracer, nullptr);
  // check the integrator's max depth
  ASSERT_EQ(path_tracer->opt.maxDepth, -1);
}

TEST(SceneParserTest, Camera) {
  ASSERT_NE(scene->camera, nullptr);
  ASSERT_EQ(scene->camera->nearPlane, 10);
  ASSERT_EQ(scene->camera->farPlane, 2800);
  ASSERT_EQ(scene->camera->focalDistance, 1000);
  ASSERT_EQ(scene->camera->spp, 4);
  ASSERT_EQ(scene->camera->nx, 512);
  ASSERT_EQ(scene->camera->ny, 512);
  auto gaussian = dynamic_cast<rdcraft::GaussianFilter*>(scene->camera->filter.
    get());
  ASSERT_NE(gaussian, nullptr);
}

TEST(SceneParserTest, Lights) {
  ASSERT_EQ(scene->lights.size(), 1);
  auto light = dynamic_cast<rdcraft::AreaLight*>(scene->lights(0));
  ASSERT_NE(light, nullptr);
}

TEST(SceneParserTest, Primitives) {
  ASSERT_EQ(scene->meshes.size(), 8);
}

int main() {
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}