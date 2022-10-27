#pragma once
#include "Microfacet.h"
#include "Material.h"
static Microfacet* trowbridgeMirror = new TrowbridgeModel(0.005f);
static Microfacet* trowbridgeGlossy = new TrowbridgeModel(0.25f);
static Material* Copper = new Metal(0.211f, 1.0344f, 1.2474f, 4.1592f, 2.5798f, 2.3289f, trowbridgeMirror);
static Material* Gold = new Metal(0.131f, 0.44715f, 1.4318f, 4.0624f, 2.4212f, 1.9392f, trowbridgeMirror);
static Material* Silver = new Metal(0.0410f, 0.059097f, 0.04f, 4.8025f, 3.5624f, 2.5127f, trowbridgeGlossy);
static Material* LambertianR = new Lambertian(1.0f, 0.0f, 0.0f);
static Material* LambertianG = new Lambertian(0.0f, 1.0f, 0.0f);
static Material* LambertianB = new Lambertian(0.0f, 0.0f, 1.0f);
static Material* Ice = new Translucent(1.31f, trowbridgeMirror);
static Material* Diamond = new Translucent(2.42f, trowbridgeMirror);