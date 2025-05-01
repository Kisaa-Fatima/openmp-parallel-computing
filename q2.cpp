#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Emoji definitions for our treasure hunt
const string TREASURE   = "🏆";
const string TRAP       = "T";
const string RESURRECTION = "✨";
const string DEADLY_TRAP = "☠️";
const string EMPTY      = ".";
const int TERMINAL_WIDTH = 100;

// Adventurer info
struct Adventurer 
{
int x, y;
int score = 0;
bool active = true;
};

// Display the grid center-aligned with colors for a bit of artistic flair
void printGrid(const vector<vector<string>>& grid, const vector<Adventurer>& adventurers) 
{
auto tempGrid = grid;
for (const auto& adv : adventurers)
if (adv.active)
tempGrid[adv.x][adv.y] = "A";

int gridWidth = grid.size() * 3;
int padding = max(0, (TERMINAL_WIDTH - gridWidth) / 2);

#pragma omp single
{
cout << "\n" << string(padding, ' ') << "+";
for (int i = 0; i < grid.size() *2+1; i++) 
cout << "-";
cout << "+\n";

for (const auto &row : tempGrid) {
cout << string(padding, ' ') << "| ";
for (const auto &cell : row) {
if (cell == TREASURE)
  cout << "\033[1;32m" << cell << " \033[0m"; // green
else if (cell == TRAP)
  cout << "\033[1;31m" << cell << " \033[0m"; // red
else if (cell == RESURRECTION)
  cout << "\033[1;34m" << cell << " \033[0m"; // blue
else if (cell == DEADLY_TRAP)
  cout << "\033[1;33m" << cell << " \033[0m"; // yellow
else if (cell == "A")
  cout << "\033[1;35m" << cell << " \033[0m"; // magenta
else
  cout << cell << " ";
}
cout << "|\n";
}

cout << string(padding, ' ') << "+";
for (int i = 0; i < grid.size() *2+1; i++) 
cout << "-";
cout << "+\n";
cout << "\n" << string(padding, ' ') << "🏆 Treasure Hunt Ongoing...\n";
}
}

// Set up our grid with treasures, traps, and surprises
void initializeGrid(vector<vector<string>>& grid, int N, int treasures, int traps, int resurrections, int deadlyTraps) {
srand(time(0));
for (int i = 0; i < N; i++)
for (int j = 0; j < N; j++)
grid[i][j] = EMPTY;

auto placeItem = [&](const string &item, int count) {
while (count > 0) {
int x = rand() % N, y = rand() % N;
if (grid[x][y] == EMPTY) {
grid[x][y] = item;
count--;
}
}
};

placeItem(TREASURE, treasures);
placeItem(TRAP, traps);
placeItem(RESURRECTION, resurrections);
placeItem(DEADLY_TRAP, deadlyTraps);
}

// Simulate our adventurous treasure hunt in a dynamic, parallel world
void simulateHunt(vector<vector<string>>& grid, int N, int T) {
vector<Adventurer> adventurers(T);
int highestScore = 0;

#pragma omp parallel for num_threads(T)
for (int i = 0; i < T; i++) {
adventurers[i].x = rand() % N;
adventurers[i].y = rand() % N;
}

bool ongoing = true;
while (ongoing) {
ongoing = false;
printGrid(grid, adventurers);

#pragma omp parallel for num_threads(T) shared(grid, adventurers, highestScore)
for (int i = 0; i < T; i++) {
if (!adventurers[i].active) continue;

int direction = rand() % 4;
int newX = adventurers[i].x, newY = adventurers[i].y;

if (direction == 0 && newX > 0)       newX--;
else if (direction == 1 && newX < N-1) newX++;
else if (direction == 2 && newY > 0)   newY--;
else if (direction == 3 && newY < N-1)   newY++;

if (grid[newX][newY] == TREASURE) {
adventurers[i].score += 10;
grid[newX][newY] = EMPTY;
} else if (grid[newX][newY] == TRAP) {
adventurers[i].score -= 5;
} else if (grid[newX][newY] == RESURRECTION) {
#pragma omp critical
{
  adventurers.push_back({rand() % N, rand() % N, 0, true});
  T++;
}
grid[newX][newY] = EMPTY;
} else if (grid[newX][newY] == DEADLY_TRAP) {
adventurers[i].active = false;
}

adventurers[i].x = newX;
adventurers[i].y = newY;
#pragma omp critical
highestScore = max(highestScore, adventurers[i].score);
#pragma omp atomic write
ongoing = true;
}
}

int winnerIndex = -1, maxScore = -1;
#pragma omp parallel for reduction(max:maxScore)
for (int i = 0; i < adventurers.size(); i++) {
if (adventurers[i].score > maxScore) {
maxScore = adventurers[i].score;
winnerIndex = i;
}
}
cout << "\n🎉 Winner is Adventurer " << winnerIndex << " with score: " << maxScore << "\n";
}

int main() {
int N, T;
cout << "Enter grid size (N): ";
cin >> N;
cout << "Enter number of adventurers (T): ";
cin >> T;

vector<vector<string>> grid(N, vector<string>(N, EMPTY));
// Distribute treasures, traps, and surprises artistically across the grid
initializeGrid(grid, N, N / 2, N / 4, N / 10, N / 8);
simulateHunt(grid, N, T);
return 0;
}

