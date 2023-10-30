#include <iostream>
#include <fstream>
#include <vector>
#include <variant>
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

struct IntLiteral{
    string value;
};

struct BinExpr{
    TokenType op;
    std::variant<BinExpr*, IntLiteral*> left;
    std::variant<BinExpr*, IntLiteral*> right;
};


class Parser{
public:
    Lexer lexer;
    BinExpr *tree;
    BinExpr *current_node;

    Parser(){
        tree = new BinExpr();
        current_node = tree;
    }

    void parse_lexer(){
        while(!lexer.tokens.empty()){
            tree = generate_expr();
        }
        print_tree(tree);
    }
private: 
    BinExpr *generate_expr(){
        BinExpr *expr = generate_term();
        while(!lexer.tokens.empty()){
            Token token = get_next_token();
            if(!is_operator(token)){
                break;
            }
            BinExpr *new_expr = new BinExpr();
            new_expr->op = token.type;
            new_expr->left = expr;
            new_expr->right = generate_term();
            if(get<BinExpr*>(new_expr->right) == nullptr){
                break;
            }
            expr = new_expr;
        }
        return expr;
    }
    BinExpr *generate_term(){
        Token token = get_next_token();
        if(token.type == TokenType::TYPE_INT){
            if(token.text == ""){
                return nullptr;
            }
            IntLiteral* literal = new IntLiteral;
            literal->value = token.text;
            BinExpr *expr = new BinExpr();
            expr->left = literal;
            expr->right = new IntLiteral();
            return expr;
        } else {
            return nullptr;
        }
    }
    void print_tree(BinExpr *current_node){
        if(current_node != nullptr){
            if(holds_alternative<IntLiteral*>(current_node->left)){
                IntLiteral *int_node_left = get<IntLiteral*>(current_node->left);
                cout << int_node_left->value;
            } else {
                BinExpr *bin_node = get<BinExpr*>(current_node->left);
                print_tree(bin_node);
            }

            if(current_node->op == TokenType::TYPE_PLUS){
                cout << " + ";
            } else if(current_node->op == TokenType::TYPE_MINUS){
                cout << " - ";
            }

            if(holds_alternative<IntLiteral*>(current_node->right)){
                IntLiteral *int_node_right = get<IntLiteral*>(current_node->right);
                cout << int_node_right->value;
            } else {
                BinExpr *bin_node = get<BinExpr*>(current_node->right);
                print_tree(bin_node);
            }
        }
    }
    bool is_operator(Token token){
        if(token.type == TokenType::TYPE_PLUS || token.type == TokenType::TYPE_MINUS){
            return true;
        }
        return false;
    }
    Token get_next_token(){
        if(lexer.tokens.empty()){
            return { TokenType::TYPE_INT, "" };
        }
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
