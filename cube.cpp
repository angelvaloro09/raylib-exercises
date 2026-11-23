#include "raylib.h"
#include "exercises.hpp"

int main()
{
    InitWindow(800, 450, "Cubo 3D");
    SetTargetFPS(60);

    Camera3D camera = CreateDefaultCubeCamera();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawSceneCube(Vector3{0.0f, 0.0f, 0.0f}, 2.0f);
        EndMode3D();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}