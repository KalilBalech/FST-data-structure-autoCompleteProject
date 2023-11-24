#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>      // std::pair
#include <string>
#include <fstream>


class Node {
public:
    int id;
    // O mapa agora associa um par de char e int (peso da transição) a um Node*.
    std::map<char, std::pair<Node*, int>> transitions; // Agora usando std::map
    bool is_final;
    std::string output;

    Node(int id, bool isFinal = false, std::string output = "") {
        this->id = id;        // Correto: atribui o parâmetro id ao membro da classe id.
        this->is_final = isFinal; // Correto: atribui o parâmetro isFinal ao membro da classe is_final.
        this->output = output;
    }

    void changeIsFinal(bool isFinal) {
        this->is_final = isFinal;
    }

    void addTransition(char letter, Node* next, int weight) {
        transitions[letter] = std::make_pair(next, weight);
    }

    // void printTransitions(){
    //     for (const auto& transition : this->transitions) {
    //         char transitionChar = transition.first;
    //         Node* targetNode = transition.second.first;
    //         int weight = transition.second.second;
    //         std::cout << "Transition from: "<< this->id << " to " << targetNode->id << " -> char: "<< transitionChar << " with weight " << weight << " final? " << targetNode->is_final << std::endl;
    //     }
    // }

    void printFST(std::string output = ""){
        for (const auto& transition : this->transitions) {
            char transitionChar = transition.first;
            Node* targetNode = transition.second.first;
            int weight = transition.second.second;
            output = output + transitionChar;
            std::cout << "--------------------------------------------------------" << std::endl;
            std::cout << "Transition from " << this->id << "(" << (this->is_final ? "final" : "not final") << ")" << " to " << targetNode->id << "(" << (targetNode->is_final ? "final" : "not final") << ")" << std::endl;
            std::cout << "Transition char: "<< transitionChar << " with weight " << weight << std::endl;
            std::cout << "Output até então: "<< output << std::endl;
            std::cout << "--------------------------------------------------------" << std::endl;
            targetNode->printFST(output);
            if (!output.empty()) {
                // Cria uma nova string sem a última letra
                output = std::string(output.begin(), output.end() - 1);
            }
        }
        if(this->is_final == true){
            std::cout << "Final state: " << this->id <<"("<<this->is_final<<")" << std::endl;
            std::cout << "OUTPUT FINAL: "<< output << std::endl;

        }
    }

    void getRecommendationsFromState(std::string output = ""){
        if(this->is_final == true && this->transitions.empty()) {
            // std::cout << "vai entrar no estado de id 2: " << this->id << std::endl;
            // std::cout << "Final state: " << this->id <<"("<<this->is_final<<")" << std::endl;
            
            // std::cout <<"Vai printar o aa:" << std::endl; // print o output final no estado terminal
            std::cout << output << std::endl; // print o output final no estado terminal
            if (!output.empty()) {
                // Cria uma nova string sem a última letra
                // output.pop_back(); // se for terminal = final + sem transicao -> remove o char da ultima transicao
                // std::cout <<"O output virou a de novo:" << output << std::endl; // print o output final no estado terminal
            }
            return;
        }
        if(this->is_final == true) {
            // std::cout << "Final state: " << this->id <<"("<<this->is_final<<")" << std::endl;

            // std::cout << "Vai printar o a:" << std::endl;
            std::cout << output << std::endl;
        }
        for (const auto& transition : this->transitions) {
            // pega as informações da transicação
            char transitionChar = transition.first;
            Node* targetNode = transition.second.first;
            int weight = transition.second.second;

            output = output + transitionChar; // coloca o char da transicao no output

            targetNode->getRecommendationsFromState(output);
            output.pop_back();
        }
    }
};

class FST {
private:
    Node* start_state;
    std::vector<Node*> states;

public:
    FST() : start_state(new Node(0, true)) {
        states.push_back(start_state);
    }

    ~FST() {
        for (Node* state : states) {
            delete state;
        }
    }
    // Adiciona um novo nó ao FST, mas ainda não conecta ele com outros nós
    Node* addState(bool isFinal = false, std::string output = "") {
        Node* newState = new Node(states.size(), isFinal, output);
        states.push_back(newState);
        return newState;
    }
    // cria a transição do estado inicial do FST ao novo nó declarado
    void addStartTransition(char letter, Node* target, int weight) {
        start_state->addTransition(letter, target, weight);
    }

    Node* getStartState() const {
        return this->start_state;
    }
// na verdade, esse cara só precisa verificar a ultima transição adicionada, pois as palavras são adicionadas em ordem alfabetica
    void insertNewWord(std::string word){
        this->start_state->is_final = false;
        Node* currentNode = this->start_state;
        bool found = true;
        int transicao = 0;
        while(found && !currentNode->transitions.empty()){
            found = false;
            auto lastTransition = currentNode->transitions.rbegin(); // Obtém um iterador reverso para o último elemento
            char transitionChar = lastTransition->first;
            Node* targetNode = lastTransition->second.first;
            int weight = lastTransition->second.second;
            if(transitionChar == word[transicao]){
                currentNode = targetNode;
                transicao ++;
                found = true;
            }
        }
        // chegou no node que não possui a transição que dá continuação à palavra que queremos inserir
        if(transicao < word.length()){
            Node* newNode;
            // insere até a penultima letra sendo estados nao finais
            for(; transicao < word.length() - 1; transicao++) {
                newNode = this->addState(false);
                currentNode->addTransition(word[transicao], newNode, this->states.size() - 1);
                currentNode = newNode;
            }
            // insere a ultima letra sendo um estado final
            newNode = this->addState(true);
            currentNode->addTransition(word[transicao], newNode, this->states.size() - 1);
            currentNode = newNode;
        }
    }

    void autoCompleteRecommendations(std::string typedWord){
        std::cout << "RECOMENDAÇÕES: " << std::endl;
        Node* currentNode = this->start_state;
        for(char c : typedWord){
            bool found = false;
            for (const auto& transition : currentNode->transitions) {
                char transitionChar = transition.first;
                if(transitionChar == c) {
                    Node* targetNode = transition.second.first;
                    // std::cout << "Encontrou a transicao referente à letra " << transitionChar <<  std::endl;
                    currentNode = targetNode;
                    // std::cout << "O novo currentNode é " << currentNode->id << std::endl;
                    found = true;
                    break;
                }
            }
            if(!found){
                std::cout << "Não há recomendações para esse input" << std::endl;
                return;
            }
        }
        std::cout << "ESTÁ ENVIANDO O RECOMMENDEDSTATE FROM COM O STATE DE ID: " << currentNode->id << std::endl;
        currentNode->getRecommendationsFromState(typedWord);
    }

};

int main() {
    FST myFST;

    Node* startState = myFST.getStartState();

    std::ifstream file("someWords.txt"); // Substitua "seuarquivo.txt" pelo caminho do seu arquivo
    std::vector<std::string> words;
    std::string word;

    if (file.is_open()) {
        while (getline(file, word)) {
            words.push_back(word);
        }
        file.close();
    } else {
        std::cerr << "Não foi possível abrir o arquivo." << std::endl;
    }

    for (const auto& w : words) {
        myFST.insertNewWord(w);
    }

    myFST.autoCompleteRecommendations("A");

    startState->printFST();
    
    return 0;
}

// o map considera os char em ordem na tabela ascii, portanto as aspas simples vem primeiro
// do que todas as maiusculas, em ordem alfabetica, e essas vem antes de todas as minusculas,
// em ordem alfabetica