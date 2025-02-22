#include <iostream>
#include <fstream>
#include <cmath>
#include <string>


bool validateCoefficients(std::ifstream& inputFile, double& a, double& b, double& c) {
    if (!(inputFile >> a >> b >> c)) {
        std::cerr << "Error: incorrect data in the file" << std::endl;
        return false;
    }
    return true;
}

void readCoefficients(const std::string& filename, double& a, double& b, double& c) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "File opening error" << std::endl;
        exit(1);
    }
    if (!validateCoefficients(inputFile, a, b, c)) {
        exit(1);
    }
    inputFile.close();
}


std::string solveEquation(double a, double b, double c) {
    std::string result;
    if (a == 0) {
        if (b == 0) {
            result = "The equation has no solutions.";
        }
        else if (c == 0) {
            result = "It's a linear equation, one root: " + std::to_string(0);
        }
        else {
            result = "It's a linear equation, one root: " + std::to_string(-c / b);
        }
    }
    else {
        double D = b * b - 4 * a * c;
        if (D > 0) {
            double x1 = (-b + std::sqrt(D)) / (2.0 * a);
            double x2 = (-b - std::sqrt(D)) / (2.0 * a);
            result = "The roots of the equation: " + std::to_string(x1) + " and " + std::to_string(x2);
        }
        else if (D == 0) {
            double x = -b / (2.0 * a);
            result = "One root: " + std::to_string(x);
        }
        else {
            result = "There are no real roots.";
        }
    }
    return result;
}
void writeRootsToFile(const std::string& filename, const std::string& result) {
    std::ofstream outputFile(filename, std::ios::out);
    if (!outputFile) {
        std::cerr << "File creation error!" << std::endl;
        exit(1);
    }
    outputFile << result;
    outputFile.close();
}

int main() {
    double a, b, c;
    std::string filename = "koeffs.txt";

    readCoefficients(filename, a, b, c);
    std::string result = solveEquation(a, b, c);
    std::cout << result << std::endl;
    writeRootsToFile("roots.txt", result);
    return 0;
}