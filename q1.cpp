#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

void applyBoxBlur(const vector<vector<int>>& in, vector<vector<int>>& out, int rows, int cols) 
{
#pragma omp parallel for
for (int i = 1; i < rows - 1; i++) {
for (int j = 1; j < cols - 1; j++) {
int sum = 0;
for (int di = -1; di <= 1; di++) {
for (int dj = -1; dj <= 1; dj++) {
sum += in[i + di][j + dj];
}
}
out[i][j] = sum / 9;
}
}
}

int main() 
{
vector<int> sizes = {1000, 2000, 3000, 5000};
omp_set_dynamic(0);

for (int size : sizes) {
vector<vector<int>> image(size, vector<int>(size, 128));
vector<vector<int>> output(size, vector<int>(size, 0));
cout << "Processing " << size << "x" << size << " image...\n";

auto start = high_resolution_clock::now();
applyBoxBlur(image, output, size, size);
auto end = high_resolution_clock::now();
cout << "1 thread (serial): " << duration<double>(end - start).count() << " sec\n";

for (int threads : {2, 4, 8, 12, 16}) {
omp_set_num_threads(threads);
start = high_resolution_clock::now();
applyBoxBlur(image, output, size, size);
end = high_resolution_clock::now();
cout << threads << " threads: " << duration<double>(end - start).count() << " sec\n";
}
cout << "_________________________________________________________________\n";
}
return 0;
}

