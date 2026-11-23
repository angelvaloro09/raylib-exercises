#ifndef GRAPH_TRAVERSAL_HPP
#define GRAPH_TRAVERSAL_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <deque>
#include <string>
#include <vector>
#include "raylib.h"

enum class TraversalAlgorithm
{
    DFS,
    BFS
};

struct Graph20
{
    static constexpr int NODE_COUNT = 20;
    static constexpr int EDGE_COUNT = 19;

    std::array<Vector2, NODE_COUNT> normalizedPositions;
    std::array<std::array<int, 2>, EDGE_COUNT> edges;
    std::array<std::vector<int>, NODE_COUNT> adjacency;
};

inline const Graph20 &GetGraph20()
{
    static const Graph20 graph = []()
    {
        Graph20 g{};

        // 20-node hierarchical tree layout (Levels 0..4) in normalized [0, 1] coordinates
        g.normalizedPositions = {{
            // Level 0 (root)
            {0.50f, 0.08f},
            // Level 1
            {0.20f, 0.27f},
            {0.50f, 0.27f},
            {0.80f, 0.27f},
            // Level 2
            {0.10f, 0.49f},
            {0.28f, 0.49f},
            {0.43f, 0.49f},
            {0.58f, 0.49f},
            {0.73f, 0.49f},
            {0.90f, 0.49f},
            // Level 3
            {0.05f, 0.71f},
            {0.17f, 0.71f},
            {0.29f, 0.71f},
            {0.42f, 0.71f},
            {0.55f, 0.71f},
            {0.73f, 0.71f},
            {0.90f, 0.71f},
            // Level 4
            {0.17f, 0.91f},
            {0.42f, 0.91f},
            {0.73f, 0.91f},
        }};

        g.edges = {{
            // Level 0 -> Level 1
            {0, 1},
            {0, 2},
            {0, 3},
            // Level 1 -> Level 2
            {1, 4},
            {1, 5},
            {2, 6},
            {2, 7},
            {3, 8},
            {3, 9},
            // Level 2 -> Level 3
            {4, 10},
            {4, 11},
            {5, 12},
            {6, 13},
            {6, 14},
            {8, 15},
            {9, 16},
            // Level 3 -> Level 4
            {11, 17},
            {13, 18},
            {15, 19},
        }};

        for (const auto &edge : g.edges)
        {
            g.adjacency[edge[0]].push_back(edge[1]);
            g.adjacency[edge[1]].push_back(edge[0]);
        }

        for (auto &neighbors : g.adjacency)
        {
            std::sort(neighbors.begin(), neighbors.end());
        }

        return g;
    }();

    return graph;
}

struct TraversalState
{
    TraversalAlgorithm algorithm = TraversalAlgorithm::DFS;
    int currentNode = -1;
    int hoveredNode = -1;
    int suggestedParent = -1;

    std::array<bool, Graph20::NODE_COUNT> visited{};
    std::array<int, Graph20::NODE_COUNT> treeParent{};
    std::vector<int> visitHistory;
    std::deque<int> bfsQueue;
    std::vector<int> suggestedNeighbors;
};

inline void ResetTraversalState(TraversalState &state)
{
    state.currentNode = -1;
    state.suggestedParent = -1;
    state.visited.fill(false);
    state.treeParent.fill(-1);
    state.visitHistory.clear();
    state.bfsQueue.clear();
    state.suggestedNeighbors.clear();
}

inline int FindFirstUnvisitedNeighbor(const Graph20 &graph, const TraversalState &state, int node)
{
    for (int neighbor : graph.adjacency[node])
    {
        if (!state.visited[neighbor])
        {
            return neighbor;
        }
    }
    return -1;
}

