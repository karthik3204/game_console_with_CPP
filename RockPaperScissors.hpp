#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <string>

class RockPaperScissors {
private:
    enum class Choice { None, Rock, Paper, Scissors };
    enum class GameState { SelectRounds, Playing, RoundEnd, GameOver };
    
    struct Button {
        sf::RectangleShape shape;
        sf::Text text;
        bool isHovered;
    };

    // Window and resources
    sf::RenderWindow& window;
    sf::Font& font;
    
    // Game state
    GameState currentState;
    int playerScore;
    int computerScore;
    int targetScore;
    int totalRounds;
    int currentRound;
    Choice playerChoice;
    Choice computerChoice;
    bool shouldExit;

    // UI Elements
    Button rockButton;
    Button paperButton;
    Button scissorsButton;
    Button roundButtons[3];  // Best of 3, 5, 7
    sf::Text scoreText;
    sf::Text resultText;
    sf::Text roundText;

    sf::Clock gameClock;
    float roundEndTimer;
    float resultDisplayTimer;
    static constexpr float ROUND_END_DELAY = 2.0f;  // 2 seconds
    static constexpr float RESULT_DISPLAY_TIME = 1.5f;  // 1.5 seconds

    // Add continue button to member variables
    Button continueButton;

public:
    RockPaperScissors(sf::RenderWindow& gameWindow, sf::Font& gameFont)
        : window(gameWindow), font(gameFont), currentState(GameState::SelectRounds),
          playerScore(0), computerScore(0), targetScore(0), currentRound(1),
          playerChoice(Choice::None), computerChoice(Choice::None), shouldExit(false) {
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
        // Initialize choice buttons
        setupChoiceButtons();
        
        // Initialize round selection buttons
        setupRoundButtons();
        
        // Initialize text elements
        setupText();

        // Setup continue button
        setupButton(continueButton, "Continue", 
                   sf::Vector2f((window.getSize().x - 200.f) / 2,  // Center horizontally
                               window.getSize().y - 100.f),         // Near bottom
                   sf::Vector2f(200.f, 50.f));                     // Width, Height
        continueButton.shape.setFillColor(sf::Color(0, 150, 0));   // Green color
    }

    void setupChoiceButtons() {
        const float buttonSize = 100.f;
        const float spacing = 50.f;
        const float startX = (window.getSize().x - (3 * buttonSize + 2 * spacing)) / 2;
        const float startY = 250.f;

        // Rock Button
        setupButton(rockButton, "Rock", 
                   sf::Vector2f(startX, startY),
                   sf::Vector2f(buttonSize, buttonSize));

        // Paper Button
        setupButton(paperButton, "Paper", 
                   sf::Vector2f(startX + buttonSize + spacing, startY),
                   sf::Vector2f(buttonSize, buttonSize));

        // Scissors Button
        setupButton(scissorsButton, "Scissors", 
                   sf::Vector2f(startX + 2 * (buttonSize + spacing), startY),
                   sf::Vector2f(buttonSize, buttonSize));
    }

