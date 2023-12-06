#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <utility> // std::pair
#include <string>
#include <fstream>
#include <set>
#include <queue>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <ncurses.h>

class Node
{
public:
    // int id;
    // O mapa agora associa um par de char e int (peso da transição) a um Node*.
    std::map<char, Node *> transitions; // Agora usando std::map - esse char é o transition outuput, ou só "output"
    bool is_final;
    // std::set<std::string> stateOutput; // isso aqui é o node output, ou "state output" - somente diferente de '' se isfinal for true

    Node(bool isFinal = false, std::set<std::string> stateOutput = {})
    {
        // this->id = id;            // Correto: atribui o parâmetro id ao membro da classe id.
        this->is_final = isFinal; // Correto: atribui o parâmetro isFinal ao membro da classe is_final.
        // this->stateOutput = stateOutput;
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
            return it->second;
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
        this->transitions[inputChar] = targetState;
    }

    Node *copy_state()
    {
        Node *newNode = new Node(this->is_final /*, this->stateOutput*/);
        newNode->transitions = this->transitions;
        return newNode;
    }

    // Limpa as transições do nó e define isFinal como false
    void clear_state()
    {
        this->transitions.clear();
        this->set_final(false);
        // this->stateOutput = {};
    }

    bool compare_states(Node *anotherState)
    {
        if (this->final() != anotherState->final() /*|| this->state_output() != anotherState->state_output() */ || this->transitions != anotherState->transitions)
        {
            return false;
        }
        return true;
    }
};