inline void VisitNodeAndSuggest(TraversalState &state, int clickedNode)
{
    const Graph20 &graph = GetGraph20();

    if (!state.visited[clickedNode])
    {
        if (state.currentNode >= 0 &&
            std::find(graph.adjacency[state.currentNode].begin(),
                      graph.adjacency[state.currentNode].end(),
                      clickedNode) != graph.adjacency[state.currentNode].end())
        {
            state.treeParent[clickedNode] = state.currentNode;
        }
        state.visited[clickedNode] = true;
        state.visitHistory.push_back(clickedNode);
    }

    state.currentNode = clickedNode;
    state.suggestedNeighbors.clear();
    state.suggestedParent = -1;

    if (state.algorithm == TraversalAlgorithm::DFS)
    {
        // Depth-first: suggest the first unvisited neighbor of the clicked node.
        // If all direct neighbors are already visited, backtrack along visit history.
        const int directNext = FindFirstUnvisitedNeighbor(graph, state, clickedNode);
        if (directNext >= 0)
        {
            state.suggestedParent = clickedNode;
            state.suggestedNeighbors.push_back(directNext);
            return;
        }

        for (auto it = state.visitHistory.rbegin(); it != state.visitHistory.rend(); ++it)
        {
            const int candidate = FindFirstUnvisitedNeighbor(graph, state, *it);
            if (candidate >= 0)
            {
                state.suggestedParent = *it;
                state.suggestedNeighbors.push_back(candidate);
                return;
            }
        }
    }
    else
    {
        // Breadth-first: remove clicked node from queue, enqueue its unvisited neighbors,
        // and highlight all unvisited neighbors of the clicked node (or fallback to queue).
        state.bfsQueue.erase(
            std::remove(state.bfsQueue.begin(), state.bfsQueue.end(), clickedNode),
            state.bfsQueue.end());

        for (int neighbor : graph.adjacency[clickedNode])
        {
            if (!state.visited[neighbor])
            {
                state.suggestedNeighbors.push_back(neighbor);
                if (std::find(state.bfsQueue.begin(), state.bfsQueue.end(), neighbor) == state.bfsQueue.end())
                {
                    state.bfsQueue.push_back(neighbor);
                }
            }
        }

        if (!state.suggestedNeighbors.empty())
        {
            state.suggestedParent = clickedNode;
            return;
        }

        // If clicked node has no unvisited neighbors, suggest the next unvisited node in the BFS queue.
        while (!state.bfsQueue.empty() && state.visited[state.bfsQueue.front()])
        {
            state.bfsQueue.pop_front();
        }
        if (!state.bfsQueue.empty())
        {
            state.suggestedNeighbors.push_back(state.bfsQueue.front());
        }
    }
}

inline void DrawSuggestedEdgeArrow(Vector2 from, Vector2 to, float nodeRadius, Color color)
{
    const float dx = to.x - from.x;
    const float dy = to.y - from.y;
    const float len = std::sqrt(dx * dx + dy * dy);
    if (len < 1.0f)
    {
        return;
    }

    const float ux = dx / len;
    const float uy = dy / len;
    const Vector2 tip = {to.x - ux * (nodeRadius + 2.0f), to.y - uy * (nodeRadius + 2.0f)};

    DrawLineEx(from, to, 4.5f, color);

    const float arrowSize = std::clamp(nodeRadius * 0.50f, 7.0f, 11.0f);
    const Vector2 baseCenter = {tip.x - ux * arrowSize, tip.y - uy * arrowSize};
    const Vector2 perp = {-uy * (arrowSize * 0.55f), ux * (arrowSize * 0.55f)};

    if (len > nodeRadius * 2.4f)
    {
        const Vector2 p1 = tip;
        const Vector2 p2 = {baseCenter.x + perp.x, baseCenter.y + perp.y};
        const Vector2 p3 = {baseCenter.x - perp.x, baseCenter.y - perp.y};
        DrawTriangle(p1, p3, p2, color);
        DrawTriangle(p1, p2, p3, color);
    }
}

inline std::string FormatTraversalPath(const TraversalState &state)
{
    const char *algoName = (state.algorithm == TraversalAlgorithm::DFS) ? "DFS" : "BFS";
    const std::string header = std::string("Camino ") + algoName + " (" +
                               std::to_string(state.visitHistory.size()) + "/" +
                               std::to_string(Graph20::NODE_COUNT) + "): ";

    if (state.visitHistory.empty())
    {
        return header + "sin nodos visitados";
    }

    std::string result = header;
    for (std::size_t i = 0; i < state.visitHistory.size(); i++)
    {
        if (i > 0)
        {
            result += " -> ";
        }
        result += std::to_string(state.visitHistory[i]);
    }
    return result;
}

