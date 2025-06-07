#include <iostream>
#include <vector>
#include <chrono>
#include <random>

// Заполнение массива случайными числами
std::vector<int> generate_random_array(size_t size) {
    std::vector<int> arr(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    for (auto& num : arr) {
        num = dis(gen);
    }
    return arr;
}

// Вычисление сумм без оптимизации ветвлений
void calculation_plain(const std::vector<int>& arr, int& sum_rare, int& sum_common) {
    sum_rare = 0;
    sum_common = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i % 1000 == 0) {  // Редкое условие
            sum_rare += arr[i];
        } else {              // Частое условие
            sum_common += arr[i];
        }
    }
}


int main(const int argc, const char* const argv[]) {
    
    size_t array_size = 0;
    int statistics_num = 0;

    if (argc == 3) {
        array_size = atoi(argv[1]);
        statistics_num = atoi(argv[2]);
    } else if (argc == 2) {
        array_size = atoi(argv[1]);
        statistics_num = 10;
    } else {
        std::cerr << "Usage: " << argv[0] << " <array_size> [statistics_iterations_num]\n";
        return 1;
    }

    auto arr = generate_random_array(array_size);

    int sum_rare, sum_common;
    auto start = std::chrono::high_resolution_clock::now();
    calculation_plain(arr, sum_rare, sum_common);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Sum rare (every 1000th): " << sum_rare << "\n";   // чтобы результат был использован
    std::cout << "Sum common (others): " << sum_common << "\n";     // чтобы результат был использован
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";

    return 0;
}