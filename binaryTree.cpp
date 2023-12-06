#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>

struct setNode{
    std::string palavra;
    std::string* leftChild, rightChild, father;
};

int numberOfNodes = 0;

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
            numberOfNodes++;
            insertWord(word);
        }
    }

    // Retorna o tamanho estimado do dicionário em bytes
    void estimateMemoryUsage() const {
        setNode teste;

        std::cout << "Espaço de memória de um nó da árvore: " << sizeof(teste) << " bytes" << std::endl; 
        std::cout << "Espaço de memória total ocupado: " << numberOfNodes*sizeof(teste) << " bytes" << std::endl; 
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

std::pair<std::vector<std::string>, size_t> getInput(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> words;
    std::string word;
    size_t maxLength = 0;

    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo." << std::endl;
        return std::make_pair(std::vector<std::string>(), 0);
    }

    while (file >> word) {
        words.push_back(word);
        if (word.length() > maxLength) {
            maxLength = word.length();
        }
    }

    file.close();

    return std::make_pair(words, maxLength);
}

int MAX_WORD_SIZE;

int main() {

    auto start = std::chrono::high_resolution_clock::now();

    std::string inputFileName = "./simpleInput/1000words.txt";

    std::pair<std::vector<std::string>, size_t> result = getInput(inputFileName);
    std::vector<std::string> words = result.first;
    MAX_WORD_SIZE = result.second;
    std::sort(words.begin(), words.end());

    AutocompleteSystem autocomplete;
    autocomplete.loadWords(words);

    // Parar a contagem de tempo
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by function: " << duration.count() << " milliseconds" << std::endl;

    autocomplete.estimateMemoryUsage();
    // std::cout << "Estimativa de uso de memória: " << memoryUsage << " bytes." << std::endl;

    
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
