#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include "HighScore.hpp"

struct DropdownMenu {
    sf::RectangleShape button;
    sf::Text buttonText;
    sf::RectangleShape optionsBox;
    std::vector<sf::Text> options;
    bool isOpen;
    int selectedIndex;

    DropdownMenu(const sf::Font& font, const std::string& defaultText, 
                const std::vector<std::string>& optionTexts, float x, float y) 
        : isOpen(false), selectedIndex(0) {
        
        // Setup main button
        button.setSize(sf::Vector2f(200, 40));
        button.setPosition(x, y);
        button.setFillColor(sf::Color(100, 100, 100));
        
        buttonText.setFont(font);
        buttonText.setString(defaultText);
        buttonText.setCharacterSize(20);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(
            x + 10,
            y + (button.getSize().y - buttonText.getCharacterSize()) / 2
        );

        // Setup options box
        optionsBox.setSize(sf::Vector2f(200, optionTexts.size() * 40));
        optionsBox.setPosition(x, y + 45);
        optionsBox.setFillColor(sf::Color(80, 80, 80));

        // Setup options
        for (size_t i = 0; i < optionTexts.size(); i++) {
            sf::Text option;
            option.setFont(font);
            option.setString(optionTexts[i]);
            option.setCharacterSize(20);
            option.setFillColor(sf::Color::White);
            option.setPosition(
                x + 10,
                y + 45 + (i * 40) + (40 - option.getCharacterSize()) / 2
            );
            options.push_back(option);
        }
    }

    bool handleClick(float mouseX, float mouseY) {
        if (button.getGlobalBounds().contains(mouseX, mouseY)) {
            isOpen = !isOpen;
            return true;
        }
        
        if (isOpen && optionsBox.getGlobalBounds().contains(mouseX, mouseY)) {
            for (size_t i = 0; i < options.size(); i++) {
                if (mouseY >= options[i].getPosition().y && 
                    mouseY <= options[i].getPosition().y + 40) {
                    selectedIndex = i;
                    buttonText.setString(options[i].getString());
                    isOpen = false;
                    return true;
                }
            }
        }
        return false;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(button);
        window.draw(buttonText);
        if (isOpen) {
            window.draw(optionsBox);
            for (const auto& option : options) {
                window.draw(option);
            }
        }
    }
};

struct ToggleSwitch {
    sf::RectangleShape background;
    sf::CircleShape knob;
    sf::Text label;
    bool isOn;

    ToggleSwitch(const sf::Font& font, const std::string& labelText, float x, float y) 
        : isOn(false) {
        
        background.setSize(sf::Vector2f(60, 30));
        background.setPosition(x, y);
        background.setFillColor(sf::Color(100, 100, 100));
        
        knob.setRadius(15);
        knob.setPosition(x, y);
        knob.setFillColor(sf::Color::White);
        
        label.setFont(font);
        label.setString(labelText);
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::White);
        label.setPosition(x + 70, y + 5);
    }

    bool handleClick(float mouseX, float mouseY) {
        if (background.getGlobalBounds().contains(mouseX, mouseY)) {
            isOn = !isOn;
            if (isOn) {
                knob.setPosition(
                    background.getPosition().x + background.getSize().x - knob.getRadius() * 2,
                    knob.getPosition().y
                );
                background.setFillColor(sf::Color::Green);
            } else {
                knob.setPosition(
                    background.getPosition().x,
                    knob.getPosition().y
                );
                background.setFillColor(sf::Color(100, 100, 100));
            }
            return true;
        }
        return false;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(knob);
        window.draw(label);
    }
};

class SnakeGame {
private:
    // Window and rendering
    sf::RenderWindow& window;
    sf::Font& font;
    sf::Text scoreText;

    // Game objects
    std::vector<sf::Vector2f> snake;
    std::vector<sf::RectangleShape> obstacles;
    sf::Vector2f food;
    sf::Vector2f direction;

    // Game settings
    float gridSize;
    float moveTimer;
    float moveInterval;
    const float SPEED_EASY = 0.15f;
    const float SPEED_MEDIUM = 0.08f;
    const float SPEED_HARD = 0.05f;

