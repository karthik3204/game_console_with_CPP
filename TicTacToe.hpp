#pragma once
#include <SFML/Graphics.hpp>
#include <array>

class TicTacToe {
private:
    enum class Player { None, X, O };
    enum class GameMode { PvP, PvC };
    enum class AIDifficulty { Easy, Medium, Hard };

    sf::RenderWindow& window;
    sf::Font& font;
    
    // Game state
    std::array<Player, 9> board;
    Player currentPlayer;
    GameMode gameMode;
    AIDifficulty aiDifficulty;
    bool gameOver;
    bool showingMenu;
    
    // Visual elements
    sf::RectangleShape grid[4];  // Grid lines
    std::vector<sf::RectangleShape> cells;  // Clickable areas
    sf::Text menuText;
    sf::Text turnText;
    sf::Text resultText;
    
    // Buttons
    struct Button {
        sf::RectangleShape shape;
        sf::Text text;
        bool isHovered;
    };
    
    Button pvpButton;
    Button pvcButton;
    Button difficultyButtons[3];  // Easy, Medium, Hard
    Button playAgainButton;
    Button mainMenuButton;
    Button continueButton;
    bool showContinueButton;

    // Constants
    const float CELL_SIZE = 100.f;
    const float GRID_OFFSET_X = 250.f;  // Center the grid
    const float GRID_OFFSET_Y = 150.f;

    // Add this member variable
    bool shouldExit;

public:
    TicTacToe(sf::RenderWindow& gameWindow, sf::Font& gameFont)
        : window(gameWindow), font(gameFont), 
          currentPlayer(Player::X), gameOver(false), showingMenu(true),
          shouldExit(false) {
        initializeGame();
    }

    bool run() {
        while (window.isOpen() && !shouldExit) {
            handleEvents();
            update();
            render();
        }
        return true;
    }

private:
    void initializeGame() {
        // Initialize board
        board.fill(Player::None);
        
        // Setup grid lines
        for (int i = 0; i < 4; i++) {
            grid[i].setFillColor(sf::Color::White);
            if (i < 2) {  // Vertical lines
                grid[i].setSize(sf::Vector2f(4.f, CELL_SIZE * 3));
                grid[i].setPosition(GRID_OFFSET_X + CELL_SIZE * (i + 1), GRID_OFFSET_Y);
            } else {      // Horizontal lines
                grid[i].setSize(sf::Vector2f(CELL_SIZE * 3, 4.f));
                grid[i].setPosition(GRID_OFFSET_X, GRID_OFFSET_Y + CELL_SIZE * (i - 1));
            }
        }

        // Setup clickable cells
        cells.clear();
        for (int i = 0; i < 9; i++) {
            sf::RectangleShape cell;
            cell.setSize(sf::Vector2f(CELL_SIZE - 4, CELL_SIZE - 4));
            cell.setPosition(
                GRID_OFFSET_X + (i % 3) * CELL_SIZE + 2,
                GRID_OFFSET_Y + (i / 3) * CELL_SIZE + 2
            );
            cell.setFillColor(sf::Color::Transparent);
            cells.push_back(cell);
        }

        // Setup menu buttons
        setupMenuButtons();
        
        // Setup text
        turnText.setFont(font);
        turnText.setCharacterSize(24);
        turnText.setFillColor(sf::Color::White);
        turnText.setPosition(20, 20);
        updateTurnText();
    }

    void setupMenuButtons() {
        // PvP Button
        pvpButton.shape.setSize(sf::Vector2f(200, 50));
        pvpButton.shape.setPosition(300, 200);
        pvpButton.shape.setFillColor(sf::Color(100, 100, 100));
        pvpButton.text.setFont(font);
        pvpButton.text.setString("Player vs Player");
        pvpButton.text.setCharacterSize(20);
        centerText(pvpButton.text, pvpButton.shape);

        // PvC Button
        pvcButton.shape.setSize(sf::Vector2f(200, 50));
        pvcButton.shape.setPosition(300, 300);
        pvcButton.shape.setFillColor(sf::Color(100, 100, 100));
        pvcButton.text.setFont(font);
        pvcButton.text.setString("Player vs Computer");
        pvcButton.text.setCharacterSize(20);
        centerText(pvcButton.text, pvcButton.shape);

        // Difficulty Buttons
        std::string difficultyNames[] = {"Easy", "Medium", "Hard"};
        for (int i = 0; i < 3; i++) {
            difficultyButtons[i].shape.setSize(sf::Vector2f(150, 40));
            difficultyButtons[i].shape.setPosition(325, 400 + i * 60);
            difficultyButtons[i].shape.setFillColor(sf::Color(100, 100, 100));
            difficultyButtons[i].text.setFont(font);
            difficultyButtons[i].text.setString(difficultyNames[i]);
            difficultyButtons[i].text.setCharacterSize(20);
            centerText(difficultyButtons[i].text, difficultyButtons[i].shape);
        }

        // Setup continue button with clear visibility
        continueButton.shape.setSize(sf::Vector2f(200, 50));
        continueButton.shape.setPosition(
            window.getSize().x / 2 - 100,
            window.getSize().y - 150
        );
        continueButton.shape.setFillColor(sf::Color(0, 150, 0));
        continueButton.text.setFont(font);
        continueButton.text.setString("Continue");
        continueButton.text.setCharacterSize(24);
        continueButton.text.setFillColor(sf::Color::White);
        centerText(continueButton.text, continueButton.shape);
    }

