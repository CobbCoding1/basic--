#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

using namespace std;

enum class TokenType{
    TYPE_INT,
    TYPE_PLUS,
    TYPE_MINUS,
};

enum class Operator{
    TYPE_ADD,
    TYPE_SUB,
};

string print_token_type(TokenType type){
    switch(type){
        case TokenType::TYPE_INT:
            return "int";
            break;
        case TokenType::TYPE_PLUS:
            return "plus";
            break;
        case TokenType::TYPE_MINUS:
            return "minus";
            break;
    }
    return "none";
}

struct Token{
    TokenType type;
    string text;
};

class Lexer{
public: 
    queue<Token> tokens;
    void push_token(Token token){
        tokens.push(token);
    }
    void print_tokens(){
        while(!tokens.empty()){
            cout << tokens.front().text << endl;
            tokens.pop();
        }
    }
};

class Node{
public:
    TokenType op;
    string left;
    string right;
};

class Parser{
public:
    Lexer lexer;
    Node *tree;
    Node *current_node;

    Parser(){
        tree = new Node;
        current_node = tree;
    }

    void parse_lexer(){
        while(!lexer.tokens.empty()){
            Token current = get_next_token();
            Token next;
            switch(current.type){
                case TokenType::TYPE_INT:
                    next = get_next_token();
                    if(!is_operator(next)){
                        cerr << "error: expected operator" << endl;
                        exit(1);
                    }
                    current_node->left = current.text;
                    current_node->op = next.type;
                    next = get_next_token();
                    if(next.type != TokenType::TYPE_INT){
                        cerr << "error: expected int" << endl;
                        exit(1);
                    }
                    current_node->right = next.text;
                    break;
                case TokenType::TYPE_PLUS:
                    current_node->op = current.type; 
                    break;
                case TokenType::TYPE_MINUS:
                    current_node->op = current.type;
                    break;
            }
        }
        print_tree();
    }
private: 
    void print_tree(){
        current_node = tree;
        cout << current_node->left << print_token_type(current_node->op) << current_node->right << endl;
    }
    bool is_operator(Token token){
        if(token.type == TokenType::TYPE_PLUS || token.type == TokenType::TYPE_MINUS){
            return true;
        }
        return false;
    }
    Token get_next_token(){
        Token token = lexer.tokens.front();
        lexer.tokens.pop();
        return token;
    }
    Token peek_next_token(){
        return lexer.tokens.front();
    }
};

#define PRINT_ERROR(message) cerr << message << endl; exit(1)

vector<char> read_file_to_buffer(string filename){
    ifstream file(filename, ios::binary | ios::ate);
    if(!file){
        PRINT_ERROR("error: could not open file");
    }
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(size);
    if(file.read(buffer.data(), size)){
        return buffer;
    }
    PRINT_ERROR("error: could not read from file");
}

string generate_number(vector<char> buffer, int *index){
    string digit;
    while(isdigit(buffer[*index])){
        digit += buffer[*index]; 
        *index += 1;
    }
    *index -= 1;
    return digit;
}

Lexer lex_file(string filename){
    Lexer lexer;
    vector<char> buffer = read_file_to_buffer(filename);
    int size = buffer.size();
    for(int i = 0; i < size; i++){
        if(isdigit(buffer[i])){
            string digit = generate_number(buffer, &i);
            lexer.push_token({.type = TokenType::TYPE_INT, .text = digit});
        } else if(buffer[i] == '+'){
            lexer.push_token({.type = TokenType::TYPE_PLUS, .text = "+"});
        } else if(buffer[i] == '-'){
            lexer.push_token({.type = TokenType::TYPE_MINUS, .text = "-"});
        }
    }
    return lexer;
}

int main(){
    Lexer lexer = lex_file("test.basic");
    Parser parser;
    parser.lexer = lexer;
    parser.parse_lexer();
    return 0;
}
