#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "SnakeGame.hpp"
#include "TicTacToe.hpp"
#include "RockPaperScissors.hpp"
#include "ConnectFour.hpp"

class GameConsole {
private:
    sf::RenderWindow window;
    std::vector<sf::Text> menuItems;
    sf::Font font;
    int selectedItem;

public:
    GameConsole() : window(sf::VideoMode(800, 600), "Game Console"), selectedItem(0) {
        // Load font
        if (!font.loadFromFile("arial.ttf")) {
            // Handle font loading error
            throw std::runtime_error("Failed to load font");
        }

        // Initialize menu items
        std::vector<std::string> gameNames = {
            "Snake Game",
            "Tic Tac Toe",
            "Rock Paper Scissors",
            "Connect Four",
            "Exit"
        };

        // Setup menu text items
        for (size_t i = 0; i < gameNames.size(); i++) {
            sf::Text text;
            text.setFont(font);
            text.setString(gameNames[i]);
            text.setCharacterSize(30);
            text.setFillColor(sf::Color::White);
            text.setPosition(300, 150 + i * 60);
            menuItems.push_back(text);
        }
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    moveSelection(-1);
                }
                if (event.key.code == sf::Keyboard::Down) {
                    moveSelection(1);
                }
                if (event.key.code == sf::Keyboard::Return) {
                    handleMenuSelection();
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                handleMouseHover(event.mouseMove.x, event.mouseMove.y);
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                }
            }
        }
    }

    void moveSelection(int direction) {
        selectedItem = (selectedItem + direction + menuItems.size()) % menuItems.size();
    }

    void handleMouseHover(int x, int y) {
        for (size_t i = 0; i < menuItems.size(); i++) {
            if (menuItems[i].getGlobalBounds().contains(x, y)) {
                selectedItem = i;
                break;
            }
        }
    }

    void handleMouseClick(int x, int y) {
        for (size_t i = 0; i < menuItems.size(); i++) {
            if (menuItems[i].getGlobalBounds().contains(x, y)) {
                selectedItem = i;
                handleMenuSelection();
                break;
            }
        }
    }

    void handleMenuSelection() {
        switch (selectedItem) {
            case 0: // Snake Game
                {
                    SnakeGame snake(window, font);
                    snake.run();
                }
                break;
            case 1: // Tic Tac Toe
                {
                    TicTacToe game(window, font);
                    game.run();
                }
                break;
            case 2: // Rock Paper Scissors
                {
                    RockPaperScissors game(window, font);
                    if (game.run()) {
                        selectedItem = -1;  // Reset selection
                    }
                }
                break;
            case 3: // Connect Four
                {
                    ConnectFour game(window, font);
                    if (game.run()) {
                        selectedItem = -1;  // Reset selection
                    }
                }
                break;
            case 4: // Exit
                window.close();
                break;
        }
    }

    void update() {
        // Update menu items appearance
        for (size_t i = 0; i < menuItems.size(); i++) {
            if (i == selectedItem) {
                menuItems[i].setFillColor(sf::Color::Yellow);
            } else {
                menuItems[i].setFillColor(sf::Color::White);
            }
        }
    }

    void render() {
        window.clear(sf::Color(50, 50, 50));
        
        // Draw menu items
        for (const auto& item : menuItems) {
            window.draw(item);
        }

        window.display();
    }
};

int main() {
    try {
        GameConsole console;
        console.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
} 