#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <SFML/Graphics.hpp>


class GameField {
public:
    std::vector<std::vector<int>> field;
    int width, height;
    int colorsCount = 6; // 6 разных цветов
    GameField(int w, int h) : width(w), height(h) {
        field.resize(height, std::vector<int>(width));
        generateField();
    }

    void generateField() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, colorsCount);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                field[y][x] = dist(gen);
            }
        }
    }

    bool isValidSwap(int x1, int y1, int x2, int y2) {
        // Проверка, что квадраты соседние 
        return (abs(x1 - x2) == 1 && y1 == y2) || (abs(y1 - y2) == 1 && x1 == x2);
    }

    void swapCells(int x1, int y1, int x2, int y2) {
        if (!isValidSwap(x1, y1, x2, y2)) return;
        std::swap(field[y1][x1], field[y2][x2]);
    }

    bool checkMatches() {
        bool foundMatches = false;
        std::vector<std::pair<int, int>> matchedCells;

        // Проверка горизонтальных линий
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width - 2; ++x) {
                if (field[y][x] != 0 &&
                    field[y][x] == field[y][x + 1] &&
                    field[y][x] == field[y][x + 2]) {

                    // отмечаем все совпадения 
                    int color = field[y][x];
                    int endX = x + 2;
                    while (endX + 1 < width && field[y][endX + 1] == color) {
                        endX++;
                    }

                    for (int dx = x; dx <= endX; ++dx) {
                        matchedCells.emplace_back(dx, y);
                    }
                    foundMatches = true;
                }
            }
        }

        // проверка вертикальных линий 
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height - 2; ++y) {
                if (field[y][x] != 0 &&
                    field[y][x] == field[y + 1][x] &&
                    field[y][x] == field[y + 2][x]) {

                    int color = field[y][x];
                    int endY = y + 2;
                    while (endY + 1 < height && field[endY + 1][x] == color) {
                        endY++;
                    }

                    for (int dy = y; dy <= endY; ++dy) {
                        matchedCells.emplace_back(x, dy);
                    }
                    foundMatches = true;
                }
            }
        }

        // Удаляем дубликаты (если клетка входит в несколько совпадений)
        std::sort(matchedCells.begin(), matchedCells.end());
        matchedCells.erase(std::unique(matchedCells.begin(), matchedCells.end()), matchedCells.end());

        // отмечаем совпадения для удаления
        for (const auto& cell : matchedCells) {
            field[cell.second][cell.first] = 0;
        }

        return foundMatches;
    }

    void removeMatches() {
        // Сдвигаем элементы вниз 
        for (int x = 0; x < width; ++x) {
            int emptyY = height - 1; 

            // Проходим снизу вверх
            for (int y = height - 1; y >= 0; --y) {
                if (field[y][x] != 0) {
                    // Если клетка не пустая, перемещаем её вниз
                    field[emptyY][x] = field[y][x];
                    if (emptyY != y) {
                        field[y][x] = 0; // Отчистка старой позиции
                    }
                    --emptyY;
                }
            }

            // Заполняем оставшиеся пустые клетки новыми цветами
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1, colorsCount);

            for (int y = emptyY; y >= 0; --y) {
                field[y][x] = dist(gen);
            }
        }
    }
};

class BonusSystem {
public:
    enum BonusType { RECOLOR, BOMB, NO_BONUS };

    // Применяет бонус с учетом всех правил
    void applyBonus(BonusType type, int originX, int originY, GameField& field) {
        if (!isValidPosition(originX, originY, field)) return;

        // Выбираем случайную клетку в радиусе 3 от источника
        auto [targetX, targetY] = getRandomNearbyPosition(originX, originY, field, 3);

        switch (type) {
        case RECOLOR:
            applyRecolorEffect(originX, originY, targetX, targetY, field);
            break;

        case BOMB:
            applyBombEffect(targetX, targetY, field);
            break;

        case NO_BONUS:
        default:
            break;
        }
    }

    // Определяем, может ли выпасть бонус (пусть шанс 20%)
    bool shouldSpawnBonus() const {
        return rand() % 5 == 0; 
    }

