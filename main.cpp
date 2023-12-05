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
    bool final() const
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

    std::set<std::string> state_output() const{
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

struct StateHasher {
    std::size_t operator()(const Node* node) const {
        std::size_t seed = 0;
        // Hash do estado final
        std::hash<bool> bool_hasher;
        hash_combine(seed, bool_hasher(node->is_final));

        // Hash das transições
        std::hash<char> char_hasher;
        std::hash<Node*> node_hasher;
        for (const auto& transition : node->transitions) {
            if(transition.first != '-'){
                hash_combine(seed, char_hasher(transition.first)); // Usa o caractere da transição
            }
            // hash_combine(seed, node_hasher(transition.second.first)); // Usa o ponteiro do próximo Node
            // Para a string de saída, você pode precisar de uma função de hash para strings
            // std::hash<std::string> string_hasher;
            // hash_combine(seed, string_hasher(transition.second.second));
        }

        // Hash do stateOutput, se relevante
        // ...

        return seed;
    }

private:
    // Combina o hash do valor 'v' com o hash atual 'seed'
    template <typename T>
    inline void hash_combine(std::size_t& seed, const T& v) const {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};

// struct StateEqual {
//     bool operator()(const Node* lhs, const Node* rhs) const {
//         // Verifique se ambos os ponteiros são não-nulos
//         if (lhs == nullptr || rhs == nullptr) {
//             return lhs == rhs; // Ambos nulos são considerados iguais, caso contrário, diferentes
//         }
//         // Compare o estado final, as saídas de estado e as transições de ambos os nós
//         // if(lhs->final() != rhs->final() || lhs->state_output() != rhs->state_output() || lhs->transitions != rhs->transitions){
//         //     return false;
//         // }
//         // return true;
//         if(lhs->final() && rhs->final()){ // se um é final, os dois são finais, por causa do hash... só comparamos ponteiros que possuem o mesmo hash
//             if(lhs->transitions.size() == 1 && rhs->transitions.size() == 1){
//                 if(lhs->transitions.count('-') > 0 && rhs->transitions.count('-') > 0){
//                     return true;
//                 }
//             }
//         }
//         else{
//             Node* lhsNextNode;
//             Node* rhsNextNode;
//             for (const auto& transition : lhs->transitions) {
//                 lhsNextNode = transition.second.first;
//                 rhsNextNode = rhs->transitions.find(transition.first).first;
//                 if(!StateEqual.operator(lhsNextNode, rhsNextNode)){
//                     return false;
//                 }
//             }
//             return true;
//         }
//     }
// };

struct StateEqual {
    bool operator()(const Node* lhs, const Node* rhs) const {
        // Verifique se ambos os ponteiros são não-nulos
        if (lhs == nullptr || rhs == nullptr) {
            return lhs == rhs; // Ambos nulos são considerados iguais, caso contrário, diferentes
        }

        // Se ambos os nós tem finais diferentes
        if (lhs->final() != rhs->final()) {
            return false;
        }

        for (const auto& lhsTransition : lhs->transitions) {
            if(lhsTransition.first != '-'){
                auto rhsTransition = rhs->transitions.find(lhsTransition.first);
                if (rhsTransition == rhs->transitions.end()) {
                    // Se um caractere em lhs não existe em rhs, não são equivalentes
                    return false;
                } else {
                    // Se os nós para um caractere específico não são equivalentes, então os nós não são equivalentes
                    if (!(*this)(lhsTransition.second.first, rhsTransition->second.first)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
};

// struct StateEqual {
//     bool operator()(const Node* lhs, const Node* rhs) const {
//         // Verifique se ambos os ponteiros são não-nulos
//         if (lhs == nullptr || rhs == nullptr) {
//             return lhs == rhs; // Ambos nulos são considerados iguais, caso contrário, diferentes
//         }

//         // Se ambos os nós são finais
//         if (lhs->final() && rhs->final()) {
//             // E ambos têm apenas uma transição com o char '-'
//             if (lhs->transitions.size() == 1 && rhs->transitions.size() == 1) {
//                 auto lhsTransition = lhs->transitions.find('-');
//                 auto rhsTransition = rhs->transitions.find('-');
//                 // Verifique se ambos têm a transição nula '-'
//                 if (lhsTransition != lhs->transitions.end() && rhsTransition != rhs->transitions.end()) {
//                     // E se as transições nulas levam ao mesmo estado (ou equivalente)
//                     return lhsTransition->second.first == rhsTransition->second.first;
//                 }
//             }
//             return false;
//         }

//         // Se não são finais, todas as transições devem ser equivalentes
//         for (const auto& lhsTransition : lhs->transitions) {
//             auto rhsTransition = rhs->transitions.find(lhsTransition.first);
//             if (rhsTransition == rhs->transitions.end()) {
//                 // Se um caractere em lhs não existe em rhs, não são equivalentes
//                 return false;
//             } else {
//                 // Se os nós para um caractere específico não são equivalentes, então os nós não são equivalentes
//                 if (!(*this)(lhsTransition.second.first, rhsTransition->second.first)) {
//                     return false;
//                 }
//             }
//         }
//         return true;
//     }
// };

class FST
{
private:
    Node* start_state;
    std::unordered_map<Node*, int, StateHasher, StateEqual> states; // hash function to find states

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

    Node* member(Node* &refNode) {
        auto it = states.find(refNode);
        if (it != states.end()) {
            // O ponteiro para Node está presente em 'states', então retorne-o
            return refNode;
        }
        std::cout << "A member disse que esse ponteiro par nó não pertence ao dictionary" << std::endl;
        // O ponteiro para Node não foi encontrado, retorna nullptr
        return nullptr;
    }


    // Adiciona um novo nó ao FST, mas ainda não conecta ele com outros nós
    void insert(Node* newNode)
    {
        // if(start_state == nullptr){
        //     start_state = newNode;
        //     // Corrigindo a inserção para usar std::pair
        //     this->states.insert({start_state, states.size()});
        // }
        // else{
            // Corrigindo a inserção para usar std::pair
            this->states.insert({newNode, states.size()});
        // }
    }

    int getNodeID(Node* &node){
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
        // else{
            // Itera por todas as transições do nó de início.
            for (const auto &transition : start->transitions) {
                char transitionChar = transition.first;
                if(transitionChar != '-'){
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
            }
        // }

        arquivo.close();
    }

};

const char FIRST_CHAR = 'a';
const char LAST_CHAR = 'z';
FST MinimalTransducerStatesDitionary;
int MAX_WORD_SIZE;

Node* findMinimized(Node*& s){
    // Supondo que MinimalTransducerStatesDitionary é uma estrutura de dados
    // que armazena ponteiros para Node e possui métodos 'member' e 'insert'.
    Node* r = MinimalTransducerStatesDitionary.member(s);

    if(r == nullptr){
        std::cout << "Tal node não é membro de MinimalTransducerStatesDitionary" << std::endl;
        
        // Copia o estado do nó s e insere a cópia no dicionário.
        r = s->copy_state();
        MinimalTransducerStatesDitionary.insert(r);

        // Atualiza o ponteiro s para apontar para o novo estado.
    }

    // Retorna o ponteiro para o nó minimizado (ou o original, se já estiver minimizado).
    return r;
}


std::pair<std::vector<std::string>, size_t> getInput(std::string filename) {
    std::ifstream file(filename); // Substitua "input.txt" pelo caminho do seu arquivo
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

void cleanOutputFile(std::string fileName){
    std::ofstream file(fileName, std::ios::out);

    if (file.is_open()) {
        // O arquivo foi aberto com sucesso e seu conteúdo foi limpo
        std::cout << "Arquivo limpo com sucesso." << std::endl;
    } else {
        // Houve um erro ao abrir o arquivo
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
    }
}

int main()
{
    int i, j, k, prefixLengthPlusOne;
    char c;
    std::string PreviousWord, CurrentWord, CommonPrefix, currentOutput, WordSuffix, tempString;
    std::set<std::string> tempSet;
    Node* initialState;

    std::pair<std::vector<std::string>, size_t> result = getInput("2wordsEqualSuffixAndPrefix.txt");
    std::vector<std::string> words = result.first;
    MAX_WORD_SIZE = result.second;
    std::vector<Node*> TempStates(MAX_WORD_SIZE+1);
    // std::cout << "MAX_WORD_SIZE: " << MAX_WORD_SIZE << std::endl;

    // for(i=0; i<20; i++){
    //     std::cout << "words[" << i << "]: " << words[i] << std::endl;
    // } 

    // inicial o vetor tempStates
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
        // pega o tamanho do prefixo em comum entre a currentWord e a PreviousWord
        while(j<=CurrentWord.size()-1 && j<=PreviousWord.size()-1 && CurrentWord[j]==PreviousWord[j]){
            j++;
        }
        prefixLengthPlusOne = j; // indice do primeiro char que nao é igual em currentWord e em previousWord
        // we minimize the states from the sufix of the previous word - transcreve os states de tempStates para o dictionary, em novos states em dictionary, de forma que o initial state ainda continue em tempStates, mas o resto da cadeia esteja em dictionary
        for(j = PreviousWord.size()-1; j>=prefixLengthPlusOne; j--){
            std::cout << "Entrou em um for que não era pra entrar";
            TempStates[j]->set_transition(PreviousWord[j], findMinimized(TempStates[j+1]));
        }
        // This loop initializes the tail states for the current word - reseta os states de tempStates, exceto o initialState, e cria as transições entre eles para formar o automato da currentWord em tempStates
        for(j=prefixLengthPlusOne; j<=CurrentWord.size()-1; j++){
            TempStates[j+1]->clear_state();
            // 
            TempStates[j]->set_transition(CurrentWord[j], TempStates[j+1]);
        }
        // define o ultimo nó do automato da currentWord, que está em TempStates, como final
        if(CurrentWord != PreviousWord){
            TempStates[CurrentWord.size()]->set_final(true);
            TempStates[CurrentWord.size()]->set_output('-', ""); // Espaço vazio como output se necessário.
        }
        // só entra se tiver prefixo em comum
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
            // esse current output está fazendo nada... tá nulo ""... ao input de 2wordsEqualSuffix, na rodada da palavra "bbc" era pra esse currentOutput ser "1", pois "bbc" é a palavra de indice 1 da lista de palavras
            TempStates[prefixLengthPlusOne]->set_output(CurrentWord[prefixLengthPlusOne], currentOutput);
        }
        PreviousWord = CurrentWord;
    }
        // aqui minimizamos os estados da ultima palavra - percorre a currentWord de trás pra frente - AQUI ESTÁ A CHAMADA DE MINIMIZAÇÃO QUE NÃO FUNICONA- DEBUGAR A PARTIR DAQUI
        // TO DO - VERIFICAR AS ADIÇÕES DOS ESTADOS QUE ESTÃO EM TEMPSTATES PARA O MINIMALTRANSDUCERDICTIONARY
    for(i = CurrentWord.size()-1; i>=0; i--){
        // if(TempStates[i+1]->compare_states(findMinimized(TempStates[i+1]))){
        //     TempStates[i]->set_transition(PreviousWord[i], TempStates[i+1]);
        // }
        // else{
            TempStates[i]->set_transition(PreviousWord[i], findMinimized(TempStates[i+1]));
        // }
    }

    // if(TempStates[0]->compare_states(findMinimized(TempStates[0]))){
    //     initialState = TempStates[0];
    // }
    // else{
        initialState = findMinimized(TempStates[0]);
    // }

    std::cout << "Terminou" << std::endl;

    // TO DO: TESTAR COM MAIS DE UMA PALAVRA: tipo "a" e "b" ou "aa" e "ab"

    cleanOutputFile("printFST.txt");
    MinimalTransducerStatesDitionary.print_transducer(initialState, "printFST.txt");
}

// o map considera os char em ordem na tabela ascii, portanto as aspas simples vem primeiro
// do que todas as maiusculas, em ordem alfabetica, e essas vem antes de todas as minusculas,
// em ordem alfabetica

// TO DO: A FUNÇÃO MEMBER SÓ RETORNA TRUE SE O PONTEIRO FOR O MESMO QUE O ENVIADO NO PARAMETRO
// OPÇÕES: ALTERAR A MEMBER E A FINDMINIMIZED, PARA FUNCIONAREM COM PONTEIROS DIFERENTES QUE APONTAM PARA O MESMO NÓ
// OU DAR UM JEITO DE MANDAR O MESMO PONTEIRO 
// to do kalil - construir a gui para as recomendações que já estão feitas
// to do kalil - comparar com outro metodo - arvore binaria ou hashtable (tem que ver na planilha do bizarro quais ainda estao disponiveis)
// to do kalil - implementar o automato de levenshtein
