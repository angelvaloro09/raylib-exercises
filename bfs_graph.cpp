#include <algorithm>
#include "raylib.h"
#include "exercises.hpp"

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1000, 600, "Arbol de 20 nodos - BFS (Breadth First Search)");
    SetWindowMinSize(720, 420);
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        const float w = static_cast<float>(GetScreenWidth());
        const float h = static_cast<float>(GetScreenHeight());

        ExerciseContext context{};
        context.contentArea = Rectangle{0.0f, 0.0f, w, h};
        context.center = Vector2{w * 0.5f, h * 0.5f};

        BeginDrawing();
        ClearBackground(RAYWHITE);

        RenderGraphBfsExercise(context);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