    // Выбирает случайный тип бонуса
    BonusType getRandomBonusType() const {
        return (rand() % 2 == 0) ? RECOLOR : BOMB;
    }

private:
    bool isValidPosition(int x, int y, const GameField& field) const {
        return x >= 0 && y >= 0 && x < field.width && y < field.height;
    }

    std::pair<int, int> getRandomNearbyPosition(int centerX, int centerY,
        const GameField& field, int radius) {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        std::uniform_int_distribution<> distX(std::max(0, centerX - radius), std::min(field.width - 1, centerX + radius));
        std::uniform_int_distribution<> distY(std::max(0, centerY - radius), std::min(field.height - 1, centerY + radius));

        return { distX(gen), distY(gen) };
    }


    void applyRecolorEffect(int originX, int originY, int targetX, int targetY, GameField& field) {
        const int originColor = field.field[originY][originX];
        int recoloredCount = 0;

        //  Перекрашиваем основную клетку
        field.field[targetY][targetX] = originColor;

        // Перекрашиваем 2 несоседние клетки
        while (recoloredCount < 2) {
            auto [newX, newY] = getRandomNearbyPosition(originX, originY, field, 3);

            // Проверяем что клетка:
            // Не соседняя с основной
            // Не та же самая
            // Еще не перекрашена
            if ((abs(newX - targetX) > 1 || abs(newY - targetY) > 1) &&
                (newX != targetX || newY != targetY) &&
                field.field[newY][newX] != originColor) {

                field.field[newY][newX] = originColor;
                recoloredCount++;
            }
        }
    }

    void applyBombEffect(int targetX, int targetY, GameField& field) {
        // Уничтожаем 5 клеток (включая целевую)
        destroyCell(targetX, targetY, field);

        for (int i = 0; i < 4; ) {
            int x = rand() % field.width;
            int y = rand() % field.height;

            // Не уничтожаем уже пустые клетки
            if (field.field[y][x] != 0) {
                destroyCell(x, y, field);
                i++;
            }
        }
    }

    void destroyCell(int x, int y, GameField& field) {
        if (isValidPosition(x, y, field)) {
            field.field[y][x] = 0;
        }
    }
};


class GameLogic {
private:
    GameField& field;
    BonusSystem bonusSystem;
    int score = 0;
public:
    GameLogic(GameField& gameField) : field(gameField) {}


    int getFieldWidth() const { return field.width; }
    int getFieldHeight() const { return field.height; }


    void processTurn(int x1, int y1, int x2, int y2) {
        if (!field.isValidSwap(x1, y1, x2, y2)) {
            std::cout << "Invalid swap!" << std::endl;
            return;
        }

        field.swapCells(x1, y1, x2, y2);

        if (!field.checkMatches()) {
            field.swapCells(x1, y1, x2, y2);
            std::cout << "No matches found! Swap reverted." << std::endl;
            return;
        }

        // Обработка цепочек совпадений
        while (true) {
            // Удаляем совпадения и сдвигаем клетки
            field.removeMatches();

            // Проверяем, остались ли совпадения после сдвига
            if (!field.checkMatches()) break;

            // 20% шанс выпадения бонуса при каждом удалении
            if (rand() % 5 == 0) {
                BonusSystem::BonusType bonus = (rand() % 2 == 0)
                    ? BonusSystem::RECOLOR
                    : BonusSystem::BOMB;

                // Выбираем случайную уничтоженную клетку как место для бонуса
                int originX = rand() % field.width;
                int originY = rand() % field.height;
                bonusSystem.applyBonus(bonus, originX, originY, field);
            }

            score += 10;
        }
    }

    int getScore() const { return score; }
};

class GraphicsManager {
private:
    const int cellSize = 60; // Размер одной клетки в пикселях
    const int infoPanelWidth = 600; // Ширина правой панели информации
    const int margin = 20; // Отступ от краев

