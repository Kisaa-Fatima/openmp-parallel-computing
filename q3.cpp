#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <omp.h>

using namespace std;
using namespace std::chrono;

// Grid and simulation parameters
static const int ROWS = 100, COLS = 100, GENERATIONS = 100;
static const char DEAD = '.', LIVE = '*';
static const int CELL_SIZE = 6;

// Count live neighbors using toroidal boundaries
int countLiveNeighbors(const vector<vector<char>>& grid, int r, int c) {
int count = 0;
for (int dr = -1; dr <= 1; dr++)
for (int dc = -1; dc <= 1; dc++) {
if (dr == 0 && dc == 0) continue;
int rr = (r + dr + ROWS) % ROWS;
int cc = (c + dc + COLS) % COLS;
if (grid[rr][cc] == LIVE) count++;
}
return count;
}

// Determine the next state of a cell
char getNextState(const vector<vector<char>>& grid, int r, int c) {
int ln = countLiveNeighbors(grid, r, c);
if (grid[r][c] == LIVE)
return (ln < 2 || ln > 3) ? DEAD : LIVE;
return (ln == 3) ? LIVE : DEAD;
}

// Initialize the grid with all dead cells except a centered 10x10 block of live cells
void initializeGrid(vector<vector<char>>& grid) 
{
for (int r = 0; r < ROWS; r++)
for (int c = 0; c < COLS; c++)
grid[r][c] = DEAD;
int startR = (ROWS - 10) / 2, startC = (COLS - 10) / 2;
for (int r = 0; r < 10; r++)
for (int c = 0; c < 10; c++)
grid[startR + r][startC + c] = LIVE;
}

// Serial update of the grid
void updateGridSerial(vector<vector<char>>& grid) {
vector<vector<char>> newGrid = grid;
for (int r = 0; r < ROWS; r++)
for (int c = 0; c < COLS; c++)
newGrid[r][c] = getNextState(grid, r, c);
grid = newGrid;
}

// Visualization mode using SFML
void runVisualization() {
int windowWidth = COLS * CELL_SIZE, windowHeight = ROWS * CELL_SIZE;
sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Conway's Game of Life");
window.setFramerateLimit(30);

vector<vector<char>> grid(ROWS, vector<char>(COLS, DEAD));
initializeGrid(grid);
sf::RectangleShape cellShape(sf::Vector2f((float)CELL_SIZE, (float)CELL_SIZE));
int generationCount = 0;

while (window.isOpen()) {
sf::Event event;
while (window.pollEvent(event))
if (event.type == sf::Event::Closed)
window.close();

window.clear(sf::Color::Black);
for (int r = 0; r < ROWS; r++) {
for (int c = 0; c < COLS; c++) {
cellShape.setFillColor((grid[r][c] == LIVE) ? sf::Color::White : sf::Color::Black);
cellShape.setPosition((float)(c * CELL_SIZE), (float)(r * CELL_SIZE));
window.draw(cellShape);
}
}
window.display();

if (generationCount < GENERATIONS) {
updateGridSerial(grid);
generationCount++;
}
this_thread::sleep_for(chrono::milliseconds(200));
}
}

// Performance Analysis Mode

// 1) Serial version
void runSerial() {
vector<vector<char>> grid(ROWS, vector<char>(COLS, DEAD));
initializeGrid(grid);
for (int gen = 0; gen < GENERATIONS; gen++) {
vector<vector<char>> newGrid = grid;
for (int r = 0; r < ROWS; r++)
for (int c = 0; c < COLS; c++)
newGrid[r][c] = getNextState(grid, r, c);
grid = newGrid;
}
}

// 2) Parallel version with static scheduling (chunk size = 1)
void runParallelStatic() {
vector<vector<char>> grid(ROWS, vector<char>(COLS, DEAD));
initializeGrid(grid);
for (int gen = 0; gen < GENERATIONS; gen++) {
vector<vector<char>> newGrid = grid;
#pragma omp parallel for schedule(static,1)
for (int r = 0; r < ROWS; r++) {
for (int c = 0; c < COLS; c++)
newGrid[r][c] = getNextState(grid, r, c);
}
grid = newGrid;
}
}

// 3) Parallel version with guided scheduling (chunk size = 1)
void runParallelGuided() {
vector<vector<char>> grid(ROWS, vector<char>(COLS, DEAD));
initializeGrid(grid);
for (int gen = 0; gen < GENERATIONS; gen++) {
vector<vector<char>> newGrid = grid;
#pragma omp parallel for schedule(guided,1)
for (int r = 0; r < ROWS; r++) {
for (int c = 0; c < COLS; c++)
newGrid[r][c] = getNextState(grid, r, c);
}
grid = newGrid;
}
}

void runPerformanceAnalysis() {
const int RUNS = 5;
double totalSerial = 0.0, totalStatic = 0.0, totalGuided = 0.0;

for (int i = 0; i < RUNS; i++) {
auto start = high_resolution_clock::now();
runSerial();
auto end = high_resolution_clock::now();
totalSerial += duration<double>(end - start).count();
}
double avgSerial = totalSerial / RUNS;

for (int i = 0; i < RUNS; i++) {
auto start = high_resolution_clock::now();
runParallelStatic();
auto end = high_resolution_clock::now();
totalStatic += duration<double>(end - start).count();
}
double avgStatic = totalStatic / RUNS;

for (int i = 0; i < RUNS; i++) {
auto start = high_resolution_clock::now();
runParallelGuided();
auto end = high_resolution_clock::now();
totalGuided += duration<double>(end - start).count();
}
double avgGuided = totalGuided / RUNS;

double speedupStatic = avgSerial / avgStatic;
double speedupGuided = avgSerial / avgGuided;

cout << "===== Performance Analysis (100x100, 100 Gens) =====\n";
cout << "Serial (avg " << RUNS << " runs):  " << avgSerial  << " s\n";
cout << "Static (avg " << RUNS << " runs):  " << avgStatic  << " s   | Speedup: " << speedupStatic << "\n";
cout << "Guided (avg " << RUNS << " runs):  " << avgGuided   << " s   | Speedup: " << speedupGuided << "\n";
cout << "===== END =====\n\n";
}

int main(int argc, char* argv[]) 
{
// Run performance analysis if "perf" is passed; otherwise run the SFML visualization
if (argc > 1 && string(argv[1]) == "perf")
runPerformanceAnalysis();
else
runVisualization();
return 0;
}