inline std::vector<std::string> BuildTraversalPathLines(
    const TraversalState &state,
    float maxLineWidth,
    int fontSize)
{
    const char *algoName = (state.algorithm == TraversalAlgorithm::DFS) ? "DFS" : "BFS";
    const std::string header = std::string("Camino ") + algoName + " (" +
                               std::to_string(state.visitHistory.size()) + "/" +
                               std::to_string(Graph20::NODE_COUNT) + "): ";

    if (state.visitHistory.empty())
    {
        return {header + "sin nodos visitados"};
    }

    std::vector<std::string> lines;
    std::string currentLine = header;
    for (std::size_t i = 0; i < state.visitHistory.size(); i++)
    {
        const std::string token =
            std::to_string(state.visitHistory[i]) + (i + 1 < state.visitHistory.size() ? " -> " : "");
        const std::string candidate = currentLine + token;
        if (currentLine != header &&
            static_cast<float>(MeasureText(candidate.c_str(), fontSize)) > maxLineWidth)
        {
            lines.push_back(currentLine);
            currentLine = token;
        }
        else
        {
            currentLine = candidate;
        }
    }
    if (!currentLine.empty())
    {
        lines.push_back(currentLine);
    }
    return lines;
}

inline void DrawTraversalPathBar(
    const Rectangle &barRect,
    const std::vector<std::string> &lines,
    int fontSize,
    float lineHeight,
    bool hasVisits)
{
    constexpr Color barBackground = {248, 250, 252, 255};
    constexpr Color barBorder = {203, 213, 225, 255};
    constexpr Color activeTextColor = {30, 41, 59, 255};
    constexpr Color emptyTextColor = {100, 116, 139, 255};

    DrawRectangleRec(barRect, barBackground);
    DrawLine(
        static_cast<int>(barRect.x),
        static_cast<int>(barRect.y + barRect.height),
        static_cast<int>(barRect.x + barRect.width),
        static_cast<int>(barRect.y + barRect.height),
        barBorder);

    const float totalTextHeight = static_cast<float>(lines.size()) * lineHeight;
    float y = barRect.y + (barRect.height - totalTextHeight) * 0.5f;
    const Color textColor = hasVisits ? activeTextColor : emptyTextColor;

    for (const std::string &line : lines)
    {
        const int lineWidth = MeasureText(line.c_str(), fontSize);
        const int x = static_cast<int>(
            barRect.x + std::max(16.0f, (barRect.width - static_cast<float>(lineWidth)) * 0.5f));
        DrawText(line.c_str(), x, static_cast<int>(y + (lineHeight - static_cast<float>(fontSize)) * 0.5f), fontSize, textColor);
        y += lineHeight;
    }
}

inline void ComputeGraphLayout(
    const Rectangle &canvasArea,
    const Graph20 &graph,
    float &nodeRadius,
    std::array<Vector2, Graph20::NODE_COUNT> &screenPositions)
{
    const float paddingX = std::clamp(canvasArea.width * 0.08f, 36.0f, 84.0f);
    const float paddingY = std::clamp(canvasArea.height * 0.08f, 20.0f, 46.0f);
    const Rectangle graphArea = {
        canvasArea.x + paddingX,
        canvasArea.y + paddingY,
        std::max(160.0f, canvasArea.width - paddingX * 2.0f),
        std::max(160.0f, canvasArea.height - paddingY * 2.0f)};

    const float minGraphDim = std::min(graphArea.width, graphArea.height);
    nodeRadius = std::clamp(minGraphDim * 0.052f, 15.0f, 24.0f);

    for (int i = 0; i < Graph20::NODE_COUNT; i++)
    {
        screenPositions[i] = Vector2{
            graphArea.x + graph.normalizedPositions[i].x * graphArea.width,
            graphArea.y + graph.normalizedPositions[i].y * graphArea.height};
    }
}

inline std::string BuildSelectedNodeFooterText(const Graph20 &graph, int selectedNode)
{
    if (selectedNode < 0 || selectedNode >= Graph20::NODE_COUNT)
    {
        return "Selecciona un vertice para ver su clave, grado y vecinos proximos";
    }

    const std::vector<int> &neighbors = graph.adjacency[selectedNode];
    std::string neighborsText;
    for (std::size_t i = 0; i < neighbors.size(); i++)
    {
        if (i > 0)
        {
            neighborsText += ", ";
        }
        neighborsText += std::to_string(neighbors[i]);
    }
    if (neighborsText.empty())
    {
        neighborsText = "ninguno";
    }

    const char *neighborLabel = (neighbors.size() == 1) ? "Vecino proximo" : "Vecinos proximos";
    return "Clave del vertice: " + std::to_string(selectedNode) +
           "  |  Grado: " + std::to_string(neighbors.size()) +
           "  |  " + neighborLabel + ": " + neighborsText;
}