    // Game state
    bool gameOver;
    bool showingHighScores;
    bool gameStarted;
    int score;
    enum class Difficulty { EASY = 0, MEDIUM, HARD };
    Difficulty speedDifficulty;
    bool hasObstacles;

    // High score system
    HighScoreManager highScoreManager;
    std::string playerName;

    void displayHighScores() {
        window.clear(sf::Color(50, 50, 50));

        sf::Text title("High Scores", font, 40);
        title.setPosition(
            window.getSize().x / 2 - title.getGlobalBounds().width / 2,
            50
        );
        window.draw(title);

        float yPos = 150;
        const auto& scores = highScoreManager.getHighScores();
        
        for (const auto& score : scores) {
            sf::Text scoreText(score.toString(), font, 20);
            scoreText.setPosition(200, yPos);
            scoreText.setFillColor(sf::Color::White);
            window.draw(scoreText);
            yPos += 40;
        }

        sf::Text continueText("Press SPACE to continue", font, 24);
        continueText.setPosition(
            window.getSize().x / 2 - continueText.getGlobalBounds().width / 2,
            yPos + 50
        );
        window.draw(continueText);

        window.display();
    }

    void handleGameOver() {
        if (gameOver && !showingHighScores) {
            if (highScoreManager.isHighScore(score)) {
                playerName = getPlayerName();
                
                std::string difficultyStr;
                switch (speedDifficulty) {
                    case Difficulty::EASY: difficultyStr = "Easy"; break;
                    case Difficulty::MEDIUM: difficultyStr = "Medium"; break;
                    case Difficulty::HARD: difficultyStr = "Hard"; break;
                }
                
                highScoreManager.addScore(playerName, score, difficultyStr, hasObstacles);
                showingHighScores = true;
            }
        }
    }

    std::string getPlayerName() {
        std::string name;
        sf::Text inputText;
        inputText.setFont(font);
        inputText.setCharacterSize(30);
        inputText.setFillColor(sf::Color::White);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\r' || event.text.unicode == '\n') {
                        return !name.empty() ? name : "Player";
                    }
                    else if (event.text.unicode == '\b') {
                        if (!name.empty()) name.pop_back();
                    }
                    else if (name.length() < 15 && event.text.unicode >= 32 && event.text.unicode < 127) {
                        name += static_cast<char>(event.text.unicode);
                    }
                }
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return "Player";
                }
            }

            window.clear(sf::Color(50, 50, 50));
            
            sf::Text prompt("New High Score!\nEnter your name:", font, 30);
            prompt.setPosition(
                window.getSize().x / 2 - prompt.getGlobalBounds().width / 2,
                200
            );
            
            inputText.setString(name + "_");
            inputText.setPosition(
                window.getSize().x / 2 - inputText.getGlobalBounds().width / 2,
                300
            );
            
            window.draw(prompt);
            window.draw(inputText);
            window.display();
        }
        return "Player";
    }

public:
    SnakeGame(sf::RenderWindow& gameWindow, sf::Font& gameFont) 
        : window(gameWindow), font(gameFont), gridSize(20.f), moveTimer(0.f),
          gameOver(false), showingHighScores(false), gameStarted(false), score(0) {
        
        setupGame();
    }

    void run() {
        // First show difficulty selection
        if (!selectDifficulty()) {
            return;
        }

        // Initialize game
        setupGame();
        gameStarted = true;
        
        sf::Clock clock;
        while (window.isOpen()) {
            float deltaTime = clock.restart().asSeconds();
            
            if (!handleEvents()) {
                return;
            }

            if (showingHighScores) {
                displayHighScores();
            } else if (gameStarted && !gameOver) {
                update(deltaTime);
                render();
            } else if (gameOver) {
                handleGameOver();
                render();
            }
        }
    }

