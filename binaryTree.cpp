#include <iostream>
#include <set>
#include <string>
#include <vector>

class AutocompleteSystem {
private:
    std::set<std::string> dictionary;

public:
    // Insere uma palavra no dicionário
    void insertWord(const std::string& word) {
        dictionary.insert(word);
    }

    // Carrega uma lista de palavras no dicionário
    void loadWords(const std::vector<std::string>& words) {
        for (const std::string& word : words) {
            insertWord(word);
        }
    }

    // Encontra e retorna até 10 palavras com o prefixo dado
    std::vector<std::string> getSuggestions(const std::string& prefix) {
        std::vector<std::string> suggestions;
        auto it = dictionary.lower_bound(prefix);

        // Enquanto não atingir o fim do conjunto e o prefixo for o início da palavra
        while (it != dictionary.end() && it->substr(0, prefix.size()) == prefix) {
            suggestions.push_back(*it);
            if (suggestions.size() == 10) break;
            ++it;
        }

        return suggestions;
    }
};

int main() {
    AutocompleteSystem autocomplete;
    autocomplete.loadWords({"casa", "carro", "cachorro", "cadeira", "computador", "céu", "criança", "comida", "caneta", "cidade"});

    std::string input;
    std::cout << "Digite o inicio da palavra: ";
    std::cin >> input;

    std::vector<std::string> suggestions = autocomplete.getSuggestions(input);
    std::cout << "Sugestoes:\n";
    for (const std::string& suggestion : suggestions) {
        std::cout << suggestion << std::endl;
    }

    return 0;
}
