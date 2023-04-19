#include <fstream>
#include <algorithm>
#include "Scene.h"
#include "Integrator.h"

int main()
{
    Scene *scene;
    scene = new Scene();
    scene->render();
}