    std::vector<sf::Color> gemColors = {
        sf::Color::Red,    // 1
        sf::Color::Green,  // 2
        sf::Color::Blue,   // 3
        sf::Color::Yellow, // 4
        sf::Color::Magenta,// 5
        sf::Color::Cyan    // 6
    };

    sf::Font font;
    sf::Text scoreText;
    sf::Text infoText;
    sf::RenderWindow window;

public:
    GraphicsManager(int fieldWidth, int fieldHeight)
        : window(sf::VideoMode(static_cast<unsigned int>(fieldWidth * cellSize + infoPanelWidth),
            static_cast<unsigned int>(fieldHeight * cellSize) , 32u),
            "GEMS Game")
    {

        // Загрузка шрифта
        if (!font.loadFromFile("C:/prog projects/C++/GEMS/arial.ttf")) {
            std::cerr << "Error loading font! Using default." << std::endl;
        }
        // Настройка текста для счета
        scoreText.setFont(font);
        scoreText.setCharacterSize(36);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(fieldWidth * cellSize + margin, margin);

        // Настройка информационного текста
        infoText.setFont(font);
        infoText.setCharacterSize(24);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(fieldWidth * cellSize + margin, 100);
        infoText.setString("Click two adjacent gems\nto swap them\n\nMatch 3 or more\nto score points");
    }

    void drawField(const GameField& field, int score) {
        window.clear(sf::Color(40, 40, 40)); // Темно-серый фон

        // 1. Рисуем игровое поле
        for (int y = 0; y < field.height; ++y) {
            for (int x = 0; x < field.width; ++x) {
                // Пропускаем пустые клетки (0)
                if (field.field[y][x] == 0) continue;

                // Создаем прямоугольник для гема
                sf::RectangleShape gem(sf::Vector2f(cellSize - 6, cellSize - 6));
                gem.setPosition(
                    x * cellSize + 3,
                    y * cellSize + 3
                );

                // Устанавливаем цвет (с проверкой выхода за границы)
                int colorIndex = (field.field[y][x] - 1) % gemColors.size();
                gem.setFillColor(gemColors[colorIndex]);

                // Добавляем обводку
                gem.setOutlineThickness(3);
                gem.setOutlineColor(sf::Color(200, 200, 200));

                window.draw(gem);
            }
        }

        // 2. Рисуем информационную панель
        drawInfoPanel(field.width * cellSize, score);

        window.display();
    }

    void drawInfoPanel(int panelStartX, int score) {
        // Фон панели
        sf::RectangleShape panel(sf::Vector2f(infoPanelWidth, window.getSize().y));
        panel.setPosition(panelStartX, 0);
        panel.setFillColor(sf::Color(60, 60, 60));
        window.draw(panel);

        // Обновляем и рисуем текст счета
        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        // Рисуем информационный текст
        window.draw(infoText);
    }

    bool isOpen() const { return window.isOpen(); }

    void handleEvents(GameLogic& gameLogic) {
        sf::Event event;
        static bool firstClick = true;
        static sf::Vector2i firstPos;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x / cellSize;
                    int y = event.mouseButton.y / cellSize;

                    // Используем методы getFieldWidth() и getFieldHeight()
                    if (x >= 0 && x < gameLogic.getFieldWidth() &&
                        y >= 0 && y < gameLogic.getFieldHeight()) {

                        if (firstClick) {
                            firstPos = sf::Vector2i(x, y);
                            firstClick = false;
                        }
                        else {
                            gameLogic.processTurn(firstPos.x, firstPos.y, x, y);
                            firstClick = true;
                        }
                    }
                }
            }
        }
    }
};

int main() {
    std::srand(static_cast<unsigned>(time(nullptr)));
    try {
        const int fieldWidth = 8;
        const int fieldHeight = 8;

        GameField field(fieldWidth, fieldHeight);
        GameLogic gameLogic(field);
        GraphicsManager graphics(fieldWidth, fieldHeight);

        // Основной игровой цикл
        while (graphics.isOpen()) {
            graphics.handleEvents(gameLogic);
            graphics.drawField(field, gameLogic.getScore());

        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}