#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <utility> // std::pair
#include <string>
#include <fstream>
#include <set>

class Node
{
public:
    // int id;
    // O mapa agora associa um par de char e int (peso da transição) a um Node*.
    std::map<char, std::pair<Node *, std::string>> transitions; // Agora usando std::map - esse char é o transition outuput, ou só "output"
    bool is_final;
    std::set<std::string> stateOutput; // isso aqui é o node output, ou "state output" - somente diferente de '' se isfinal for true

    Node(bool isFinal = false, std::set<std::string> stateOutput = {})
    {
        // this->id = id;            // Correto: atribui o parâmetro id ao membro da classe id.
        this->is_final = isFinal; // Correto: atribui o parâmetro isFinal ao membro da classe is_final.
        this->stateOutput = stateOutput;
    }

    // Define um novo valor para isFinal do Node
    bool final()
    {
        return this->is_final;
    }
    
    void set_final(bool isFinal)
    {
        this->is_final = isFinal;
    }

    // Dado um char, retorna o targetState definido pela transição
    Node *transition(char inputChar)
    {
        auto it = this->transitions.find(inputChar);
        if (it != this->transitions.end())
        {
            // Retorna o estado de destino se a transição for encontrada.
            return it->second.first;
        }
        else
        {
            // Retorna nullptr se não houver transição para o caractere de entrada.
            return nullptr;
        }
    }

    // Dado um char e um state, define uma transição
    void set_transition(char inputChar, Node *targetState, std::string transitionOutput = "")
    {
        this->transitions[inputChar] = std::make_pair(targetState, transitionOutput);
    }

    std::set<std::string> state_output(){
        return this->stateOutput;
    }

    void set_state_output(std::set<std::string> stateOutput){
        this->stateOutput = stateOutput;
    }

    //  return the output string for a transitionChar
    std::string output(char transitionChar){
        return this->transitions[transitionChar].second;
    }

    void set_output(char transitionChar, std::string transitionOutput){
        this->transitions[transitionChar].second = transitionOutput;
    }

    Node* copy_state(){
        Node* newNode = new Node(this->is_final, this->stateOutput);
        newNode->transitions = this->transitions;
        return newNode;
    }

    // Limpa as transições do nó e define isFinal como false
    void clear_state()
    {
        this->transitions.clear();
        this->set_final(false);
        this->stateOutput = {};
    }

    bool compare_states(Node* anotherState){
        if(this->final() != anotherState->final() || this->state_output() != anotherState->state_output() || this->transitions != anotherState->transitions){
            return false;
        }
        return true;
    }
};

class FST
{
private:
    Node* start_state;
    std::unordered_map<Node*, int> states; // hash function to find states

public:
    FST() : start_state(nullptr) // start_state é inicializado como nullptr
    {
        // Não adicionar nenhum estado em 'states', deixando-o vazio
    }

    ~FST()
    {
        for (auto &state : states)
        {
            delete state.first;
        }
    }

    Node* member(Node* refNode) {
        auto it = states.find(refNode);
        if (it != states.end()) {
            // O ponteiro para Node está presente em 'states', então retorne-o
            return refNode;
        }
        // O ponteiro para Node não foi encontrado, retorna nullptr
        return nullptr;
    }


    // Adiciona um novo nó ao FST, mas ainda não conecta ele com outros nós
    void insert(Node* newNode)
    {
        if(start_state == nullptr){
            start_state = newNode;
            // Corrigindo a inserção para usar std::pair
            this->states.insert({start_state, states.size()});
        }
        else{
            // Corrigindo a inserção para usar std::pair
            this->states.insert({newNode, states.size()});
        }
    }

    int getNodeID(Node* node){
        auto it = states.find(node); // Use find para verificar se o node está presente
        if (it != states.end()) {
            // Se o node foi encontrado, retorne o valor associado, que é o ID do node
            return it->second; // it é um iterador para um par (key, value)
        }
        return -1;
    }

