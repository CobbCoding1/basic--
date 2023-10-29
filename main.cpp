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

void parse_lexer(Lexer lexer){
    lexer.print_tokens();
}

int main(){
    Lexer lexer = lex_file("test.basic");
    parse_lexer(lexer);
    return 0;
}
