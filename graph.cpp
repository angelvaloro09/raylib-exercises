#include "raylib.h"
#include "exercises.hpp"

int main()
{
    InitWindow(800, 450, "Grafo no dirigido de 4 vertices y 6 aristas");
    SetTargetFPS(60);

    constexpr float nodeRadius = 30.0f;

    const Vector2 vertices[4] = {
        {200.0f, 112.5f},
        {600.0f, 112.5f},
        {200.0f, 337.5f},
        {600.0f, 337.5f}};

    static constexpr int edges[6][2] = {
        {0, 1},
        {1, 3},
        {3, 2},
        {2, 0},
        {0, 3},
        {1, 2}};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawGraph(vertices, 4, edges, 6, nodeRadius);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}