    void setupRoundButtons() {
        const float buttonWidth = 200.f;
        const float buttonHeight = 50.f;
        const float spacing = 30.f;
        const float startY = 200.f;

        std::string roundTexts[] = {"Best of 3", "Best of 5", "Best of 7"};
        
        for (int i = 0; i < 3; i++) {
            setupButton(roundButtons[i], roundTexts[i],
                       sf::Vector2f((window.getSize().x - buttonWidth) / 2,
                                  startY + i * (buttonHeight + spacing)),
                       sf::Vector2f(buttonWidth, buttonHeight));
        }
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
        // Score Text
        scoreText.setFont(font);
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(20, 20);
        
        // Result Text
        resultText.setFont(font);
        resultText.setCharacterSize(40);
        resultText.setFillColor(sf::Color::White);
        resultText.setPosition(
            window.getSize().x / 2,
            150.f
        );
        
        // Round Text
        roundText.setFont(font);
        roundText.setCharacterSize(24);
        roundText.setFillColor(sf::Color::White);
        roundText.setPosition(20, 60);
        
        updateScoreText();
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
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    shouldExit = true;
                }
                else {
                    handleKeyPress(event.key);
                }
            }
        }
    }

    void handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton) {
        sf::Vector2f mousePos(mouseButton.x, mouseButton.y);

        switch (currentState) {
            case GameState::SelectRounds:
                handleRoundSelection(mousePos);
                break;
            
            case GameState::Playing:
                handleChoiceSelection(mousePos);
                break;
            
            case GameState::RoundEnd:
                currentState = GameState::Playing;
                playerChoice = Choice::None;
                computerChoice = Choice::None;
                if (playerChoice != computerChoice) {  // Only increment round if it wasn't a draw
                    currentRound++;
                }
                updateScoreText();
                break;
            
            case GameState::GameOver:
                if (checkButtonClick(mousePos, continueButton)) {
                    shouldExit = true;
                    return;
                }
                break;
        }
    }

    void handleRoundSelection(const sf::Vector2f& mousePos) {
        for (int i = 0; i < 3; i++) {
            if (checkButtonClick(mousePos, roundButtons[i])) {
                totalRounds = (i + 1) * 2 + 1;  // 3, 5, or 7
                targetScore = totalRounds / 2 + 1;  // 2, 3, or 4
                currentState = GameState::Playing;
                updateScoreText();
                break;
            }
        }
    }

    void handleChoiceSelection(const sf::Vector2f& mousePos) {
        if (playerChoice == Choice::None) {
            if (checkButtonClick(mousePos, rockButton)) {
                makeChoice(Choice::Rock);
            }
            else if (checkButtonClick(mousePos, paperButton)) {
                makeChoice(Choice::Paper);
            }
            else if (checkButtonClick(mousePos, scissorsButton)) {
                makeChoice(Choice::Scissors);
            }
        }
    }

    void handleKeyPress(const sf::Event::KeyEvent& keyEvent) {
        if (currentState == GameState::Playing && playerChoice == Choice::None) {
            switch (keyEvent.code) {
                case sf::Keyboard::R:
                    makeChoice(Choice::Rock);
                    break;
                case sf::Keyboard::P:
                    makeChoice(Choice::Paper);
                    break;
                case sf::Keyboard::S:
                    makeChoice(Choice::Scissors);
                    break;
                default:
                    break;
            }
        }
    }

    void makeChoice(Choice choice) {
        playerChoice = choice;
        computerChoice = getRandomChoice();
        determineRoundWinner();
        roundEndTimer = ROUND_END_DELAY;
        resultDisplayTimer = RESULT_DISPLAY_TIME;
        currentState = GameState::RoundEnd;
    }

    Choice getRandomChoice() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(1, 3);
        return static_cast<Choice>(dis(gen));
    }

    void determineRoundWinner() {
        if (playerChoice == computerChoice) {
            resultText.setString("Draw! Play again!");
            resultText.setFillColor(sf::Color::Yellow);
            currentRound--;  // Don't count this round
        }
        else if ((playerChoice == Choice::Rock && computerChoice == Choice::Scissors) ||
                 (playerChoice == Choice::Paper && computerChoice == Choice::Rock) ||
                 (playerChoice == Choice::Scissors && computerChoice == Choice::Paper)) {
            resultText.setString("You Win!");
            resultText.setFillColor(sf::Color::Green);
            playerScore++;
        }
        else {
            resultText.setString("Computer Wins!");
            resultText.setFillColor(sf::Color::Red);
            computerScore++;
        }

        updateScoreText();
        
        if (playerScore >= targetScore || computerScore >= targetScore) {
            currentState = GameState::GameOver;
        }
        else {
            currentState = GameState::RoundEnd;
        }
    }

    void updateScoreText() {
        scoreText.setString("Score - Player: " + std::to_string(playerScore) + 
                          " Computer: " + std::to_string(computerScore));
        
        roundText.setString("Round: " + std::to_string(currentRound) + 
                          " of " + std::to_string(totalRounds));
    }

    bool checkButtonClick(const sf::Vector2f& mousePos, const Button& button) {
        return button.shape.getGlobalBounds().contains(mousePos);
    }

    void handleMouseMove(const sf::Event::MouseMoveEvent& mouseMove) {
        sf::Vector2f mousePos(mouseMove.x, mouseMove.y);
        
        // Update hover states based on current game state
        if (currentState == GameState::SelectRounds) {
            for (auto& button : roundButtons) {
                updateButtonHover(button, mousePos);
            }
        }
        else if (currentState == GameState::Playing) {
            updateButtonHover(rockButton, mousePos);
            updateButtonHover(paperButton, mousePos);
            updateButtonHover(scissorsButton, mousePos);
        }
        else if (currentState == GameState::GameOver) {
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

    void resetGame() {
        playerScore = 0;
        computerScore = 0;
        currentRound = 1;
        playerChoice = Choice::None;
        computerChoice = Choice::None;
        currentState = GameState::SelectRounds;
        updateScoreText();
    }

    void render() {
        window.clear(sf::Color(50, 50, 50));

        // Always draw score
        window.draw(scoreText);
        window.draw(roundText);

        switch (currentState) {
            case GameState::SelectRounds:
                renderRoundSelection();
                break;
            
            case GameState::Playing:
            case GameState::RoundEnd:
                renderGame();
                break;
            
            case GameState::GameOver:
                renderGameOver();
                break;
        }

        window.display();
    }

    void renderRoundSelection() {
        sf::Text title("Select Number of Rounds", font, 40);
        title.setPosition(
            (window.getSize().x - title.getGlobalBounds().width) / 2,
            100
        );
        window.draw(title);

        for (const auto& button : roundButtons) {
            window.draw(button.shape);
            window.draw(button.text);
        }
    }

    void renderGame() {
        // Draw choice buttons
        window.draw(rockButton.shape);
        window.draw(rockButton.text);
        window.draw(paperButton.shape);
        window.draw(paperButton.text);
        window.draw(scissorsButton.shape);
        window.draw(scissorsButton.text);

        // Draw choices and result if round is complete
        if (currentState == GameState::RoundEnd) {
            window.draw(resultText);
            
            // Display choices
            sf::Text playerChoiceText("Your choice: " + choiceToString(playerChoice), font, 24);
            sf::Text computerChoiceText("Computer's choice: " + choiceToString(computerChoice), font, 24);
            
            playerChoiceText.setPosition(200, 400);
            computerChoiceText.setPosition(200, 450);
            
            window.draw(playerChoiceText);
            window.draw(computerChoiceText);

            // Add instruction for draw
            if (playerChoice == computerChoice) {
                sf::Text drawText("Round will not count. Click to play again.", font, 24);
                drawText.setFillColor(sf::Color::Yellow);
                drawText.setPosition(
                    (window.getSize().x - drawText.getGlobalBounds().width) / 2,
                    500
                );
                window.draw(drawText);
            }
        }
    }

    void renderGameOver() {
        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(40);
        
        if (playerScore > computerScore) {
            gameOverText.setString("Congratulations! You Won!");
            gameOverText.setFillColor(sf::Color::Green);
        }
        else {
            gameOverText.setString("Game Over! Computer Wins!");
            gameOverText.setFillColor(sf::Color::Red);
        }
        
        gameOverText.setPosition(
            (window.getSize().x - gameOverText.getGlobalBounds().width) / 2,
            200
        );
        
        window.draw(gameOverText);

        // Draw final score
        sf::Text finalScoreText;
        finalScoreText.setFont(font);
        finalScoreText.setCharacterSize(30);
        finalScoreText.setString("Final Score: Player " + std::to_string(playerScore) + 
                                " - Computer " + std::to_string(computerScore));
        finalScoreText.setFillColor(sf::Color::White);
        finalScoreText.setPosition(
            (window.getSize().x - finalScoreText.getGlobalBounds().width) / 2,
            300
        );
        
        window.draw(finalScoreText);
        
        // Draw continue button
        window.draw(continueButton.shape);
        window.draw(continueButton.text);
    }

    std::string choiceToString(Choice choice) {
        switch (choice) {
            case Choice::Rock: return "Rock";
            case Choice::Paper: return "Paper";
            case Choice::Scissors: return "Scissors";
            default: return "None";
        }
    }

    void updateGame() {
        float deltaTime = gameClock.restart().asSeconds();

        switch (currentState) {
            case GameState::RoundEnd:
                // Add delay before next round
                roundEndTimer -= deltaTime;
                if (roundEndTimer <= 0) {
                    if (playerScore >= targetScore || computerScore >= targetScore) {
                        currentState = GameState::GameOver;
                    } else {
                        currentState = GameState::Playing;
                        playerChoice = Choice::None;
                        computerChoice = Choice::None;
                        currentRound++;
                        updateScoreText();
                    }
                }
                break;

            case GameState::Playing:
                // Update any animations or effects
                if (resultDisplayTimer > 0) {
                    resultDisplayTimer -= deltaTime;
                    // Could update text opacity or position here
                }
                break;
        }
    }
}; 