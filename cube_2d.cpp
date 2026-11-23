#include "raylib.h"
#include "exercises.hpp"

int main()
{
    InitWindow(800, 450, "Cubo 2D");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCube2D(Vector2{400.0f, 225.0f}, 200.0f);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}