struct StateHasher
{
    std::size_t operator()(const Node *node) const
    {
        std::size_t seed = 0;
        // Hash do estado final
        std::hash<bool> bool_hasher;
        hash_combine(seed, bool_hasher(node->is_final));

        // Hash das transições
        std::hash<char> char_hasher;
        std::hash<Node *> node_hasher;
        for (const auto &transition : node->transitions)
        {
            hash_combine(seed, char_hasher(transition.first)); // Usa o caractere da transi ção
            hash_combine(seed, bool_hasher(transition.second->final()));
            hash_combine(seed, node_hasher(transition.second)); // Usa o ponteiro do próximo Node
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
    inline void hash_combine(std::size_t &seed, const T &v) const
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};

struct StateEqual
{
    bool operator()(const Node *lhs, const Node *rhs) const
    {
        // Verifique se ambos os ponteiros são não-nulos
        if (lhs == nullptr || rhs == nullptr)
        {
            return lhs == rhs; // Ambos nulos são considerados iguais, caso contrário, diferentes
        }

        // Se ambos os nós tem finais diferentes
        if (lhs->final() != rhs->final())
        {
            return false;
        }

        for (const auto &lhsTransition : lhs->transitions)
        {
            // if(lhsTransition.first != '-'){
            auto rhsTransition = rhs->transitions.find(lhsTransition.first);
            if (rhsTransition == rhs->transitions.end())
            {
                // Se um caractere em lhs não existe em rhs, não são equivalentes
                return false;
            }
            else
            {
                // Se os nós para um caractere específico não são equivalentes, então os nós não são equivalentes
                if (!(*this)(lhsTransition.second, rhsTransition->second))
                {
                    return false;
                }
            }
            // }
        }
        return true;
    }
};

class FST
{
private:
    Node *start_state;
    std::unordered_map<Node *, int, StateHasher, StateEqual> states; // hash function to find states

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

    Node *member(Node *&refNode)
    {
        auto it = states.find(refNode);
        if (it != states.end())
        {
            // O ponteiro para Node está presente em 'states', então retorne-o
            return it->first;
        }
        // std::cout << "A member disse que esse ponteiro par nó não pertence ao dictionary" << std::endl;
        // O ponteiro para Node não foi encontrado, retorna nullptr
        return nullptr;
    }

    // Adiciona um novo nó ao FST, mas ainda não conecta ele com outros nós
    void insert(Node *newNode)
    {
        this->states.insert({newNode, states.size()});
    }

    int getNodeID(Node *&node)
    {
        auto it = states.find(node); // Use find para verificar se o node está presente
        if (it != states.end())
        {
            // Se o node foi encontrado, retorne o valor associado, que é o ID do node
            return it->second; // it é um iterador para um par (key, value)
        }
        return -1;
    }

    void generateDotFile(Node *start, const std::string &filename)
    {
        std::ofstream out(filename, std::ios::app); // Abrir o arquivo apenas uma vez

        if (!out.is_open())
        {
            std::cerr << "Não foi possível abrir o arquivo." << std::endl;
            return;
        }

        out << "digraph G {\n";
        out << "    rankdir=LR;\n";

        if (start == nullptr)
        {
            // Se o nó de início não for válido, encerre a função.
            out.close();
            return;
        }

        std::queue<std::tuple<Node *, char, Node *>> queue;
        std::set<std::tuple<Node *, char, Node *>> addedTransitions;
        char transitionChar;
        Node *targetNode;

        // Processa as transições iniciais
        for (const auto &transition : start->transitions)
        {
            transitionChar = transition.first;
            targetNode = transition.second;
            std::tuple<Node *, char, Node *> myTransition = std::make_tuple(start, transitionChar, targetNode);

            if (/*transitionChar != '-' && */ addedTransitions.find(myTransition) == addedTransitions.end())
            {
                addedTransitions.insert(myTransition);
                out << "    " << getNodeID(start) << " -> " << getNodeID(targetNode) << " [label=\"" << transitionChar << "\"];\n";
                queue.push(myTransition);
            }
        }

        while (!queue.empty())
        {
            std::tuple<Node *, char, Node *> currentTransition = queue.front();
            queue.pop();

            Node *currentNode = std::get<2>(currentTransition);

            if (currentNode->final())
            {
                out << "    " << getNodeID(currentNode) << " [penwidth=3];\n";
            }

            for (const auto &trans : currentNode->transitions)
            {
                char transitionChar = trans.first;
                targetNode = trans.second;
                std::tuple<Node *, char, Node *> nextTransition = std::make_tuple(currentNode, transitionChar, targetNode);

                if (/*transitionChar != '-' && */ addedTransitions.find(nextTransition) == addedTransitions.end())
                {
                    addedTransitions.insert(nextTransition);
                    out << "    " << getNodeID(currentNode) << " -> " << getNodeID(targetNode) << " [label=\"" << transitionChar << "\"];\n";
                    queue.push(nextTransition);
                }
            }
        }

        out << "}\n";
        out.close();
    }

    void get_final_strings(Node *start, std::vector<std::string> &recomendations, std::string output = "")
    {
        if (start == nullptr)
        {
            // Se o nó de início não for válido, encerre a função.
            return;
        }

        if (recomendations.size() == 10)
        {
            // Se já coletou 10 recomendações, pare a execução.
            return;
        }

        // Se for um nó final, adicione a palavra ao vetor de recomendações.
        if (start->final())
        {
            recomendations.push_back(output);
        }

        // Itera por todas as transições do nó de início.
        for (const auto &transition : start->transitions)
        {
            char transitionChar = transition.first;
            Node *targetNode = transition.second;
            get_final_strings(targetNode, recomendations, output + transitionChar);
        }
        return;
    }

    Node *get_start_search_node(Node *start, std::string search_string)
    {

        for (char c : search_string)
        {
            auto it = start->transitions.find(c);
            if (it != start->transitions.end())
            {
                // Retorna o estado de destino se a transição for encontrada.
                start = it->second;
            }
            else
            {
                // Retorna nullptr se não houver transição para o caractere de entrada.
                start = nullptr;
                break;
            }
        }
        return start;
    }
};

FST MinimalTransducerStatesDitionary;
int MAX_WORD_SIZE;

Node *findMinimized(Node *&s)
{
    // Supondo que MinimalTransducerStatesDitionary é uma estrutura de dados
    // que armazena ponteiros para Node e possui métodos 'member' e 'insert'.
    Node *r = MinimalTransducerStatesDitionary.member(s);

    if (r == nullptr)
    {
        // std::cout << "Tal node não é membro de MinimalTransducerStatesDitionary" << std::endl;

        // Copia o estado do nó s e insere a cópia no dicionário.
        r = s->copy_state();
        MinimalTransducerStatesDitionary.insert(r);

        // Atualiza o ponteiro s para apontar para o novo estado.
    }

    // Retorna o ponteiro para o nó minimizado (ou o original, se já estiver minimizado).
    return r;
}

std::pair<std::vector<std::string>, size_t> getInput(const std::string &filename)
{
    std::ifstream file(filename);
    std::vector<std::string> words;
    std::string word;
    size_t maxLength = 0;

    if (!file.is_open())
    {
        std::cerr << "Não foi possível abrir o arquivo." << std::endl;
        return std::make_pair(std::vector<std::string>(), 0);
    }

    while (file >> word)
    {
        words.push_back(word);
        if (word.length() > maxLength)
        {
            maxLength = word.length();
        }
    }

    file.close();

    return std::make_pair(words, maxLength);
}

void writeSortedWordsToFile(const std::vector<std::string> &words, const std::string &outputFilename)
{
    std::ofstream outputFile(outputFilename);

    if (!outputFile.is_open())
    {
        std::cerr << "Não foi possível abrir o arquivo para escrita." << std::endl;
        return;
    }

    for (const auto &word : words)
    {
        outputFile << word << "\n";
    }

    outputFile.close();
}

void cleanOutputFile(std::string fileName)
{
    std::ofstream file(fileName, std::ios::out);

    if (file.is_open())
    {
        // O arquivo foi aberto com sucesso e seu conteúdo foi limpo
        std::cout << "Arquivo limpo com sucesso." << std::endl;
    }
    else
    {
        // Houve um erro ao abrir o arquivo
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
    }
}

int levenshteinDistance(const std::string &s1, const std::string &s2)
{
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));

