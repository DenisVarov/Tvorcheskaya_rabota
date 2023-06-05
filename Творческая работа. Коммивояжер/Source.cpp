#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <limits>
#include <sstream>
#include <Windows.h>

using namespace std;
using namespace sf;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int VERTEX_RADIUS = 20;

Font font;
vector<Vector2f> vertices;
vector<vector<int>> adjacencyMatrix;

void generateVertices(int numVertices) {
    vertices.clear();
    adjacencyMatrix.clear();
    adjacencyMatrix.resize(numVertices, vector<int>(numVertices, 0));

    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < numVertices; ++i) {
        int x = rand() % (WINDOW_WIDTH - 2 * VERTEX_RADIUS) + VERTEX_RADIUS;
        int y = rand() % (WINDOW_HEIGHT - 2 * VERTEX_RADIUS) + VERTEX_RADIUS;
        vertices.push_back(Vector2f(x, y));
    }
}


void generateEdges() {
    srand(static_cast<unsigned int>(time(nullptr)));

    int numVertices = vertices.size();

    for (int i = 0; i < numVertices; ++i) {
        for (int j = i + 1; j < numVertices; ++j) {
            int weight = rand() % 100 + 1;
            adjacencyMatrix[i][j] = weight;
            adjacencyMatrix[j][i] = weight;
        }
    }
}



void drawVerticesAndEdges(RenderWindow& window) {
    window.clear(Color(220, 220, 220));

    // Определение минимального расстояния между вершинами для предотвращения их наложения
    float minDistance = 4 * VERTEX_RADIUS;

    // Рисование ребер
    for (int i = 0; i < vertices.size(); ++i) {
        for (int j = i + 1; j < vertices.size(); ++j) {
            if (adjacencyMatrix[i][j] != 0) {
                Vector2f point1 = vertices[i];
                Vector2f point2 = vertices[j];

                // Проверка и предотвращение наложения вершин и весов ребер
                if (hypot(point2.x - point1.x, point2.y - point1.y) < minDistance) {
                    Vector2f direction = point2 - point1;
                    float length = hypot(direction.x, direction.y);
                    direction /= length;
                    point2 = point1 + direction * minDistance;
                }



                Vertex line[] = {
                    Vertex(point1, Color::Black),
                    Vertex(point2, Color::Black)
                };
                window.draw(line, 2, Lines);

                // Рисование веса ребра
                int weight = adjacencyMatrix[i][j];
                Vector2f weightPosition = (point1 + point2) / 2.0f + Vector2f(0, -15);
                Text weightText(to_string(weight), font, 12);
                weightText.setFillColor(Color::Black);
                FloatRect textRect = weightText.getLocalBounds();
                weightText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                weightText.setPosition(weightPosition);
                window.draw(weightText);

                // Обновление позиции вершины, чтобы избежать наложения
                vertices[j] = point2;
            }
        }
    }

    // Рисование вершин
    for (int i = 0; i < vertices.size(); ++i) {
        CircleShape vertexShape(VERTEX_RADIUS);
        vertexShape.setFillColor(Color::White);
        vertexShape.setOutlineThickness(2);
        vertexShape.setOutlineColor(Color::Black);
        vertexShape.setPosition(vertices[i] - Vector2f(VERTEX_RADIUS, VERTEX_RADIUS));

        // Рисование номера вершины
        int vertexNumber = i + 1;
        Vector2f numberPosition = vertices[i] - Vector2f(8, 8);
        Text numberText(to_string(vertexNumber), font, 16);
        numberText.setFillColor(Color::Black);
        numberText.setPosition(numberPosition);

        window.draw(vertexShape);
        window.draw(numberText);
    }

    window.display();
}


void branchAndBoundAlgorithm(const vector<vector<int>>& graph, vector<int>& path, vector<bool>& visited, int currentVertex, int currentDistance, int& minDistance, vector<int>& minPath)
{
    int numVertices = graph.size();
    visited[currentVertex] = true;
    path.push_back(currentVertex);

    if (path.size() == numVertices)
    {
        currentDistance += graph[currentVertex][0];
        if (currentDistance < minDistance)
        {
            minDistance = currentDistance;
            minPath = path;
        }
    }
    else
    {
        vector<pair<int, int>> candidates;
        for (int nextVertex = 0; nextVertex < numVertices; ++nextVertex)
        {
            if (!visited[nextVertex])
            {
                int edgeWeight = graph[currentVertex][nextVertex];
                if (edgeWeight != 0)
                {
                    candidates.emplace_back(nextVertex, edgeWeight);
                }
            }
        }


        sort(candidates.begin(), candidates.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
            return a.second < b.second;
            });

        for (const auto& candidate : candidates)
        {
            int nextVertex = candidate.first;
            int edgeWeight = candidate.second;

            currentDistance += edgeWeight;
            if (currentDistance + *min_element(graph[nextVertex].begin(), graph[nextVertex].end()) * (numVertices - path.size() - 1) < minDistance)
            {
                branchAndBoundAlgorithm(graph, path, visited, nextVertex, currentDistance, minDistance, minPath);
            }
            currentDistance -= edgeWeight;
        }
    }

    visited[currentVertex] = false;
    path.pop_back();
}



int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    srand(static_cast<unsigned int>(time(nullptr)));

    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Failed to load font." << endl;
        return 1;
    }

    int numVertices;
    cout << "Введите количество вершин: ";
    cin >> numVertices;

    generateVertices(numVertices);
    generateEdges();

    // Создаем переменные для хранения минимального пути и его длины
    int minDistance = INT_MAX;
    vector<int> minPath;
    vector<int> path;
    vector<bool> visited(numVertices, false);

    branchAndBoundAlgorithm(adjacencyMatrix, path, visited, 0, 0, minDistance, minPath);

    cout << "Минимальный путь: ";
    for (int vertex : minPath) {
        cout << vertex + 1 << " -> ";
    }
    cout << "1" << endl;
    cout << "Длина минимального пути: " << minDistance << endl;

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Граф");
    window.setFramerateLimit(60);

    wstring title = L"Граф";
    window.setTitle(title);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        drawVerticesAndEdges(window);
    }

    return 0;
}
