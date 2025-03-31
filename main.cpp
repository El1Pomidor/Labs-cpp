#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>
#include <string>
#include <random>
#include <iomanip>

struct QuadraticEquation {
    double a, b, c;
    double root1 = std::numeric_limits<double>::quiet_NaN();
    double root2 = std::numeric_limits<double>::quiet_NaN();
    QuadraticEquation* next = nullptr;
};

enum class StudentType { EXCELLENT, GOOD, POOR };

struct Student {
    std::string name;
    StudentType type;
    int cnt = 0;
    Student* next = nullptr;
};

Student* readStudentsFromFile(const std::string& filename) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return nullptr;
    }

    Student* head = nullptr;
    Student* tail = nullptr;
    std::string line;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 2);

    while (std::getline(inputFile, line)) {
        StudentType type = static_cast<StudentType>(dist(gen));
        Student* newStudent = new Student{ line, type, 0, nullptr };
        if (!head) {
            head = tail = newStudent;
        }
        else {
            tail->next = newStudent;
            tail = newStudent;
        }
    }

    inputFile.close();
    return head;
}

void Teacher(Student* students, QuadraticEquation* equations) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.8, 1.0);

    Student* currentStudent = students;
    while (currentStudent) {
        int correctCount = 0;
        QuadraticEquation* currentEq = equations;
        while (currentEq) {
            if (currentStudent->type == StudentType::EXCELLENT) {
                correctCount++;
            }
            else if (currentStudent->type == StudentType::GOOD) {
                double factor = dist(gen);
                double root1 = currentEq->root1 * factor;
                double root2 = currentEq->root2 * factor;
                if ((std::abs(root1 - currentEq->root1) < 1e-1) && (std::abs(root2 - currentEq->root2) < 1e-1)) {
                    correctCount++;
                }
            }
            currentEq = currentEq->next;
        }
        currentStudent->cnt = correctCount;
        currentStudent = currentStudent->next;
    }
}

void printStudentsTable(Student* head) {
    std::cout << std::left << std::setw(20) << "Name" << std::setw(15) << "Type" << "Solved Equations" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    Student* current = head;
    while (current) {
        std::cout << std::left << std::setw(20) << current->name;
        switch (current->type) {
        case StudentType::EXCELLENT: std::cout << std::setw(15) << "Excellent"; break;
        case StudentType::GOOD: std::cout << std::setw(15) << "Good"; break;
        case StudentType::POOR: std::cout << std::setw(15) << "Bad"; break;
        }
        std::cout << current->cnt << std::endl;
        current = current->next;
    }
}

void deleteStudents(Student* head) {
    while (head) {
        Student* temp = head;
        head = head->next;
        delete temp;
    }
}

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
    std::string line;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        double a, b, c;
        if (!(iss >> a >> b >> c)) {
            std::cerr << "Invalid string: " << line << std::endl;
            continue;
        }
        if (a == 0) {
            std::cerr << "Incorrect equation: a cannot be 0." << std::endl;
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

    Student* students = readStudentsFromFile("students.txt");
    if (!students) return 1;

    Teacher(students, equations);
    printStudentsTable(students);

    deleteEquations(equations);
    deleteStudents(students);

    return 0;
}