    void print_transducer(Node* start, std::string fileName, std::string output = "") {
        std::ofstream arquivo(fileName, std::ios::app);
        if (!arquivo.is_open()) {
            std::cerr << "Não foi possível abrir o arquivo." << std::endl;
            return;
        }

        if (start == nullptr) {
            // Se o nó de início não for válido, encerre a função.
            arquivo.close();
            return;
        }

        // Se for um nó final, imprimir isso.
        if (start->final()) {
            arquivo << "Final state: " << getNodeID(start) << std::endl;
        }

        // Itera por todas as transições do nó de início.
        for (const auto &transition : start->transitions) {
            char transitionChar = transition.first;
            Node* targetNode = transition.second.first;
            std::string transitionOutput = transition.second.second;

            // Imprime a transição atual.
            arquivo << "--------------------------------------------------------" << std::endl;
            arquivo << "Transition from " << getNodeID(start) << " to " << getNodeID(targetNode) << std::endl;
            arquivo << "Transition char: " << transitionChar << std::endl;
            arquivo << "Transition output: " << transitionOutput << std::endl;
            arquivo << "--------------------------------------------------------" << std::endl;

            // Chama recursivamente para imprimir a partir do nó de destino.
            print_transducer(targetNode, fileName, output + transitionOutput);
        }

        arquivo.close();
    }

};

const char FIRST_CHAR = 'a';
const char LAST_CHAR = 'z';
FST MinimalTransducerStatesDitionary;
int MAX_WORD_SIZE;

Node* findMinimized(Node* s){
    Node* r = MinimalTransducerStatesDitionary.member(s);
    if(r == nullptr){
        r = s->copy_state();
        MinimalTransducerStatesDitionary.insert(r);
        return r;
    }
    return s;
}

std::pair<std::vector<std::string>, size_t> getInput() {
    std::ifstream file("1word.txt"); // Substitua "input.txt" pelo caminho do seu arquivo
    std::vector<std::string> words;
    std::string word;
    size_t maxLength = 0;

    if (!file.is_open()) {
        std::cout << "Não foi possível abrir o arquivo." << std::endl;
        // Aqui você pode optar por retornar um par vazio ou lançar uma exceção
        return std::make_pair(std::vector<std::string>(), 0);
    }

    while (file >> word) {
        words.push_back(word);
        if (word.length() > maxLength) {
            maxLength = word.length();
        }
    }

    file.close();

    return std::make_pair(words, maxLength); // Correção aqui
}

std::string eraseSubString(std::string stringReference, std::string subString){
    std::string result = stringReference;
    // Encontra a posição inicial de subString.
    size_t start_pos = result.find(subString);
    // Se subString for encontrada, apaga-a.
    if (start_pos != std::string::npos) {
        result.erase(start_pos, subString.length());
    }
    // Se subString não for encontrada, retorna a string original.
    return result;
}


