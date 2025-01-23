// File: single_file_program.cpp
#include <iostream>
#include <fstream>
#include <string>
/**
 * @brief
 *
 * @param a
 * @param b
 * @return int, en summa av a och b
 */
int add(int a, int b)
{
    return a + b;
}
/**
 * @brief
 *
 * @param a
 * @param b
 * @return int, en differens av a och b
 */
int subtract(int a, int b)
{
    return a - b;
}
/**
 * @brief
 *
 * @param filename
 * @param text
 * @return void, skriver text till filen
 */
void writeToFile(const std::string &filename, const std::string &text)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << text;
        file.close();
    }
    else
    {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
    }
}
/**
 * @brief
 *
 * @param filename
 * @return std::string, läser innehållet från filen
 */
std::string readFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::string content;
    if (file.is_open())
    {
        std::getline(file, content);
        file.close();
    }
    else
    {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
    }
    return content;
}
/**
 * @brief
 *
 * @return int, main funktionen
 *
 */
int main()
{
    int x = 8, y = 3;
    std::cout << "Addition: " << add(x, y) << std::endl;
    std::cout << "Subtraction: " << subtract(x, y) << std::endl;

    std::string filename = "output.txt";
    writeToFile(filename, "Hello, single-file documentation!");
    std::cout << "File content: " << readFromFile(filename) << std::endl;

    return 0;
}
