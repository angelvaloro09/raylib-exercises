#include <algorithm>
#include <cmath>
#include "raylib.h"
#include "exercises.hpp"
#include "navbar.hpp"

namespace
{
constexpr float FOOTER_HEIGHT = 32.0f;

struct OrbitCameraState
{
    float yaw = 0.726642f;
    float pitch = 0.526681f;
    float distance = 6.964194f;
    bool isDragging = false;
};

void ResetOrbitCamera(OrbitCameraState &orbit, Camera3D &camera)
{
    orbit = OrbitCameraState{};
    camera = CreateDefaultCubeCamera();
}

void UpdateOrbitCamera(OrbitCameraState &orbit, Camera3D &camera, const Rectangle &contentArea)
{
    if (IsKeyPressed(KEY_R))
    {
        ResetOrbitCamera(orbit, camera);
        return;
    }

    const Vector2 mousePosition = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, contentArea))
    {
        orbit.isDragging = true;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        orbit.isDragging = false;
    }

    if (!orbit.isDragging)
    {
        return;
    }

    const Vector2 delta = GetMouseDelta();
    if (delta.x == 0.0f && delta.y == 0.0f)
    {
        return;
    }

    constexpr float sensitivity = 0.008f;
    constexpr float maxPitch = 1.45f;

    orbit.yaw -= delta.x * sensitivity;
    orbit.pitch = std::clamp(orbit.pitch + delta.y * sensitivity, -maxPitch, maxPitch);

    const float cosPitch = std::cos(orbit.pitch);
    camera.position = Vector3{
        orbit.distance * cosPitch * std::sin(orbit.yaw),
        orbit.distance * std::sin(orbit.pitch),
        orbit.distance * cosPitch * std::cos(orbit.yaw)};
}

void UpdateViewportContext(ExerciseContext &context, int screenWidth, int screenHeight)
{
    const float contentHeight = std::max(1.0f, static_cast<float>(screenHeight) - NAVBAR_HEIGHT - FOOTER_HEIGHT);
    context.contentArea = Rectangle{
        0.0f,
        NAVBAR_HEIGHT,
        static_cast<float>(screenWidth),
        contentHeight};
    context.center = Vector2{
        context.contentArea.x + context.contentArea.width * 0.5f,
        context.contentArea.y + context.contentArea.height * 0.5f};
}

struct FooterCache
{
    std::size_t cachedIndex = static_cast<std::size_t>(-1);
    int titleWidth = 0;
    int hintWidth = 0;
    char hintBuffer[256] = {};
};

void UpdateFooterCache(FooterCache &cache, std::size_t activeIndex, std::size_t totalExercises, const ExerciseInfo &exercise)
{
    if (cache.cachedIndex == activeIndex)
    {
        return;
    }

    constexpr int fontSize = 15;
    cache.cachedIndex = activeIndex;
    cache.titleWidth = MeasureText(exercise.title, fontSize);
    TextCopy(cache.hintBuffer, TextFormat("%s  |  Teclas 1-%d o <- / ->", exercise.hint, static_cast<int>(totalExercises)));
    cache.hintWidth = MeasureText(cache.hintBuffer, fontSize);
}

void DrawFooter(const ExerciseInfo &exercise, const FooterCache &cache, int screenWidth, int screenHeight)
{
    constexpr Color footerBackground = {241, 245, 249, 255};
    constexpr Color footerBorder = {203, 213, 225, 255};
    constexpr Color titleColor = {30, 41, 59, 255};
    constexpr Color hintColor = {100, 116, 139, 255};
    constexpr int fontSize = 15;

    const int footerY = screenHeight - static_cast<int>(FOOTER_HEIGHT);
    const int textY = footerY + (static_cast<int>(FOOTER_HEIGHT) - fontSize) / 2;

    DrawRectangle(0, footerY, screenWidth, static_cast<int>(FOOTER_HEIGHT), footerBackground);
    DrawLine(0, footerY, screenWidth, footerY, footerBorder);

    DrawText(exercise.title, 16, textY, fontSize, titleColor);

    if (16 + cache.titleWidth + 24 + cache.hintWidth < screenWidth)
    {
        DrawText(cache.hintBuffer, screenWidth - cache.hintWidth - 16, textY, fontSize, hintColor);
    }
}
} // namespace

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1080, 640, "Raylib - Circulo");
    SetWindowMinSize(720, 420);
    SetTargetFPS(60);

    const auto &exercises = GetExercises();
    NavbarState<exercises.size()> navbar;
    FooterCache footerCache{};

    ExerciseContext context{};
    context.camera = CreateDefaultCubeCamera();
    OrbitCameraState orbit{};

    int cachedWidth = GetScreenWidth();
    int cachedHeight = GetScreenHeight();
    UpdateViewportContext(context, cachedWidth, cachedHeight);
    UpdateFooterCache(footerCache, navbar.activeIndex, exercises.size(), exercises[navbar.activeIndex]);

    while (!WindowShouldClose())
    {
        const int screenWidth = GetScreenWidth();
        const int screenHeight = GetScreenHeight();
        if (screenWidth != cachedWidth || screenHeight != cachedHeight)
        {
            cachedWidth = screenWidth;
            cachedHeight = screenHeight;
            UpdateViewportContext(context, screenWidth, screenHeight);
        }

        if (UpdateNavbar(navbar, exercises, screenWidth))
        {
            orbit.isDragging = false;
            const ExerciseInfo &selected = exercises[navbar.activeIndex];
            UpdateFooterCache(footerCache, navbar.activeIndex, exercises.size(), selected);
            SetWindowTitle(TextFormat("Raylib - %s", selected.title));
        }

        const ExerciseInfo &activeExercise = exercises[navbar.activeIndex];
        if (activeExercise.is3D)
        {
            UpdateOrbitCamera(orbit, context.camera, context.contentArea);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginScissorMode(
            static_cast<int>(context.contentArea.x),
            static_cast<int>(context.contentArea.y),
            static_cast<int>(context.contentArea.width),
            static_cast<int>(context.contentArea.height));
        activeExercise.render(context);
        EndScissorMode();

        DrawNavbar(navbar, exercises, screenWidth);
        DrawFooter(activeExercise, footerCache, screenWidth, screenHeight);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
