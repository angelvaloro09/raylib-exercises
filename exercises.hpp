#ifndef EXERCISES_HPP
#define EXERCISES_HPP

#include <algorithm>
#include <array>
#include "raylib.h"
#include "graph_traversal.hpp"

struct ExerciseContext
{
    Rectangle contentArea;
    Vector2 center;
    Camera3D camera;
};

struct ExerciseInfo
{
    const char *navLabel;
    const char *title;
    const char *hint;
    bool is3D;
    void (*render)(const ExerciseContext &context);
};

inline Camera3D CreateDefaultCubeCamera()
{
    Camera3D camera = {};
    camera.position = Vector3{4.0f, 3.5f, 4.5f};
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    return camera;
}

inline void DrawYellowCircle(Vector2 center, float radius = 120.0f)
{
    DrawCircleV(center, radius, YELLOW);
}

inline void DrawSeriousFace(Vector2 center, float scale = 1.0f)
{
    const float faceRadius = 120.0f * scale;
    const float eyeOffset = 60.0f * scale;
    const float eyeRadius = 25.0f * scale;
    const float mouthThickness = 6.0f * scale;

    DrawYellowCircle(center, faceRadius);
    DrawCircleV(Vector2{center.x - eyeOffset, center.y - eyeOffset}, eyeRadius, BLACK);
    DrawCircleV(Vector2{center.x + eyeOffset, center.y - eyeOffset}, eyeRadius, BLACK);
    DrawLineEx(
        Vector2{center.x - eyeOffset, center.y + eyeOffset},
        Vector2{center.x + eyeOffset, center.y + eyeOffset},
        mouthThickness,
        BLACK);
}

inline void DrawCube2D(Vector2 center, float size)
{
    const float offset = size * 0.5f;
    const float x1 = center.x - (size + offset) * 0.5f;
    const float y1 = center.y - (size + offset) * 0.5f;
    const float x2 = x1 + offset;
    const float y2 = y1 + offset;

    const Vector2 vertices[8] = {
        {x1, y1},
        {x1 + size, y1},
        {x1 + size, y1 + size},
        {x1, y1 + size},
        {x2, y2},
        {x2 + size, y2},
        {x2 + size, y2 + size},
        {x2, y2 + size}};

    static constexpr int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    for (int i = 0; i < 12; i++)
    {
        const int originIndex = edges[i][0];
        const int targetIndex = edges[i][1];
        DrawLineEx(vertices[originIndex], vertices[targetIndex], 6.0f, BLACK);
    }
}

inline void DrawSceneCube(Vector3 position, float size)
{
    DrawCube(position, size, size, size, SKYBLUE);
    DrawCubeWires(position, size, size, size, DARKBLUE);
}

inline void DrawEdge(Vector2 start, Vector2 end, Color color)
{
    DrawLineEx(start, end, 3.0f, color);
}

inline void DrawGraph(const Vector2 vertices[], int vertexCount, const int edges[][2], int edgeCount, float radius)
{
    for (int i = 0; i < edgeCount; i++)
    {
        const int originIndex = edges[i][0];
        const int targetIndex = edges[i][1];
        DrawEdge(vertices[originIndex], vertices[targetIndex], DARKGRAY);
    }

    const int fontSize = 20;
    for (int i = 0; i < vertexCount; i++)
    {
        DrawCircleV(vertices[i], radius, SKYBLUE);
        DrawCircleLinesV(vertices[i], radius, DARKBLUE);

        const char *label = TextFormat("%d", i);
        const int textWidth = MeasureText(label, fontSize);
        DrawText(
            label,
            static_cast<int>(vertices[i].x) - textWidth / 2,
            static_cast<int>(vertices[i].y) - fontSize / 2,
            fontSize,
            BLACK);
    }
}

inline void RenderCircleExercise(const ExerciseContext &context)
{
    const float minDimension = std::min(context.contentArea.width, context.contentArea.height);
    const float radius = std::min(120.0f, minDimension * 0.34f);
    DrawYellowCircle(context.center, radius);
}

inline void RenderSeriousFaceExercise(const ExerciseContext &context)
{
    const float minDimension = std::min(context.contentArea.width, context.contentArea.height);
    const float scale = std::min(1.0f, minDimension / 340.0f);
    DrawSeriousFace(context.center, scale);
}

inline void RenderCube2DExercise(const ExerciseContext &context)
{
    const float minDimension = std::min(context.contentArea.width, context.contentArea.height);
    const float size = std::min(200.0f, minDimension * 0.45f);
    DrawCube2D(context.center, size);
}

inline void RenderCube3DExercise(const ExerciseContext &context)
{
    BeginMode3D(context.camera);
    DrawSceneCube(Vector3{0.0f, 0.0f, 0.0f}, 2.0f);
    EndMode3D();
}

inline void RenderGraphExercise(const ExerciseContext &context)
{
    const float spanX = std::min(200.0f, context.contentArea.width * 0.28f);
    const float spanY = std::min(112.5f, context.contentArea.height * 0.28f);
    const float nodeRadius = std::min(30.0f, std::min(spanX, spanY) * 0.28f);

    const Vector2 vertices[4] = {
        {context.center.x - spanX, context.center.y - spanY},
        {context.center.x + spanX, context.center.y - spanY},
        {context.center.x - spanX, context.center.y + spanY},
        {context.center.x + spanX, context.center.y + spanY}};

    static constexpr int edges[6][2] = {
        {0, 1}, {1, 3}, {3, 2}, {2, 0}, {0, 3}, {1, 2}};

    DrawGraph(vertices, 4, edges, 6, nodeRadius);
}

inline void RenderGraphDfsExercise(const ExerciseContext &context)
{
    static TraversalState dfsState = []() {
        TraversalState state{};
        state.algorithm = TraversalAlgorithm::DFS;
        return state;
    }();
    RenderTraversalExercise(context.contentArea, dfsState);
}

inline void RenderGraphBfsExercise(const ExerciseContext &context)
{
    static TraversalState bfsState = []() {
        TraversalState state{};
        state.algorithm = TraversalAlgorithm::BFS;
        return state;
    }();
    RenderTraversalExercise(context.contentArea, bfsState);
}

inline const std::array<ExerciseInfo, 7> &GetExercises()
{
    static constexpr std::array<ExerciseInfo, 7> exercises = {{
        {"1. Circulo", "Circulo", "Figura basica 2D", false, RenderCircleExercise},
        {"2. Cara seria", "Cara seria", "Composicion 2D", false, RenderSeriousFaceExercise},
        {"3. Cubo 2D", "Cubo 2D", "Proyeccion isometrica con aristas", false, RenderCube2DExercise},
        {"4. Cubo 3D", "Cubo 3D", "Arrastra con el mouse para rotar | R: reiniciar camara", true, RenderCube3DExercise},
        {"5. Grafo", "Grafo no dirigido (4 vertices y 6 aristas)", "Grafo completo K4", false, RenderGraphExercise},
        {"6. DFS", "Arbol de 20 nodos - DFS (Profundidad)", "Click en nodo: visitar y sugerir | R: reiniciar", false, RenderGraphDfsExercise},
        {"7. BFS", "Arbol de 20 nodos - BFS (Amplitud)", "Click en nodo: visitar y sugerir | R: reiniciar", false, RenderGraphBfsExercise}}};

    return exercises;
}

#endif