inline void DrawSelectedNodeFooter(const Rectangle &footerRect, const Graph20 &graph, int selectedNode)
{
    constexpr Color barBackground = {248, 250, 252, 255};
    constexpr Color barBorder = {203, 213, 225, 255};
    constexpr Color activeTextColor = {30, 41, 59, 255};
    constexpr Color emptyTextColor = {100, 116, 139, 255};
    constexpr Color badgeFill = {249, 115, 22, 255};
    constexpr Color badgeBorder = {154, 52, 18, 255};

    DrawRectangleRec(footerRect, barBackground);
    DrawLine(
        static_cast<int>(footerRect.x),
        static_cast<int>(footerRect.y),
        static_cast<int>(footerRect.x + footerRect.width),
        static_cast<int>(footerRect.y),
        barBorder);

    const std::string text = BuildSelectedNodeFooterText(graph, selectedNode);
    const bool hasSelection = (selectedNode >= 0 && selectedNode < Graph20::NODE_COUNT);

    int fontSize = 15;
    int textWidth = MeasureText(text.c_str(), fontSize);
    const float maxAllowedWidth = std::max(80.0f, footerRect.width - 40.0f);
    if (static_cast<float>(textWidth) > maxAllowedWidth)
    {
        fontSize = 13;
        textWidth = MeasureText(text.c_str(), fontSize);
    }

    const int badgeSpan = hasSelection ? 18 : 0;
    const int totalWidth = badgeSpan + textWidth;
    const int startX = static_cast<int>(
        footerRect.x + std::max(12.0f, (footerRect.width - static_cast<float>(totalWidth)) * 0.5f));
    const int textY = static_cast<int>(footerRect.y + (footerRect.height - static_cast<float>(fontSize)) * 0.5f);

    if (hasSelection)
    {
        const Vector2 badgeCenter = {
            static_cast<float>(startX) + 5.0f,
            footerRect.y + footerRect.height * 0.5f};
        DrawCircleV(badgeCenter, 5.0f, badgeFill);
        DrawCircleLinesV(badgeCenter, 5.0f, badgeBorder);
    }

    DrawText(
        text.c_str(),
        startX + badgeSpan,
        textY,
        fontSize,
        hasSelection ? activeTextColor : emptyTextColor);
}