private:
    void setupGame() {
        // Initialize snake position
        snake.clear();
        snake.push_back(sf::Vector2f(400.f, 300.f));
        for(int i = 1; i < 3; i++) {
            snake.push_back(sf::Vector2f(400.f + (i * gridSize), 300.f));
        }
        
        direction = sf::Vector2f(-gridSize, 0.f);
        score = 0;
        gameOver = false;
        showingHighScores = false;
        
        // Set speed based on difficulty
        switch (speedDifficulty) {
            case Difficulty::EASY:
                moveInterval = SPEED_EASY;
                break;
            case Difficulty::MEDIUM:
                moveInterval = SPEED_MEDIUM;
                break;
            case Difficulty::HARD:
                moveInterval = SPEED_HARD;
                break;
        }

        // Create obstacles if enabled
        obstacles.clear();
        if (hasObstacles) {
            createObstacles();
        }

        updateScoreText();
        spawnFood();
    }

    bool selectDifficulty() {
        // Initialize UI elements with adjusted positions
        std::vector<std::string> speedOptions = {"Easy", "Medium", "Hard"};
        DropdownMenu* speedDropdown = new DropdownMenu(font, "Select Speed", speedOptions, 300, 150);  // Moved up to y=150
        ToggleSwitch* obstacleToggle = new ToggleSwitch(font, "Obstacles", 300, 350);  // Moved down to y=350
        
        // Adjust start button position accordingly
        sf::RectangleShape startButton(sf::Vector2f(200, 50));
        startButton.setPosition(300, 450);  // Moved down to y=450
        
        sf::Text startButtonText;
        startButtonText.setFont(font);
        startButtonText.setString("Start Game");
        startButtonText.setCharacterSize(24);
        startButtonText.setFillColor(sf::Color::Black);
        startButtonText.setPosition(
            startButton.getPosition().x + (startButton.getSize().x - startButtonText.getGlobalBounds().width) / 2,
            startButton.getPosition().y + (startButton.getSize().y - startButtonText.getCharacterSize()) / 2
        );

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return false;
                }
                
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        float mouseX = event.mouseButton.x;
                        float mouseY = event.mouseButton.y;
                        
                        speedDropdown->handleClick(mouseX, mouseY);
                        obstacleToggle->handleClick(mouseX, mouseY);
                        
                        if (startButton.getGlobalBounds().contains(mouseX, mouseY)) {
                            // Set game settings based on selections
                            speedDifficulty = static_cast<Difficulty>(speedDropdown->selectedIndex);
                            hasObstacles = obstacleToggle->isOn;
                            
                            // Cleanup
                            delete speedDropdown;
                            delete obstacleToggle;
                            
                            setupGame();  // Apply the settings
                            return true;
                        }
                    }
                }
                
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        delete speedDropdown;
                        delete obstacleToggle;
                        return false;
                    }
                }
            }

            window.clear(sf::Color(50, 50, 50));
            
            // Draw UI elements
            speedDropdown->draw(window);
            obstacleToggle->draw(window);
            window.draw(startButton);
            window.draw(startButtonText);
            
            window.display();
        }

        delete speedDropdown;
        delete obstacleToggle;
        return false;
    }

    bool handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    return false;
                }
                
                if (showingHighScores && event.key.code == sf::Keyboard::Space) {
                    return false;  // Return to main menu
                }

                // Only handle game controls if game is active
                if (gameStarted && !gameOver && !showingHighScores) {
                    switch (event.key.code) {
                        case sf::Keyboard::Up:
                            if (direction.y == 0)
                                direction = sf::Vector2f(0.f, -gridSize);
                            break;
                        case sf::Keyboard::Down:
                            if (direction.y == 0)
                                direction = sf::Vector2f(0.f, gridSize);
                            break;
                        case sf::Keyboard::Left:
                            if (direction.x == 0)
                                direction = sf::Vector2f(-gridSize, 0.f);
                            break;
                        case sf::Keyboard::Right:
                            if (direction.x == 0)
                                direction = sf::Vector2f(gridSize, 0.f);
                            break;
                    }
                }
            }
        }
        return true;
    }

    void createObstacles() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> disX(2, (window.getSize().x / int(gridSize)) - 3);
        std::uniform_int_distribution<> disY(2, (window.getSize().y / int(gridSize)) - 3);
        
        // Add 8 random obstacles
        for (int i = 0; i < 8; i++) {
            bool validPosition;
            sf::Vector2f obstaclePos;
            
            do {
                validPosition = true;
                obstaclePos = sf::Vector2f(disX(gen) * gridSize, disY(gen) * gridSize);
                
                // Check if obstacle would spawn on snake
                for (const auto& segment : snake) {
                    if (obstaclePos == segment) {
                        validPosition = false;
                        break;
                    }
                }
                
                // Check if obstacle would spawn on food
                if (obstaclePos == food) {
                    validPosition = false;
                }
                
                // Check if obstacle would spawn on another obstacle
                for (const auto& existing : obstacles) {
                    if (existing.getPosition() == obstaclePos) {
                        validPosition = false;
                        break;
                    }
                }
                
            } while (!validPosition);

            sf::RectangleShape obstacle(sf::Vector2f(gridSize - 1, gridSize - 1));
            obstacle.setPosition(obstaclePos);
            obstacle.setFillColor(sf::Color::Blue);
            obstacles.push_back(obstacle);
        }
    }

    void update(float deltaTime) {
        if (gameOver) return;

        moveTimer += deltaTime;
        if (moveTimer >= moveInterval) {
            moveTimer = 0;
            
            sf::Vector2f newHead = snake[0] + direction;
            
            // Check wall collision
            if (newHead.x < 0 || newHead.x >= window.getSize().x ||
                newHead.y < 0 || newHead.y >= window.getSize().y) {
                gameOver = true;
                return;
            }

            // Check self collision
            for (size_t i = 1; i < snake.size(); i++) {
                if (newHead == snake[i]) {
                    gameOver = true;
                    return;
                }
            }

            // Check obstacle collision
            for (const auto& obstacle : obstacles) {
                if (obstacle.getGlobalBounds().contains(newHead)) {
                    gameOver = true;
                    return;
                }
            }

            // Move snake
            snake.insert(snake.begin(), newHead);
            
            // Check food collision
            if (newHead == food) {
                score += 10;
                updateScoreText();
                spawnFood();
            } else {
                snake.pop_back();
            }
        }
    }

    void render() {
        window.clear(sf::Color(50, 50, 50));

        if (showingHighScores) {
            displayHighScores();
            return;
        }

        // Draw obstacles
        if (hasObstacles) {
            for (const auto& obstacle : obstacles) {
                window.draw(obstacle);
            }
        }

        // Draw snake
        for (const auto& segment : snake) {
            sf::RectangleShape rect(sf::Vector2f(gridSize - 1, gridSize - 1));
            rect.setPosition(segment);
            rect.setFillColor(sf::Color::Green);
            window.draw(rect);
        }

        // Draw food
        sf::RectangleShape foodRect(sf::Vector2f(gridSize - 1, gridSize - 1));
        foodRect.setPosition(food);
        foodRect.setFillColor(sf::Color::Red);
        window.draw(foodRect);

        // Draw score
        window.draw(scoreText);

        if (gameOver) {
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("Game Over!\nFinal Score: " + std::to_string(score) + 
                                 "\nPress SPACE to continue");
            gameOverText.setCharacterSize(40);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(
                window.getSize().x / 2 - gameOverText.getGlobalBounds().width / 2,
                window.getSize().y / 2 - gameOverText.getGlobalBounds().height / 2
            );
            window.draw(gameOverText);
        }

        window.display();
    }

    void spawnFood() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> disX(0, (window.getSize().x / int(gridSize)) - 1);
        std::uniform_int_distribution<> disY(0, (window.getSize().y / int(gridSize)) - 1);
        
        bool validPosition;
        do {
            validPosition = true;
            food = sf::Vector2f(disX(gen) * gridSize, disY(gen) * gridSize);
            
            // Check if food spawned on snake
            for (const auto& segment : snake) {
                if (food == segment) {
                    validPosition = false;
                    break;
                }
            }
            
            // Check if food spawned on obstacle
            for (const auto& obstacle : obstacles) {
                if (obstacle.getGlobalBounds().contains(food)) {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition);
    }

    void updateScoreText() {
        scoreText.setFont(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);  // Position in top-left corner
    }
}; 