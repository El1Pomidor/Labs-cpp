#include <iostream>
#include <cmath>

int main() {
    double a, b, c;

    std::cout << "ax^2 + bx + c = 0" << std::endl;
    std::cout << "Please, enter coefficients a, b and c " << std::endl;
    std::cout << "a = ";
    std::cin >> a ;
    std::cout << "b = ";
    std::cin >> b;
    std::cout << "c = ";
    std::cin >> c;

    if (a == 0) {
        if (b == 0) {
            std::cout << "Equation doesn't have solutions." << std::endl;
        }
        else {
            std::cout << "It's a linear equation, the root: " << c / b << std::endl;
        }
    }
    else {
        double D = b * b - 4 * a * c;
        if (D > 0) {
            double x1 = (- b + std::sqrt(D)) / (2.0 * a);
            double x2 = (-b - std::sqrt(D)) / (2.0 * a);
            std::cout << "Roots are: " << x1 << " and " << x2 << std::endl;
        }
        else if (D == 0) {
            double x = -b / (2.0 * a);
            std::cout << "There is only one root: " << x << std::endl;
        }
        else {
            std::cout << "There are no real roots." << std::endl;
        }
    }

    return 0;
}
