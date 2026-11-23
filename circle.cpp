#include "raylib.h"
#include "exercises.hpp"

int main()
{
    InitWindow(800, 450, "Circulo");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawYellowCircle(Vector2{400.0f, 225.0f});

        EndDrawing();
    }

    CloseWindow();
    return 0;
}