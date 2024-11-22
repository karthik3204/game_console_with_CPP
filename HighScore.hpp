#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>

class ScoreEntry {
private:
    std::string playerName;
    int score;
    std::string difficulty;
    bool hasObstacles;

public:
    // Default constructor
    ScoreEntry() : playerName(""), score(0), difficulty(""), hasObstacles(false) {}

    // Parameter constructor
    ScoreEntry(const std::string& name, int s, const std::string& diff, bool obs)
        : playerName(name), score(s), difficulty(diff), hasObstacles(obs) {}

    // Getters
    std::string getPlayerName() const { return playerName; }
    int getScore() const { return score; }
    std::string getDifficulty() const { return difficulty; }
    bool getHasObstacles() const { return hasObstacles; }

    // Method to format score entry as string
    std::string toString() const {
        std::stringstream ss;
        ss << std::left << std::setw(15) << playerName 
           << std::setw(8) << score 
           << std::setw(10) << difficulty
           << (hasObstacles ? " (with obstacles)" : "");
        return ss.str();
    }

    // Method to save to file
    friend std::ofstream& operator<<(std::ofstream& out, const ScoreEntry& entry) {
        out << entry.playerName << " " 
            << entry.score << " "
            << entry.difficulty << " "
            << entry.hasObstacles;
        return out;
    }

    // Method to load from file
    friend std::ifstream& operator>>(std::ifstream& in, ScoreEntry& entry) {
        in >> entry.playerName >> entry.score >> entry.difficulty >> entry.hasObstacles;
        return in;
    }
};

class HighScoreManager {
private:
    std::vector<ScoreEntry> highScores;
    const std::string filename;
    const int MAX_SCORES;

public:
    HighScoreManager(const std::string& file = "highscores.txt", int maxScores = 10) 
        : filename(file), MAX_SCORES(maxScores) {
        loadScores();
    }

    void loadScores() {
        std::ifstream file(filename);
        if (file.is_open()) {
            ScoreEntry entry;
            while (file >> entry) {
                highScores.push_back(entry);
            }
            file.close();
        }
    }

    void saveScores() {
        std::ofstream file(filename);
        if (file.is_open()) {
            for (const auto& score : highScores) {
                file << score << "\n";
            }
            file.close();
        }
    }

    bool isHighScore(int score) const {
        if (highScores.size() < MAX_SCORES) return true;
        return score > highScores.back().getScore();
    }

    void addScore(const std::string& name, int score, const std::string& diff, bool obs) {
        highScores.emplace_back(name, score, diff, obs);
        
        // Sort scores in descending order
        std::sort(highScores.begin(), highScores.end(),
            [](const ScoreEntry& a, const ScoreEntry& b) {
                return a.getScore() > b.getScore();
            });

        // Keep only top scores
        if (highScores.size() > MAX_SCORES) {
            highScores.erase(highScores.begin() + MAX_SCORES, highScores.end());
        }

        saveScores();
    }

    const std::vector<ScoreEntry>& getHighScores() const {
        return highScores;
    }
}; 