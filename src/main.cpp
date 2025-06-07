#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <functional>

#include "timing.hpp"

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
        if ((arr[i] & 0x3E8) == 0) {  // Редкое условие
            sum_rare += arr[i];
        } else {              // Частое условие
            sum_common += arr[i];
        }
    }
}

// // Включим макросы для подсказок ветвления (если компилятор поддерживает)
// #if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 800)) || defined(__clang__)
// #  define expect(expr,value)    (__builtin_expect ((expr),(value)) )
// #else
// #  define expect(expr,value)    (expr)
// #endif

// #ifndef likely
// #define likely(expr)     expect((expr) != 0, 1)
// #endif
// #ifndef unlikely
// #define unlikely(expr)   expect((expr) != 0, 0)
// #endif

// Переопределенная функция с оптимизацией ветвлений
void calculation_optimized(const std::vector<int>& arr, int& sum_rare, int& sum_common) {
    sum_rare = 0;
    sum_common = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        if ((arr[i] & 0x3E8) == 0) [[unlikely]] {  // Подсказываем, что это маловероятно
            sum_rare += arr[i];
        } else [[likely]] {                // Основная ветка
            sum_common += arr[i];
        }
    }
}

// Переопределенная функция с НЕправильной оптимизацией ветвлений
void calculation_misoptimized(const std::vector<int>& arr, int& sum_rare, int& sum_common) {
    sum_rare = 0;
    sum_common = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        if ((arr[i] & 0x3E8) == 0) [[likely]] {  // ОШИБКА: говорим, что это вероятно (хотя на самом деле нет)
            sum_rare += arr[i];
        } else [[unlikely]] {
            sum_common += arr[i];
        }
    }
}

/**
* @brief Тестирует производительность функции, вызывая её multiple_times раз.
* @param func Функция для тестирования (обёрнутая в std::function).
* @param multiple_times Количество повторений.
* @return Среднее время выполнения одного вызова в секундах.
*/
double measure_performance(
        std::function<void(const std::vector<int>&, int&, int&)> calculation,
        size_t array_size,
        int statistics_num
    ) {

    std::chrono::duration<double> total_elapsed;
    
    for (int i = 0; i < statistics_num; i++){

        auto arr = generate_random_array(array_size);
        int sum_rare, sum_common;

        auto start = std::chrono::high_resolution_clock::now();
        calculation(arr, sum_rare, sum_common);
        auto end = std::chrono::high_resolution_clock::now();

        total_elapsed += end - start;
    }
    
    return double(total_elapsed.count() / statistics_num);
}

int measure_and_write (
    std::function<void(const std::vector<int>&, int&, int&)> calculation,
    size_t array_size,
    int statistics_num,
    const std::string name,
    const std::string keyword
) {

    std::cout << "Измеряется" << name << keyword << "...\n";

    return write_time_to_csv(
        name,
        keyword,
        measure_performance(calculation, array_size, statistics_num)
    );
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

    measure_and_write(
        calculation_plain,
        array_size,
        statistics_num,
        "calculaton",
        "plain"
    );

    measure_and_write(
        calculation_optimized,
        array_size,
        statistics_num,
        "calculaton",
        "optimized"
    );

    measure_and_write(
        calculation_misoptimized,
        array_size,
        statistics_num,
        "calculaton",
        "misoptimized"
    );

    return 0;
}