inline void RenderTraversalExercise(const Rectangle &contentArea, TraversalState &state)
{
    const Graph20 &graph = GetGraph20();

    constexpr int pathFontSize = 14;
    constexpr float pathLineHeight = 18.0f;
    constexpr float pathVerticalPad = 7.0f;
    constexpr float infoBarHeight = 32.0f;
    const float maxPathLineWidth = std::max(120.0f, contentArea.width - 32.0f);

    std::vector<std::string> pathLines = BuildTraversalPathLines(state, maxPathLineWidth, pathFontSize);
    float pathBarHeight = pathVerticalPad * 2.0f + static_cast<float>(pathLines.size()) * pathLineHeight;
    Rectangle pathBarRect = {
        contentArea.x,
        contentArea.y,
        contentArea.width,
        pathBarHeight};
    Rectangle canvasArea = {
        contentArea.x,
        contentArea.y + pathBarHeight,
        contentArea.width,
        std::max(160.0f, contentArea.height - pathBarHeight - infoBarHeight)};
    const Rectangle infoBarRect = {
        contentArea.x,
        contentArea.y + contentArea.height - infoBarHeight,
        contentArea.width,
        infoBarHeight};

    float nodeRadius = 18.0f;
    std::array<Vector2, Graph20::NODE_COUNT> screenPositions{};
    ComputeGraphLayout(canvasArea, graph, nodeRadius, screenPositions);

    const Vector2 mousePos = GetMousePosition();
    state.hoveredNode = -1;
    if (CheckCollisionPointRec(mousePos, canvasArea))
    {
        for (int i = 0; i < Graph20::NODE_COUNT; i++)
        {
            if (CheckCollisionPointCircle(mousePos, screenPositions[i], nodeRadius + 4.0f))
            {
                state.hoveredNode = i;
                break;
            }
        }
    }

    bool historyChanged = false;
    const std::size_t previousHistorySize = state.visitHistory.size();
    if (state.hoveredNode >= 0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        VisitNodeAndSuggest(state, state.hoveredNode);
        historyChanged = (state.visitHistory.size() != previousHistorySize);
    }

    if (IsKeyPressed(KEY_R) ||
        (CheckCollisionPointRec(mousePos, contentArea) && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)))
    {
        ResetTraversalState(state);
        historyChanged = (state.visitHistory.size() != previousHistorySize);
    }

    if (historyChanged)
    {
        pathLines = BuildTraversalPathLines(state, maxPathLineWidth, pathFontSize);
        pathBarHeight = pathVerticalPad * 2.0f + static_cast<float>(pathLines.size()) * pathLineHeight;
        pathBarRect.height = pathBarHeight;
        canvasArea.y = contentArea.y + pathBarHeight;
        canvasArea.height = std::max(160.0f, contentArea.height - pathBarHeight - infoBarHeight);
        ComputeGraphLayout(canvasArea, graph, nodeRadius, screenPositions);
    }

    constexpr Color currentFill = {249, 115, 22, 255};
    constexpr Color currentBorder = {154, 52, 18, 255};
    constexpr Color suggestedFill = {34, 197, 94, 255};
    constexpr Color suggestedBorder = {21, 128, 61, 255};
    constexpr Color visitedFill = {124, 58, 237, 255};
    constexpr Color visitedBorder = {76, 29, 149, 255};

    // Draw base & traversed edges
    for (const auto &edge : graph.edges)
    {
        const int u = edge[0];
        const int v = edge[1];
        const bool isTreeEdge =
            (state.treeParent[u] == v || state.treeParent[v] == u) &&
            state.visited[u] && state.visited[v];

        if (isTreeEdge)
        {
            DrawLineEx(screenPositions[u], screenPositions[v], 3.5f, Color{139, 92, 246, 200});
        }
        else
        {
            DrawLineEx(screenPositions[u], screenPositions[v], 2.5f, Color{185, 195, 210, 255});
        }
    }

    // Highlight edges to suggested neighbors
    if (state.suggestedParent >= 0)
    {
        for (int target : state.suggestedNeighbors)
        {
            DrawSuggestedEdgeArrow(
                screenPositions[state.suggestedParent],
                screenPositions[target],
                nodeRadius,
                suggestedFill);
        }
    }

    // Draw nodes (0 to 19)
    const int nodeFontSize = std::clamp(static_cast<int>(nodeRadius * 0.85f), 13, 20);
    for (int i = 0; i < Graph20::NODE_COUNT; i++)
    {
        const bool isCurrent = (i == state.currentNode);
        const bool isSuggested =
            std::find(state.suggestedNeighbors.begin(), state.suggestedNeighbors.end(), i) !=
            state.suggestedNeighbors.end();
        const bool isVisited = state.visited[i] && !isCurrent;
        const bool isHovered = (i == state.hoveredNode);

        Color fill = SKYBLUE;
        Color border = DARKBLUE;
        Color textColor = BLACK;

        if (isCurrent)
        {
            fill = currentFill;
            border = currentBorder;
            textColor = WHITE;
        }
        else if (isSuggested)
        {
            fill = suggestedFill;
            border = suggestedBorder;
            textColor = WHITE;
        }
        else if (isVisited)
        {
            fill = visitedFill;
            border = visitedBorder;
            textColor = WHITE;
        }

        if (isHovered)
        {
            DrawCircleV(screenPositions[i], nodeRadius + 4.5f, Color{59, 130, 246, 90});
        }

        DrawCircleV(screenPositions[i], nodeRadius, fill);
        DrawCircleLinesV(screenPositions[i], nodeRadius, border);
        if (isCurrent || isSuggested)
        {
            DrawCircleLinesV(screenPositions[i], nodeRadius - 1.5f, border);
        }

        const char *label = TextFormat("%d", i);
        const int labelWidth = MeasureText(label, nodeFontSize);
        DrawText(
            label,
            static_cast<int>(screenPositions[i].x) - labelWidth / 2,
            static_cast<int>(screenPositions[i].y) - nodeFontSize / 2,
            nodeFontSize,
            textColor);
    }

    DrawSelectedNodeFooter(infoBarRect, graph, state.currentNode);
}

#endif
