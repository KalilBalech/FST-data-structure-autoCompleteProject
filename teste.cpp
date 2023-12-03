Node* member(Node* refNode) {
    auto it = states.find(refNode);
    if (it != states.end()) {
        // O ponteiro para Node está presente em 'states', então retorne-o
        return refNode;
    }
    // O ponteiro para Node não foi encontrado, retorna nullptr
    return nullptr;
}

Node* findMinimized(Node* s){
    Node* r = MinimalTransducerStatesDitionary.member(s);
    if(r == nullptr){
        r = s->copy_state();
        MinimalTransducerStatesDitionary.insert(r);
        return r;
    }
    return s;
}