int main()
{
    int i, j, k, prefixLengthPlusOne;
    char c;
    std::string PreviousWord, CurrentWord, CommonPrefix, currentOutput, WordSuffix, tempString;
    std::set<std::string> tempSet;
    Node* initialState;

    std::pair<std::vector<std::string>, size_t> result = getInput();
    std::vector<std::string> words = result.first;
    MAX_WORD_SIZE = result.second;
    std::vector<Node*> TempStates(MAX_WORD_SIZE+1);
    // std::cout << "MAX_WORD_SIZE: " << MAX_WORD_SIZE << std::endl;

    // for(i=0; i<20; i++){
    //     std::cout << "words[" << i << "]: " << words[i] << std::endl;
    // } 

    for(i = 0; i<=MAX_WORD_SIZE; i++){
        TempStates[i] = new Node();
        std::cout << i << std::endl;
    }
    
    PreviousWord = "";
    currentOutput = "";
    TempStates[0]->clear_state();

    std::cout << "MAX_WORD_SIZE: " << MAX_WORD_SIZE << std::endl;

    for(i = 0; i<words.size(); i++){
        std::cout << " words[" << i << "]: " << words[i] << std::endl;
    }

    std::cout << "Que comecem os jogos" << std::endl;

    // esse for é equivalente ao while do pseudocodigo
    for(i=0; i<words.size(); i++){
        CurrentWord = words[i];
        j = 0;
        while(j<CurrentWord.size()-1 && j<PreviousWord.size()-1 && CurrentWord[j]==PreviousWord[j]){
            j++;
        }
        prefixLengthPlusOne = j; // indice do primeiro char que nao é igual em currentWord e em previousWord
        // we minimize the states from the sufix of the previous word
        for(j = PreviousWord.size()-1; j>=prefixLengthPlusOne; j--){
            std::cout << "Entrou em um for que não era pra entrar";
            TempStates[j]->set_transition(PreviousWord[j], findMinimized(TempStates[j+1]));
        }
        // This loop initializes the tail states for the current word
        for(j=prefixLengthPlusOne; j<=CurrentWord.size()-1; j++){
            TempStates[j+1]->clear_state();
            // AGORA O ERRO ESTÁ AQUI! J-2 NÃO EXISTE NO INICIO
            TempStates[j]->set_transition(CurrentWord[j], TempStates[j+1]);
        }
        if(CurrentWord != PreviousWord){
            TempStates[CurrentWord.size()]->set_final(true);
            TempStates[CurrentWord.size()]->set_output('-', ""); // Espaço vazio como output se necessário.
        }
        for(k=0; k <= prefixLengthPlusOne-1; k++){
            // definir commonPrefix como o prefixo comum entre current e previous
            // definir wordSuffix como o resto da current
            CommonPrefix = TempStates[k]->output(CurrentWord[k]) + currentOutput;
            WordSuffix = eraseSubString(CommonPrefix, TempStates[k]->output(CurrentWord[k]));
            TempStates[k]->set_output(CurrentWord[k], CommonPrefix);
            for(c = FIRST_CHAR; c <= LAST_CHAR; c++){
                if(TempStates[k+1]->transition(c) != nullptr){
                    TempStates[k+1]->set_output(c, WordSuffix + TempStates[k+1]->output(c));
                }
            }
            if(TempStates[k+1]->final()){
                tempSet.clear();
                for (const auto& tempString : TempStates[k+1]->state_output()) {
                    tempSet.insert(WordSuffix + tempString);
                }
                TempStates[k+1]->set_state_output(tempSet);
            }
            currentOutput = eraseSubString(CommonPrefix, currentOutput);
        }
        if(CurrentWord == PreviousWord){
            std::set<std::string> currentStateOutput = TempStates[CurrentWord.size()]->state_output();
            currentStateOutput.insert(currentOutput); 
            TempStates[CurrentWord.size()]->set_state_output(currentStateOutput);
        }
        else{
            TempStates[prefixLengthPlusOne]->set_output(CurrentWord[prefixLengthPlusOne], currentOutput);
        }
        PreviousWord = CurrentWord;
    }
        // aqui minimizamos os estados da ultima palavra
    for(i = CurrentWord.size()-1; i>=0; i--){
        if(TempStates[i+1]->compare_states(findMinimized(TempStates[i+1]))){
            TempStates[i]->set_transition(PreviousWord[i], TempStates[i+1]);
        }
        else{
            TempStates[i]->set_transition(PreviousWord[i], findMinimized(TempStates[i+1]));
        }
    }

    if(TempStates[0]->compare_states(findMinimized(TempStates[0]))){
        initialState = TempStates[0];
    }
    else{
        initialState = findMinimized(TempStates[0]);
    }

    // std::cout << MinimalTransducerStatesDitionary.getNodeID(TempStates[0]) << std::endl;

    // MinimalTransducerStatesDitionary.print_transducer(initialState, "printFST.txt");



    // if(TempStates[0] == findMinimized(TempStates[0])){
    //     std::cout << "São iguais mesmo" << std::endl;
    // }
    // else{
    //     std::cout << "Não são iguais porra" << std::endl;
    // }

    // if(TempStates[0]->compare_states(findMinimized(TempStates[0]))){
    //     std::cout << "São iguais mesmo" << std::endl;
    // }
    // else{
    //     std::cout << "Não são iguais porra" << std::endl;
    // }

    // Node* newNode = findMinimized(TempStates[0]);

    // std::cout << newNode->final() << std::endl;
    // std::cout << TempStates[0]->final() << std::endl;

    // std::cout << newNode->transitions['a'].first << std::endl;
    // std::cout << TempStates[0]->transitions['a'].first << std::endl;

}

// o map considera os char em ordem na tabela ascii, portanto as aspas simples vem primeiro
// do que todas as maiusculas, em ordem alfabetica, e essas vem antes de todas as minusculas,
// em ordem alfabetica

// to do kalil - construir a gui para as recomendações que já estão feitas
// to do kalil - comparar com outro metodo - arvore binaria ou hashtable (tem que ver na planilha do bizarro quais ainda estao disponiveis)
// to do kalil - implementar o automato de levenshtein