    void centerText(sf::Text& text, const sf::RectangleShape& shape) {
        text.setPosition(
            shape.getPosition().x + (shape.getSize().x - text.getGlobalBounds().width) / 2,
            shape.getPosition().y + (shape.getSize().y - text.getGlobalBounds().height) / 2
        );
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    
                    if (gameOver) {
                        // Check for continue button click
                        if (continueButton.shape.getGlobalBounds().contains(mousePos)) {
                            shouldExit = true;  // Set flag to exit game
                            return;
                        }
                    }
                    else if (showingMenu) {
                        handleMenuClick(mousePos);
                    }
                    else {
                        handleGameClick(mousePos);
                    }
                }
            }
            else if (event.type == sf::Event::MouseMoved) {
                handleMouseMove(event.mouseMove);
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                shouldExit = true;
            }
        }
    }

    void handleMenuClick(const sf::Vector2f& mousePos) {
        if (pvpButton.shape.getGlobalBounds().contains(mousePos)) {
            gameMode = GameMode::PvP;
            startNewGame();
        }
        else if (pvcButton.shape.getGlobalBounds().contains(mousePos)) {
            gameMode = GameMode::PvC;
            aiDifficulty = AIDifficulty::Easy;  // Set default difficulty
            startNewGame();
        }
    }

    void handleGameClick(const sf::Vector2f& mousePos) {
        if (!gameOver) {
            // Only allow player moves when it's their turn
            if (gameMode == GameMode::PvC && currentPlayer == Player::O) {
                return;
            }

            for (size_t i = 0; i < cells.size(); i++) {
                if (cells[i].getGlobalBounds().contains(mousePos)) {
                    makeMove(i);
                    break;
                }
            }
        }
    }

    void handleMouseMove(const sf::Event::MouseMoveEvent& mouseMove) {
        sf::Vector2f mousePos(mouseMove.x, mouseMove.y);
        
        if (showingMenu) {
            pvpButton.isHovered = pvpButton.shape.getGlobalBounds().contains(mousePos);
            pvcButton.isHovered = pvcButton.shape.getGlobalBounds().contains(mousePos);
            updateButtonColors();
        }
        
        // Update continue button hover state if game is over
        if (gameOver) {
            bool isHovered = continueButton.shape.getGlobalBounds().contains(mousePos);
            continueButton.shape.setFillColor(isHovered ? 
                sf::Color(0, 180, 0) : sf::Color(0, 150, 0));
        }
    }

    void updateButtonColors() {
        pvpButton.shape.setFillColor(pvpButton.isHovered ? 
            sf::Color(150, 150, 150) : sf::Color(100, 100, 100));
        pvcButton.shape.setFillColor(pvcButton.isHovered ? 
            sf::Color(150, 150, 150) : sf::Color(100, 100, 100));
    }

    void makeMove(int cell) {
        if (board[cell] == Player::None && !gameOver) {
            board[cell] = currentPlayer;
            
            if (checkWin() || checkDraw()) {
                gameOver = true;
                return;
            }
            
            currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
            updateTurnText();
            
            // Make AI move if it's PvC mode and computer's turn
            if (gameMode == GameMode::PvC && currentPlayer == Player::O) {
                sf::sleep(sf::milliseconds(300));
                makeAIMove();
            }
        }
    }

    void makeAIMove() {
        // Simple AI implementation
        // First, check for winning move
        for (size_t i = 0; i < board.size(); i++) {
            if (board[i] == Player::None) {
                // Try move
                board[i] = Player::O;
                if (checkWin()) {
                    board[i] = Player::None;  // Reset before making actual move
                    makeMove(i);
                    return;
                }
                // Undo move
                board[i] = Player::None;
            }
        }

        // Then, block player's winning move
        for (size_t i = 0; i < board.size(); i++) {
            if (board[i] == Player::None) {
                // Try move
                board[i] = Player::X;
                if (checkWin()) {
                    board[i] = Player::None;  // Reset before making actual move
                    makeMove(i);
                    return;
                }
                // Undo move
                board[i] = Player::None;
            }
        }

        // If no winning or blocking moves, make a random move
        std::vector<int> availableMoves;
        for (size_t i = 0; i < board.size(); i++) {
            if (board[i] == Player::None) {
                availableMoves.push_back(i);
            }
        }
        
        if (!availableMoves.empty()) {
            int randomMove = availableMoves[rand() % availableMoves.size()];
            makeMove(randomMove);
        }
    }

    bool checkWin() {
        // Check rows, columns, and diagonals
        const int winPatterns[8][3] = {
            {0,1,2}, {3,4,5}, {6,7,8}, // Rows
            {0,3,6}, {1,4,7}, {2,5,8}, // Columns
            {0,4,8}, {2,4,6}           // Diagonals
        };

        for (const auto& pattern : winPatterns) {
            if (board[pattern[0]] != Player::None &&
                board[pattern[0]] == board[pattern[1]] &&
                board[pattern[0]] == board[pattern[2]]) {
                return true;
            }
        }
        return false;
    }

    bool checkDraw() {
        for (const auto& cell : board) {
            if (cell == Player::None) return false;
        }
        return true;
    }

    void updateTurnText() {
        turnText.setString("Current Turn: " + 
            std::string(currentPlayer == Player::X ? "X" : "O"));
    }

    void startNewGame() {
        board.fill(Player::None);
        currentPlayer = Player::X;
        gameOver = false;
        showingMenu = false;
        showContinueButton = false;
        updateTurnText();
    }

    void update() {
        // Add any game logic that needs to be updated each frame
    }

    void render() {
        window.clear(sf::Color(50, 50, 50));

        if (showingMenu) {
            renderMenu();
        }
        else {
            renderGame();
        }

        window.display();
    }

    void renderMenu() {
        window.draw(pvpButton.shape);
        window.draw(pvpButton.text);
        window.draw(pvcButton.shape);
        window.draw(pvcButton.text);
        
        if (gameMode == GameMode::PvC) {
            for (const auto& button : difficultyButtons) {
                window.draw(button.shape);
                window.draw(button.text);
            }
        }
    }

    void renderGame() {
        window.clear(sf::Color(50, 50, 50));

        // Draw grid
        for (const auto& line : grid) {
            window.draw(line);
        }

        // Draw X's and O's
        for (size_t i = 0; i < board.size(); i++) {
            if (board[i] != Player::None) {
                sf::Text symbol;
                symbol.setFont(font);
                symbol.setString(board[i] == Player::X ? "X" : "O");
                symbol.setCharacterSize(60);
                symbol.setFillColor(sf::Color::White);
                
                // Center the symbol in its cell
                sf::FloatRect bounds = symbol.getGlobalBounds();
                symbol.setPosition(
                    GRID_OFFSET_X + (i % 3) * CELL_SIZE + (CELL_SIZE - bounds.width) / 2,
                    GRID_OFFSET_Y + (i / 3) * CELL_SIZE + (CELL_SIZE - bounds.height) / 2
                );
                
                window.draw(symbol);
            }
        }

        // Draw turn indicator if game is not over
        if (!gameOver) {
            window.draw(turnText);
        }

        // Draw game over state
        if (gameOver) {
            // Draw game over text
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(40);
            gameOverText.setFillColor(sf::Color::White);
            
            if (checkWin()) {
                std::string winner = (currentPlayer == Player::X) ? "X" : "O";
                if (gameMode == GameMode::PvC && currentPlayer == Player::O) {
                    gameOverText.setString("Computer Wins!");
                } else {
                    gameOverText.setString("Player " + winner + " Wins!");
                }
            } else {
                gameOverText.setString("Draw!");
            }
            
            // Center the text
            sf::FloatRect bounds = gameOverText.getGlobalBounds();
            gameOverText.setPosition(
                (window.getSize().x - bounds.width) / 2,
                50
            );
            
            window.draw(gameOverText);

            // Always draw continue button in game over state
            window.draw(continueButton.shape);
            window.draw(continueButton.text);
        }
    }
}; 