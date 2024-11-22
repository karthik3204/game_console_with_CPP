#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <optional>
#include <random>

class ConnectFour {
private:
    enum class Player { None, One, Two };
    enum class GameState { Playing, GameOver };

    struct Button {
        sf::RectangleShape shape;
        sf::Text text;
        bool isHovered;
    };

    // Constants
    static const int ROWS = 6;
    static const int COLS = 7;
    float CELL_SIZE;
    float GRID_OFFSET_X;
    float GRID_OFFSET_Y;

    // Animation struct
    struct DroppingDisc {
        int col;
        float currentY;
        float targetY;
        Player player;
    };

    // Window and resources
    sf::RenderWindow& window;
    sf::Font& font;

    // Game state
    GameState currentState;
    Player currentPlayer;
    std::array<std::array<Player, COLS>, ROWS> grid;
    bool shouldExit;

    // Scores
    int player1Score;
    int player2Score;

    // UI Elements
    Button continueButton;
    sf::Text statusText;
    sf::Text scoreText;

    // Animation
    std::optional<DroppingDisc> droppingDisc;

public:
    ConnectFour(sf::RenderWindow& gameWindow, sf::Font& gameFont)
        : window(gameWindow), font(gameFont),
          CELL_SIZE(80.0f),
          GRID_OFFSET_X(200.0f),
          GRID_OFFSET_Y(100.0f),
          currentState(GameState::Playing), 
          currentPlayer(Player::One), 
          shouldExit(false),
          player1Score(0), 
          player2Score(0) {
        initializeGame();
    }

    bool run() {
        while (window.isOpen() && !shouldExit) { 
            handleEvents();
            updateGame();
            render();
        }
        return true;
    }

private:
    void initializeGame() {
        setupButtons();
        resetGrid();
        setupText();
    }

    void setupButtons() {
        // Only continue button needed
        setupButton(continueButton, "Continue",
                   sf::Vector2f((window.getSize().x - 200) / 2, window.getSize().y - 100),
                   sf::Vector2f(200, 50));
        continueButton.shape.setFillColor(sf::Color(0, 150, 0));
    }

    void setupButton(Button& button, const std::string& text,
                    const sf::Vector2f& position, const sf::Vector2f& size) {
        button.shape.setSize(size);
        button.shape.setPosition(position);
        button.shape.setFillColor(sf::Color(100, 100, 100));

        button.text.setFont(font);
        button.text.setString(text);
        button.text.setCharacterSize(24);
        button.text.setFillColor(sf::Color::White);

        // Center text in button
        sf::FloatRect textBounds = button.text.getGlobalBounds();
        button.text.setPosition(
            position.x + (size.x - textBounds.width) / 2,
            position.y + (size.y - textBounds.height) / 2
        );

        button.isHovered = false;
    }

    void setupText() {
        statusText.setFont(font);
        statusText.setCharacterSize(30);
        statusText.setFillColor(sf::Color::White);
        statusText.setPosition(20, 20);

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(20, 60);

        updateStatusText();
        updateScoreText();
    }

