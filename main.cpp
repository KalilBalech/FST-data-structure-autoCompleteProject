#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>      // std::pair
#include <string>


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

    void printTransitions(){
        for (const auto& transition : this->transitions) {
            char transitionChar = transition.first;
            Node* targetNode = transition.second.first;
            int weight = transition.second.second;
            std::cout << "Transition from: "<< this->id << " to " << targetNode->id << " -> char: "<< transitionChar << " with weight " << weight << " final? " << targetNode->is_final << std::endl;
        }
    }

    void printFST(Node* startState){
        for (const auto& transition : startState->transitions) {
            char transitionChar = transition.first;
            Node* targetNode = transition.second.first;
            int weight = transition.second.second;
            targetNode->printFST(targetNode);
            std::cout << "--------------------------------------------------------" << std::endl;
            std::cout << "Transition from "<< startState->id <<"("<<startState->is_final<<")" << " to " << targetNode->id <<"("<<targetNode->is_final<<")" << std::endl;
            std::cout << "Transition char: "<< transitionChar << " with weight " << weight << std::endl;
            std::cout << "--------------------------------------------------------" << std::endl;
        }
        if(startState->is_final == true){
            std::cout << "Final state: " << startState->id <<"("<<startState->is_final<<")" << std::endl;
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

    void insertNewWord(std::string word){

        Node* currentNode = this->start_state;
        bool found = true;
        int transicao = 0;
        // Iterar desde o startState para encontrar o prefixo da palavra que já se encontra no FST
        while(found && !currentNode->is_final){
            found = false;
            for (const auto& transition : currentNode->transitions) {
                char transitionChar = transition.first;
                Node* targetNode = transition.second.first;
                if(transitionChar == word[transicao]) {
                    std::cout << "encontrou a transicao " << transitionChar <<  std::endl;
                    std::cout << "Is next node final? " << targetNode->is_final <<  std::endl;
                    currentNode = targetNode;
                    transicao ++;
                    found = true;
                    break;
                }
            }
        }
        // chegou no node que não possui a transição que dá continuação à palavra que queremos inserir
        if(transicao < word.length()){
            currentNode->is_final = false;
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

};

int main() {
    FST myFST;

    Node* startState = myFST.getStartState();

    myFST.insertNewWord("a");
    myFST.insertNewWord("b");
    myFST.insertNewWord("c");

    myFST.insertNewWord("ab");

    startState->printFST(startState);

    return 0;
}