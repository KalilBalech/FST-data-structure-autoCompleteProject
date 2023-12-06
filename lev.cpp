#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int levenshteinDistance(const std::string &s1, const std::string &s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));

    for (size_t i = 0; i <= len1; ++i) {
        for (size_t j = 0; j <= len2; ++j) {
            if (i == 0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }

    return dp[len1][len2];
}

int main() {
    std::ifstream dictionaryFile("linuxDictionary.txt");
    if (!dictionaryFile) {
        std::cerr << "Error opening dictionary file.\n";
        return 1;
    }

    std::string targetWord;
    int distanceThreshold;
    std::cout << "Digite a palavra-alvo: ";
    std::cin >> targetWord;

    std::cout << "Digite a distancia (d): ";
    std::cin >> distanceThreshold;

    std::vector<std::string> closeWords;

    std::string word;
    while (dictionaryFile >> word) {
        int distance = levenshteinDistance(targetWord, word);
        if (distance <= distanceThreshold) {
            closeWords.push_back(word);
        }
    }

    std::cout << "Palavras a distancia " << distanceThreshold << " de '" << targetWord << "':\n";
    for (const auto &closeWord : closeWords) {
        std::cout << closeWord << "\n";
    }

    return 0;
}