    void resetGrid() {
        for (auto& row : grid) {
            row.fill(Player::None);
        }
        currentPlayer = Player::One;
        droppingDisc.reset();
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                handleMouseClick(event.mouseButton);
            }
            else if (event.type == sf::Event::MouseMoved) {
                handleMouseMove(event.mouseMove);
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                shouldExit = true;
            }
        }
    }

    void handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton) {
        sf::Vector2f mousePos(mouseButton.x, mouseButton.y);

        if (currentState == GameState::Playing) {
            if (!droppingDisc) {  // Only allow new moves if no animation is playing
                int col = getColumnFromX(mousePos.x);
                if (col >= 0 && col < COLS && !isColumnFull(col)) {
                    makeMove(col);
                }
            }
        }
        else if (currentState == GameState::GameOver) {
            if (checkButtonClick(mousePos, continueButton)) {
                shouldExit = true;
            }
        }
    }

    void makeMove(int col) {
        int row = getLowestEmptyRow(col);
        if (row >= 0) {
            // Start dropping animation
            droppingDisc = DroppingDisc{
                col,
                GRID_OFFSET_Y - CELL_SIZE,  // Start above grid
                GRID_OFFSET_Y + row * CELL_SIZE,  // Target position
                currentPlayer
            };
        }
    }

    void updateGame() {
        if (droppingDisc) {
            // Update dropping animation
            const float DROP_SPEED = 500.0f;
            float deltaTime = 1.0f / 60.0f;

            droppingDisc->currentY += DROP_SPEED * deltaTime;

            if (droppingDisc->currentY >= droppingDisc->targetY) {
                // Animation complete, place disc
                int row = getLowestEmptyRow(droppingDisc->col);
                grid[row][droppingDisc->col] = droppingDisc->player;

                // Check for win
                if (checkWin(row, droppingDisc->col)) {
                    handleWin();
                }
                else if (isBoardFull()) {
                    handleDraw();
                }
                else {
                    // Switch players
                    currentPlayer = (currentPlayer == Player::One) ? Player::Two : Player::One;
                    updateStatusText();
                }

                droppingDisc.reset();
            }
        }
    }

    void handleWin() {
        currentState = GameState::GameOver;
        if (currentPlayer == Player::One) {
            player1Score++;
        } else {
            player2Score++;
        }
        updateScoreText();
        updateStatusText();
    }

    void handleDraw() {
        currentState = GameState::GameOver;
        updateStatusText();
    }

    void render() {
        window.clear(sf::Color(50, 50, 50));

        switch (currentState) {
            case GameState::Playing:
            case GameState::GameOver:
                renderGame();
                break;
        }

        window.display();
    }

    void renderGame() {
        // Draw status and score
        window.draw(statusText);
        window.draw(scoreText);

        // Draw grid
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                sf::CircleShape cell(CELL_SIZE / 2 - 2);
                cell.setPosition(
                    GRID_OFFSET_X + col * CELL_SIZE + 2,
                    GRID_OFFSET_Y + row * CELL_SIZE + 2
                );

                // Set color based on player
                switch (grid[row][col]) {
                    case Player::One:
                        cell.setFillColor(sf::Color::Red);
                        break;
                    case Player::Two:
                        cell.setFillColor(sf::Color::Yellow);
                        break;
                    default:
                        cell.setFillColor(sf::Color::White);
                        break;
                }

                window.draw(cell);
            }
        }

        // Draw dropping disc animation
        if (droppingDisc) {
            sf::CircleShape disc(CELL_SIZE / 2 - 2);
            disc.setPosition(
                GRID_OFFSET_X + droppingDisc->col * CELL_SIZE + 2,
                droppingDisc->currentY + 2
            );
            disc.setFillColor(droppingDisc->player == Player::One ? 
                             sf::Color::Red : sf::Color::Yellow);
            window.draw(disc);
        }

        // Draw continue button in game over state
        if (currentState == GameState::GameOver) {
            window.draw(continueButton.shape);
            window.draw(continueButton.text);
        }
    }

    // Helper functions
    bool isColumnFull(int col) const {
        return grid[0][col] != Player::None;
    }

    bool isBoardFull() const {
        for (int col = 0; col < COLS; col++) {
            if (!isColumnFull(col)) return false;
        }
        return true;
    }

    int getLowestEmptyRow(int col) const {
        for (int row = ROWS - 1; row >= 0; row--) {
            if (grid[row][col] == Player::None) {
                return row;
            }
        }
        return -1;
    }

    int getColumnFromX(float x) const {
        if (x < GRID_OFFSET_X) return -1;
        int col = (x - GRID_OFFSET_X) / CELL_SIZE;
        if (col >= COLS) return -1;
        return col;
    }

    void updateStatusText() {
        if (currentState == GameState::GameOver) {
            if (isBoardFull()) {
                statusText.setString("Game Over - Draw!");
            } else {
                std::string winner = (currentPlayer == Player::One) ? "Player 1" : "Player 2";
                statusText.setString(winner + " Wins!");
            }
        } else {
            std::string currentPlayerStr = (currentPlayer == Player::One) ? "Player 1" : "Player 2";
            statusText.setString(currentPlayerStr + "'s Turn");
        }
    }

    void updateScoreText() {
        scoreText.setString(
            "Player 1: " + std::to_string(player1Score) + " - " +
            "Player 2: " + std::to_string(player2Score)
        );
    }

    void handleMouseMove(const sf::Event::MouseMoveEvent& mouseMove) {
        sf::Vector2f mousePos(mouseMove.x, mouseMove.y);
        
        if (currentState == GameState::GameOver) {
            updateButtonHover(continueButton, mousePos);
        }
    }

    void updateButtonHover(Button& button, const sf::Vector2f& mousePos) {
        bool isHovered = button.shape.getGlobalBounds().contains(mousePos);
        if (isHovered != button.isHovered) {
            button.isHovered = isHovered;
            button.shape.setFillColor(
                isHovered ? sf::Color(150, 150, 150) : sf::Color(100, 100, 100)
            );
        }
    }

    void centerText(sf::Text& text, const sf::RectangleShape& shape) {
        sf::FloatRect textBounds = text.getGlobalBounds();
        text.setPosition(
            shape.getPosition().x + (shape.getSize().x - textBounds.width) / 2,
            shape.getPosition().y + (shape.getSize().y - textBounds.height) / 2
        );
    }

    bool checkButtonClick(const sf::Vector2f& mousePos, const Button& button) {
        return button.shape.getGlobalBounds().contains(mousePos);
    }

    bool checkWin(int row, int col) {
        // Check horizontal
        int count = 0;
        for (int c = 0; c < COLS; c++) {
            if (grid[row][c] == currentPlayer) {
                count++;
                if (count == 4) return true;
            } else {
                count = 0;
            }
        }

        // Check vertical
        count = 0;
        for (int r = 0; r < ROWS; r++) {
            if (grid[r][col] == currentPlayer) {
                count++;
                if (count == 4) return true;
            } else {
                count = 0;
            }
        }

        // Check diagonal (both directions)
        for (int r = 0; r < ROWS - 3; r++) {
            for (int c = 0; c < COLS - 3; c++) {
                if (grid[r][c] == currentPlayer &&
                    grid[r+1][c+1] == currentPlayer &&
                    grid[r+2][c+2] == currentPlayer &&
                    grid[r+3][c+3] == currentPlayer) {
                    return true;
                }
            }
        }

        for (int r = 3; r < ROWS; r++) {
            for (int c = 0; c < COLS - 3; c++) {
                if (grid[r][c] == currentPlayer &&
                    grid[r-1][c+1] == currentPlayer &&
                    grid[r-2][c+2] == currentPlayer &&
                    grid[r-3][c+3] == currentPlayer) {
                    return true;
                }
            }
        }

        return false;
    }
};

// // Define static constants
// const float ConnectFour::CELL_SIZE = 80.0f;
// const float ConnectFour::GRID_OFFSET_X = 200.0f;
// const float ConnectFour::GRID_OFFSET_Y = 100.0f; 