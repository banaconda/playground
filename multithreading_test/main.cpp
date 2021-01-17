#include <iostream>
#include <thread>
#include <vector>
#include <time.h>

using namespace std;

int
main(int argc, char **argv)
{
    const auto processor_count = std::thread::hardware_concurrency();
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

    clock_t start = clock();
    vector<thread> threads(n);
    for (int i = 0; i < n; i++) {
        threads[i] = thread(f, i, 1000);
    }

    for (int i = 0; i < n; i++) {
        threads[i].join();
    }

    clock_t end = clock();
    cout << "done. it took "
         << (((float)(end - start)) / CLOCKS_PER_SEC) /
                ((n <= processor_count) ? n : processor_count)
         << " seconds" << endl;
    return 0;
}