    for (size_t i = 0; i <= len1; ++i)
    {
        for (size_t j = 0; j <= len2; ++j)
        {
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

int main()
{

    /*************************************************LEVEINSTEIN**************************************************************************/
    std::ifstream dictionaryFile("./simpleInput/american-english.txt");
    if (!dictionaryFile)
    {
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
    while (dictionaryFile >> word)
    {
        int distance = levenshteinDistance(targetWord, word);
        if (distance <= distanceThreshold)
        {
            closeWords.push_back(word);
        }
    }

    /*************************************************LEVEINSTEIN**************************************************************************/
    /*****************************************************FST********************************************************************************/
    int i, j, k, prefixLengthPlusOne;
    char c;
    std::string PreviousWord, CurrentWord, tempString;
    Node *initialState;

    std::string inputFileName = "./simpleInput/american-english-noaccent.txt";

    std::pair<std::vector<std::string>, size_t> result = getInput(inputFileName);
    std::vector<std::string> words = result.first;
    MAX_WORD_SIZE = result.second;
    std::sort(words.begin(), words.end());
    std::vector<Node *> TempStates(MAX_WORD_SIZE + 1);

    // inicial o vetor tempStates
    for (i = 0; i <= MAX_WORD_SIZE; i++)
    {
        TempStates[i] = new Node();
    }

    PreviousWord = "";
    TempStates[0]->clear_state();

    auto start = std::chrono::high_resolution_clock::now();

    // esse for é equivalente ao while do pseudocodigo
    for (i = 0; i < words.size(); i++)
    {
        CurrentWord = words[i];
        j = 0;
        // pega o tamanho do prefixo em comum entre a currentWord e a PreviousWord
        while (j <= CurrentWord.size() - 1 && j <= PreviousWord.size() - 1 && CurrentWord[j] == PreviousWord[j])
        {
            j++;
        }
        prefixLengthPlusOne = j; // indice do primeiro char que nao é igual em currentWord e em previousWord
        // transcreve os states de tempStates para o dictionary, em novos states em dictionary, de forma que o initial state ainda continue em tempStates, mas o resto da cadeia esteja em dictionary
        for (j = PreviousWord.size() - 1; j >= prefixLengthPlusOne; j--)
        {
            // std::cout << "Entrou em um for que não era pra entrar";
            TempStates[j]->set_transition(PreviousWord[j], findMinimized(TempStates[j + 1]));
        }
        // reseta os states de tempStates, exceto o initialState, e cria as transições entre eles para formar o automato da currentWord em tempStates
        for (j = prefixLengthPlusOne; j <= CurrentWord.size() - 1; j++)
        {
            TempStates[j + 1]->clear_state();
            //
            TempStates[j]->set_transition(CurrentWord[j], TempStates[j + 1]);
        }
        // define o ultimo nó do automato da currentWord, que está em TempStates, como final
        if (CurrentWord != PreviousWord)
        {
            TempStates[CurrentWord.size()]->set_final(true);
            // TempStates[CurrentWord.size()]->set_output('-', ""); // Espaço vazio como output se necessário.
        }
        PreviousWord = CurrentWord;
    }
    // percorre a currentWord de trás pra frente e vai mandando pro minimalTransducerDictionary
    for (i = CurrentWord.size() - 1; i >= 0; i--)
    {
        TempStates[i]->set_transition(PreviousWord[i], findMinimized(TempStates[i + 1]));
    }

    initialState = findMinimized(TempStates[0]);

    /***************************FINAL FST*********************************************************************************/

    std::vector<std::string> recommendations;

    for (const auto &closeWord : closeWords)
    {
        if(closeWord.substr(0, targetWord.size()) == targetWord){

            Node* startSearchNode = MinimalTransducerStatesDitionary.get_start_search_node(initialState, closeWord);

            if(startSearchNode != nullptr){
                recommendations.push_back(closeWord);
                std::cout << closeWord << std::endl;
            }
        }
    }

    return 0;
}
