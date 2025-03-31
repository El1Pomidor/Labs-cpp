#include <iostream>
#include <fstream>
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
        std::cerr << "File opening error!" << std::endl;
        return nullptr;
    }

    QuadraticEquation* head = nullptr;
    QuadraticEquation* tail = nullptr;
    double a, b, c;

    while (inputFile >> a >> b >> c) {
        if (a == 0) {
            std::cerr << "Incorrect equation: the coefficient a cannot be 0" << std::endl;
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
        std::cout << "The equation: " << current->a << "x^2 + " << current->b << "x + " << current->c << " = 0\n";
        if (!std::isnan(current->root1) && !std::isnan(current->root2)) {
            std::cout << "ROots: " << current->root1 << " and " << current->root2 << "\n";
        }
        else {
            std::cout << "There are no real roots.\n";
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
