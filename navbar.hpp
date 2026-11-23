#ifndef NAVBAR_HPP
#define NAVBAR_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include "raylib.h"
#include "exercises.hpp"

constexpr float NAVBAR_HEIGHT = 56.0f;

template <std::size_t ItemCount>
struct NavbarState
{
    std::size_t activeIndex = 0;
    int hoveredIndex = -1;
    int currentCursor = MOUSE_CURSOR_DEFAULT;
    int cachedWidth = -1;
    int brandWidth = 0;
    bool showBrand = true;
    std::array<int, ItemCount> labelWidths{};
    std::array<Rectangle, ItemCount> itemRects{};
};

template <std::size_t ItemCount>
inline void UpdateNavbarLayout(
    NavbarState<ItemCount> &state,
    const std::array<ExerciseInfo, ItemCount> &items,
    int screenWidth)
{
    if (state.cachedWidth == screenWidth)
    {
        return;
    }

    state.cachedWidth = screenWidth;

    constexpr int brandFontSize = 20;
    constexpr int tabFontSize = 16;
    constexpr float outerMargin = 16.0f;
    constexpr float brandGap = 24.0f;
    constexpr float tabGap = 8.0f;
    constexpr float tabHeight = 36.0f;
    const float tabY = (NAVBAR_HEIGHT - tabHeight) * 0.5f;

    if (state.brandWidth == 0)
    {
        state.brandWidth = MeasureText("Raylib", brandFontSize);
        for (std::size_t i = 0; i < ItemCount; i++)
        {
            state.labelWidths[i] = MeasureText(items[i].navLabel, tabFontSize);
        }
    }

    int totalLabelsWidth = 0;
    for (std::size_t i = 0; i < ItemCount; i++)
    {
        totalLabelsWidth += state.labelWidths[i];
    }

    const float brandReserved = static_cast<float>(state.brandWidth) + brandGap;
    const float availableWithBrand = static_cast<float>(screenWidth) - outerMargin * 2.0f - brandReserved;
    const float minTabPadding = 16.0f;
    const float totalGaps = tabGap * static_cast<float>(ItemCount > 0 ? ItemCount - 1 : 0);
    const float minNeeded = static_cast<float>(totalLabelsWidth) + minTabPadding * static_cast<float>(ItemCount) + totalGaps;

    state.showBrand = availableWithBrand >= minNeeded;

    const float startX = state.showBrand ? (outerMargin + brandReserved) : outerMargin;
    const float availableWidth = static_cast<float>(screenWidth) - startX - outerMargin - totalGaps;
    const float extraPerTab = std::max(8.0f, std::min(28.0f, (availableWidth - static_cast<float>(totalLabelsWidth)) / static_cast<float>(ItemCount)));

    float cursorX = startX;
    for (std::size_t i = 0; i < ItemCount; i++)
    {
        const float itemWidth = static_cast<float>(state.labelWidths[i]) + extraPerTab;
        state.itemRects[i] = Rectangle{cursorX, tabY, itemWidth, tabHeight};
        cursorX += itemWidth + tabGap;
    }
}

template <std::size_t ItemCount>
inline bool UpdateNavbar(
    NavbarState<ItemCount> &state,
    const std::array<ExerciseInfo, ItemCount> &items,
    int screenWidth)
{
    UpdateNavbarLayout(state, items, screenWidth);

    const std::size_t previousIndex = state.activeIndex;
    const Vector2 mousePosition = GetMousePosition();
    state.hoveredIndex = -1;

    if (mousePosition.y >= 0.0f && mousePosition.y <= NAVBAR_HEIGHT)
    {
        for (std::size_t i = 0; i < ItemCount; i++)
        {
            if (CheckCollisionPointRec(mousePosition, state.itemRects[i]))
            {
                state.hoveredIndex = static_cast<int>(i);
                break;
            }
        }
    }

    const int desiredCursor = (state.hoveredIndex >= 0) ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT;
    if (desiredCursor != state.currentCursor)
    {
        SetMouseCursor(desiredCursor);
        state.currentCursor = desiredCursor;
    }

    if (state.hoveredIndex >= 0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        state.activeIndex = static_cast<std::size_t>(state.hoveredIndex);
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        state.activeIndex = (state.activeIndex + 1) % ItemCount;
    }
    else if (IsKeyPressed(KEY_LEFT))
    {
        state.activeIndex = (state.activeIndex + ItemCount - 1) % ItemCount;
    }

    for (std::size_t i = 0; i < ItemCount && i < 9; i++)
    {
        if (IsKeyPressed(KEY_ONE + static_cast<int>(i)) || IsKeyPressed(KEY_KP_1 + static_cast<int>(i)))
        {
            state.activeIndex = i;
            break;
        }
    }

    return state.activeIndex != previousIndex;
}

template <std::size_t ItemCount>
inline void DrawNavbar(
    const NavbarState<ItemCount> &state,
    const std::array<ExerciseInfo, ItemCount> &items,
    int screenWidth)
{
    constexpr Color barBackground = {24, 30, 41, 255};
    constexpr Color barBorder = {51, 65, 85, 255};
    constexpr Color activeTabColor = {37, 99, 235, 255};
    constexpr Color hoverTabColor = {42, 52, 68, 255};
    constexpr Color inactiveTextColor = {203, 213, 225, 255};
    constexpr Color activeTextColor = {255, 255, 255, 255};
    constexpr Color brandColor = {125, 211, 252, 255};

    DrawRectangle(0, 0, screenWidth, static_cast<int>(NAVBAR_HEIGHT), barBackground);
    DrawLine(0, static_cast<int>(NAVBAR_HEIGHT), screenWidth, static_cast<int>(NAVBAR_HEIGHT), barBorder);

    if (state.showBrand)
    {
        constexpr int brandFontSize = 20;
        const int brandY = (static_cast<int>(NAVBAR_HEIGHT) - brandFontSize) / 2;
        DrawText("Raylib", 16, brandY, brandFontSize, brandColor);
        const int dividerX = 16 + state.brandWidth + 12;
        DrawLine(dividerX, 14, dividerX, static_cast<int>(NAVBAR_HEIGHT) - 14, barBorder);
    }

    constexpr int tabFontSize = 16;
    for (std::size_t i = 0; i < ItemCount; i++)
    {
        const Rectangle &rect = state.itemRects[i];
        const bool isActive = (i == state.activeIndex);
        const bool isHovered = (static_cast<int>(i) == state.hoveredIndex);

        if (isActive)
        {
            DrawRectangleRounded(rect, 0.25f, 6, activeTabColor);
        }
        else if (isHovered)
        {
            DrawRectangleRounded(rect, 0.25f, 6, hoverTabColor);
        }

        const int textX = static_cast<int>(rect.x + (rect.width - static_cast<float>(state.labelWidths[i])) * 0.5f);
        const int textY = static_cast<int>(rect.y + (rect.height - static_cast<float>(tabFontSize)) * 0.5f);
        DrawText(items[i].navLabel, textX, textY, tabFontSize, isActive ? activeTextColor : inactiveTextColor);
    }
}

#endif
