#include <iostream>
#include <complex>
#include <chrono>
#include <fstream>

int write_time_to_csv(  
        const std::string name,
        const std::string keyword,
        const double vol
    ){
    const std::string csv_filename = "timings.csv";
    const std::string sep = ",";

    std::ofstream csv_file(csv_filename, std::ios::app);
    if (!csv_file.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return 1;
    }
    
    // Проверяем, пустой ли файл (чтобы добавить заголовок при первом вызове)
    csv_file.seekp(0, std::ios::end);
    bool is_empty = csv_file.tellp() == 0;
    if (is_empty) {
        csv_file << "Name" << sep << "Keyword" << sep << "Optimization" << sep << "Value\n";
    }

    csv_file << name << sep << keyword << sep << sep << vol << "\n";
    csv_file.close();

    return 0;
}