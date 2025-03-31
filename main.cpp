#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>

struct QuadraticEquation {
    double a, b, c;
    double root1 = std::numeric_limits<double>::quiet_NaN();
    double root2 = std::numeric_limits<double>::quiet_NaN();
    QuadraticEquation* next = nullptr;
};

void solveEquations(QuadraticEquation* head) {
    QuadraticEquation* current = head;
    while (current) {
        double discriminant = current->b * current->b - 4 * current->a * current->c;
        if (discriminant > 0) {
            current->root1 = (-current->b + std::sqrt(discriminant)) / (2 * current->a);
            current->root2 = (-current->b - std::sqrt(discriminant)) / (2 * current->a);
        }
        else if (discriminant == 0) {
            current->root1 = current->root2 = -current->b / (2 * current->a);
        }
        current = current->next;
    }
}

QuadraticEquation* readEquationsFromFile(const std::string& filename) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return nullptr;
    }

    QuadraticEquation* head = nullptr;
    QuadraticEquation* tail = nullptr;
    std::string line;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        double a, b, c;
        if (!(iss >> a >> b >> c)) {
            std::cerr << "Некорректная строка: " << line << std::endl;
            continue;
        }
        if (a == 0) {
            std::cerr << "Некорректное уравнение: a не может быть 0." << std::endl;
            continue;
        }
        QuadraticEquation* newEq = new QuadraticEquation{ a, b, c };
        if (!head) {
            head = tail = newEq;
        }
        else {
            tail->next = newEq;
            tail = newEq;
        }
    }

    inputFile.close();
    return head;
}

void printEquations(QuadraticEquation* head) {
    QuadraticEquation* current = head;
    while (current) {
        std::cout << "Уравнение: " << current->a << "x^2 + " << current->b << "x + " << current->c << " = 0\n";
        if (!std::isnan(current->root1) && !std::isnan(current->root2)) {
            std::cout << "Корни: " << current->root1 << " и " << current->root2 << "\n";
        }
        else {
            std::cout << "Нет вещественных корней.\n";
        }
        std::cout << "------------------\n";
        current = current->next;
    }
}

void deleteEquations(QuadraticEquation* head) {
    while (head) {
        QuadraticEquation* temp = head;
        head = head->next;
        delete temp;
    }
}

int main() {
    QuadraticEquation* equations = readEquationsFromFile("equations.txt");
    if (!equations) return 1;

    solveEquations(equations);
    printEquations(equations);
    deleteEquations(equations);

    return 0;
}
