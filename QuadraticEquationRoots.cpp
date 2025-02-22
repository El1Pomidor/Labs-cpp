#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>

void readCoefficients(const std::string& filename, double& a, double& b, double& c) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error when opening a file!" << std::endl;
        exit(1);
    }
    inputFile >> a >> b >> c;
    inputFile.close();
}

void solveEquation(double a, double b, double c) {
    if (a == 0) {
        if (b == 0) {
            std::cout << "The equation has no solutions" << std::endl;
        }
        else {
            std::cout << "It's a linear equation, one root :" << static_cast<double>(-c) / b << std::endl;
        }
        return;
    }

    double D = b * b - 4 * a * c;
    if (D > 0) {
        double x1 = (-b + std::sqrt(D)) / (2 * a);
        double x2 = (-b - std::sqrt(D)) / (2 * a);
        std::cout << "The roots of the equation: " << x1 << " и " << x2 << std::endl;
    }
    else if (D == 0) {
        double x = -b / (2 * a);
        std::cout << "One root: " << x << std::endl;
    }
    else {
        std::cout << "There are no real roots." << std::endl;
    }
}

int main() {
    double a, b, c;
    std::string filename = "input.txt";
    readCoefficients(filename, a, b, c);
    solveEquation(a, b, c);
    return 0;
}