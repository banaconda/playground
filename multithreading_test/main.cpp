#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

using namespace std;

int
main(int argc, char **argv)
{
    int n = 32;
    if (argc == 1) {
    } else if (argc == 2) {
        n = atoi(argv[1]);
    } else {
        return 0;
    }

    cout << "test number of thread " << n << endl;

    auto f = [](int id, int n) {
        std::hash<int> h;

        for (int i = 0; i < n; i++) {
            for (int k = 0; k < n; k++) {
                for (int h = 0; h < n; h++) {
                    long int value = i + k + h;
                }
            }
        }
    };

    auto start = chrono::high_resolution_clock::now();
    vector<thread> threads(n);
    for (int i = 0; i < n; i++) {
        threads[i] = thread(f, i, 1000);
    }

    for (int i = 0; i < n; i++) {
        threads[i].join();
    }

    auto end = chrono::high_resolution_clock::now();
    auto nanosec = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    cout << "done. it took " << nanosec / 1000000000 << "." << nanosec % 1000000000 << " seconds" << endl;
    return 0;
}
