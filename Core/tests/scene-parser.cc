//
// Created by creeper on 12/21/23.
//
#include <Core/scene-parser.h>
#include <gtest/gtest.h>
constexpr char cbox[] = RDCRAFT_SCENE_DIR"/cbox/cbox.xml";
std::unique_ptr<rdcraft::Scene> scene;
std::unique_ptr<rdcraft::Integrator> integrator;
TEST(SceneMembers) {

}

int main() {
  ::testing::InitGoogleTest();
  auto [scene, integrator] = rdcraft::loadScene(cbox);

}