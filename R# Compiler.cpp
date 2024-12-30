#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <cmath>
#include <variant>
#include <functional>
#include <memory>
#include <stdexcept>
#include <algorithm>

using namespace std;

vector<string> allLines;

struct Variable {
    string type;
    variant<int, float, string, char, bool, vector<variant<int, float, string, char, bool>>> value;
};

unordered_map<string, Variable> symbol_table;

template <typename T>
int getIndexOfListItem(const string& var_name, const T& item) {
    if (symbol_table.find(var_name) != symbol_table.end()) {
        Variable& var = symbol_table[var_name];

        if (var.type == "list") {
            // Acessando a lista armazenada na variável
            auto& list = get<vector<variant<int, float, string, char, bool>>>(var.value);

            for (int i = 0; i < list.size(); i++) {
                bool found = std::visit([&item](auto&& arg) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, T>) {
                        return arg == item;
                    }
                    else {
                        return false;
                    }
                    }, list[i]);

                if (found) {
                    return i;
                }
            }
            cerr << "Erro: Item não existe na lista '" << var_name << "'" << endl;
        }
        else {
            cerr << "Erro: Variável '" << var_name << "' não é uma lista." << endl;
        }
    }
    else {
        cerr << "Erro: Variável '" << var_name << "' não definida." << endl;
    }
    return -1; // Retorna -1 se o item não for encontrado
}

void add_to_list(const string& var_name, const variant<int, float, string, char, bool>& value) {
    if (symbol_table.find(var_name) != symbol_table.end()) {
        Variable& var = symbol_table[var_name];
        if (var.type == "list") {
            vector<variant<int, float, string, char, bool>>& list = get<vector<variant<int, float, string, char, bool>>>(var.value);
            list.push_back(value);
        }
        else {
            cerr << "Erro: Variável '" << var_name << "' não é uma lista." << endl;
        }
    }
    else {
        cerr << "Erro: Variável '" << var_name << "' não definida." << endl;
    }
}

// Função para excluir um elemento de uma lista por índice
void delete_from_list(const string& var_name, int index) {
    if (symbol_table.find(var_name) != symbol_table.end()) {
        Variable& var = symbol_table[var_name];
        if (var.type == "list") {
            vector<variant<int, float, string, char, bool>>& list = get<vector<variant<int, float, string, char, bool>>>(var.value);
            if (index >= 0 && index < list.size()) {
                list.erase(list.begin() + index);
            }
            else {
                cerr << "Erro: Índice fora do intervalo." << endl;
            }
        }
        else {
            cerr << "Erro: Variável '" << var_name << "' não é uma lista." << endl;
        }
    }
    else {
        cerr << "Erro: Variável '" << var_name << "' não definida." << endl;
    }
}

int getSizeOfList(const string& var_name) {
    if (symbol_table.find(var_name) != symbol_table.end()) {
        Variable& var = symbol_table[var_name];
        if (var.type == "list") {
            vector<variant<int, float, string, char, bool>>& list = get<vector<variant<int, float, string, char, bool>>>(var.value);
            return list.size(); // Retorna o tamanho da lista
        }
        else {
            cerr << "Erro: Variável '" << var_name << "' não é uma lista." << endl;
        }
    }
    else {
        cerr << "Erro: Variável '" << var_name << "' não definida." << endl;
    }
    return -1; // Retorna -1 em caso de erro
}

// Função para aplicar operações aritméticas a variáveis de tipo int
void apply_arithmetic_op(const string& var_name, const string& op, int value) {
    if (symbol_table.find(var_name) != symbol_table.end()) {
        Variable& var = symbol_table[var_name];
        if (var.type == "int") {
            int& var_value = get<int>(var.value);
            if (op == "+=") var_value += value;
            else if (op == "-=") var_value -= value;
            else if (op == "*=") var_value *= value;
            else if (op == "**=") var_value = pow(var_value, value);
            else if (op == "^=") var_value = pow(var_value, 1.0 / value);
            else if (op == "%=") var_value = var_value % value;
            else if (op == "/=") var_value /= value;
        }
        else {
            cerr << "Erro: Tipo de variável não compatível para operação '" << op << "'." << endl;
        }
    }
    else {
        cerr << "Erro: Variável '" << var_name << "' não definida." << endl;
    }
}

// Função para aplicar operações aritméticas a variáveis de tipo float
void apply_arithmetic_op_float(const string& var_name, const string& op, float value) {
    if (symbol_table.find(var_name) != symbol_table.end()) {
        Variable& var = symbol_table[var_name];
        if (var.type == "float") {
            float& var_value = get<float>(var.value);
            if (op == "+=") var_value += value;
            else if (op == "-=") var_value -= value;
            else if (op == "*=") var_value *= value;
            else if (op == "**=") var_value = pow(var_value, value);
            else if (op == "^=") var_value = pow(var_value, 1.0 / value);
            else if (op == "/=") var_value /= value;
        }
        else {
            cerr << "Erro: Tipo de variável não compatível para operação '" << op << "'." << endl;
        }
    }
    else {
        cerr << "Erro: Variável '" << var_name << "' não definida." << endl;
    }
}

std::string to_string_dynamic(const std::variant<int, float, std::string, char, bool, std::vector<std::variant<int, float, std::string, char, bool>>>& value) {
    if (std::holds_alternative<std::vector<std::variant<int, float, std::string, char, bool>>>(value)) { const auto& vec = std::get<std::vector<std::variant<int, float, std::string, char, bool>>>(value); std::stringstream ss; ss << "["; for (size_t i = 0; i < vec.size(); ++i) { if (i > 0) ss << ", "; std::visit([&ss](auto&& arg) { if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, int>) { ss << arg; } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, float>) { ss << arg; } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) { ss << "\"" << arg << "\""; } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, char>) { ss << "'" << arg << "'"; } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, bool>) { ss << (arg ? "true" : "false"); } }, vec[i]); } ss << "]"; return ss.str(); } return std::visit([](auto&& arg) -> std::string { if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, int>) { return std::to_string(arg); } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, float>) { return std::to_string(arg); } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) { return arg; } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, char>) { return std::string(1, arg); } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, bool>) { return arg ? "true" : "false"; } else { return std::string(""); } }, value);
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r"); if (start == std::string::npos) { return ""; } size_t end = str.find_last_not_of(" \t\n\r"); return str.substr(start, end - start + 1);
}

vector<string> split_arguments(const string& args) {
    vector<string> tokens; string token; bool inside_string = false; char string_delimiter = '\0'; for (char ch : args) { if (inside_string) { if (ch == string_delimiter) { inside_string = false; } token += ch; } else { if (ch == '"' || ch == '\'') { inside_string = true; string_delimiter = ch; token += ch; } else if (ch == ',') { tokens.push_back(trim(token)); token.clear(); } else { token += ch; } } } if (!token.empty()) { tokens.push_back(trim(token)); } return tokens;
}

bool compare_variants(const variant<int, float, string, char, bool, vector<variant<int, float, string, char, bool>>>& var1, const variant<int, float, string, char, bool, vector<variant<int, float, string, char, bool>>>& var2, const string& op) { return visit([&](auto&& arg1) -> bool { return visit([&](auto&& arg2) -> bool { using T1 = decay_t<decltype(arg1)>; using T2 = decay_t<decltype(arg2)>; if constexpr (is_same_v<T1, T2>) { if (op == "!=") { return arg1 != arg2; } else if (op == "==") { return arg1 == arg2; } else if (op == "&") { if constexpr (is_same_v<T1, bool>) { return arg1 && arg2; } else { throw invalid_argument("Operador '&' só é válido para booleanos."); } } else if (op == "@") { if constexpr (is_same_v<T1, bool>) { return arg1 || arg2; } else { throw invalid_argument("Operador '@' só é válido para booleanos."); } } return false; } else { throw invalid_argument("Comparação entre tipos diferentes não suportada."); } }, var2); }, var1); }

bool evaluate_condition(const string& condition) { 
    smatch match; 
    regex condition_regex(R"((\w+)\s*([!=<>@&]+)\s*(\w+))");
    if (regex_search(condition, match, condition_regex)) {
        string var1 = match[1];
        string op = match[2];
        string var2 = match[3];
        auto get_value = [](const string& var) -> variant<int, float, string, char, bool, vector<variant<int, float, string, char, bool>>> {
            if (symbol_table.find(var) != symbol_table.end()) {
                return symbol_table[var].value;
            }
            if (var == "true") return true;
            if (var == "false") return false;
            if (isdigit(var[0]) || var[0] == '-') {
                if (var.find('.') != string::npos) return stof(var); return stoi(var);
            } return var;
        };
        auto val1 = get_value(var1);
        auto val2 = get_value(var2);
        return compare_variants(val1, val2, op);
    }
    return false;
}

vector<string> extract_block_lines(const vector<string>& lines, size_t& index) {
    vector<string> block_lines; 
    int open_braces = 1; 
    ++index; 
    while (index < lines.size() && open_braces > 0) { 
        const string& line = lines[index]; 
        if (line.find('{') != string::npos) { 
            ++open_braces; 
        } 
        if (line.find('}') != string::npos) { 
            --open_braces; 
        } 
        if (open_braces > 0) { 
            block_lines.push_back(line); 
        } 
        ++index; 
    } 
    return block_lines;
}

void execute(const string& line);

void execute_block_within_brackets(const vector<string>& block_lines) {
    for (const auto& line : block_lines) {
        execute(line, true);
    }
}

std::string trim_left(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start);
}

// Função para executar uma linha de código
void execute(const string& linef, bool execute_block) {
    string line;
    static size_t index = 0;
    try {
        regex comment_regex("//.*");
        regex int_regex("int\\s+(\\w+)\\s*=\\s*(\\d+);");
        regex float_regex("float\\s+(\\w+)\\s*=\\s*(\\d+\\.\\d+);");
        regex string_regex(R"(string\s+(\w+)\s*=\s*\"(.*?)\";)");
        regex char_regex(R"(char\s+(\w+)\s*=\s*'(.)';)");
        regex bool_regex(R"(bool\s+(\w+)\s*=\s*(true|false);)");
        regex print_regex(R"(print\s*\((.+)\)\s*;)");
        regex arithmetic_op_regex(R"((\w+)\s*([+\-*/%^&|^]{1,2}=)\s*(\d+);)");
        regex list_declaration_regex(R"(list\s+(\w+)\s*=\s*\[\];)");
        regex list_add_regex(R"((\w+)\.add\((.*)\);)");
        regex list_delete_regex(R"((\w+)\.delete\((\d+)\);)");
        regex list_size_regex(R"(\s*(\w+)\s*=\s*getSizeOfList\((\w+)\);)");
        regex list_indexof_regex(R"((\w+)\s*=\s*getIndexOfListItem\((\w+),\s*(\d+)\);)");
        regex assignment_with_index_regex(R"(\s*(\w+)\s*=\s*(\w+)\s*\[(\d+)\];)");
        regex nothing_regex(R"(\s*)");
        regex if_regex(R"(if\s*\(([^)]+)\)\s*\{)");
        regex elseif_regex(R"(else\s*if\s*\(([^)]+)\)\s*\{)");
        regex else_regex(R"(else\s*\{)");
        regex closing_brackets_regex(R"(\s*\}\s*)");

        smatch match;

        if (index >= allLines.size()) { return; }

        line = trim_left(linef);
        ++index;

        if (regex_match(line, match, comment_regex) || regex_match(line, match, closing_brackets_regex)) {
            return;
        }
        if (execute_block == true) {
            // Comando de declaração de variável int
            if (regex_match(line, match, int_regex)) {
                string var_name = match[1];
                int value = stoi(match[2]);
                symbol_table[var_name] = { "int", value };
            }
            // Comando de declaração de variável float
            else if (regex_match(line, match, float_regex)) {
                string var_name = match[1];
                float value = stof(match[2]);
                symbol_table[var_name] = { "float", value };
            }
            // Comando de declaração de string
            else if (regex_match(line, match, string_regex)) {
                string var_name = match[1];
                string value = match[2];
                symbol_table[var_name] = { "string", value };
            }
            // Comando de declaração de char
            else if (regex_match(line, match, char_regex)) {
                string var_name = match[1];
                char value = match[2].str()[0];
                symbol_table[var_name] = { "char", value };
            }
            // Comando de declaração de bool
            else if (regex_match(line, match, bool_regex)) {
                string var_name = match[1];
                bool value = (match[2] == "true");
                symbol_table[var_name] = { "bool", value };
            }
            //IF STATEMENTS
            else if (regex_match(line, match, if_regex)) {
                string condition = match[1];
                cout << "Condicao IF encontrada: " << condition << endl;
                if (evaluate_condition(condition) == true) {
                    cout << "Condicao e verdadeira";
                    vector<string> block_lines = extract_block_lines(allLines, index);
                    for (const auto& block_line : block_lines) {
                        execute(block_line, true);
                    }
                }
                else {
                    while (!regex_match(allLines[++index], closing_brackets_regex)) {}
                }
            }
            else if (regex_match(line, match, elseif_regex)) {
                string condition = match[1];
                cout << "Condicao ELSEIF encontrada: " << condition << endl;
                if (evaluate_condition(condition) == true) {
                    cout << "Condicao e verdadeira";
                    vector<string> block_lines = extract_block_lines(allLines, index);
                    for (const auto& block_line : block_lines) {
                        execute(block_line, true);
                    }
                }
                else {
                    while (!regex_match(allLines[++index], closing_brackets_regex)) {}
                }
            }
            else if (regex_match(line, match, else_regex)) {
                vector<string> block_lines = extract_block_lines(allLines, index);
                for (const auto& block_line : block_lines) {
                    execute(block_line, true);
                }
            }
            // Comando print
            else if (regex_match(line, match, print_regex)) {
                string args = match[1];
                vector<string> tokens = split_arguments(args);
                for (size_t i = 0; i < tokens.size(); i++) {
                    string var_name = tokens[i];
                    if (symbol_table.find(var_name) != symbol_table.end()) {
                        Variable var = symbol_table[var_name];
                        cout << to_string_dynamic(var.value);
                    }
                    else if (var_name.front() == '"' && var_name.back() == '"') {
                        cout << var_name.substr(1, var_name.size() - 2);
                    }
                    else {
                        cerr << "Erro: Variável ou valor '" << var_name << "' não definido." << endl; break;
                    }
                } cout << endl;
            }

            // Comando de operação aritmética
            else if (regex_match(line, match, arithmetic_op_regex)) {
                string var_name = match[1];
                string op = match[2];
                int value = stoi(match[3]);
                apply_arithmetic_op(var_name, op, value);
            }
            // Comando de operação aritmética para float
            else if (regex_match(line, match, arithmetic_op_regex)) {
                string var_name = match[1];
                string op = match[2];
                float value = stof(match[3]);
                apply_arithmetic_op_float(var_name, op, value);
            }
            // Comando de declaração de lista
            else if (regex_match(line, match, list_declaration_regex)) {
                string list_name = match[1];
                symbol_table[list_name] = { "list", vector<variant<int, float, string, char, bool>>() };
            }
            // Comando de adicionar item à lista
            else if (regex_match(line, match, list_add_regex)) {
                string list_name = match[1];
                string value_str = match[2];
                variant<int, float, string, char, bool> value;

                if (value_str.find_first_not_of("0123456789") == string::npos) {
                    value = stoi(value_str);
                }
                else if (value_str.find('.') != string::npos) {
                    value = stof(value_str);
                }
                else if (value_str == "true" || value_str == "false") {
                    value = (value_str == "true");
                }
                else {
                    value = value_str;
                }
                add_to_list(list_name, value);
            }
            // Comando de deletar item da lista
            else if (regex_match(line, match, list_delete_regex)) {
                string list_name = match[1];
                int index = stoi(match[2]);
                delete_from_list(list_name, index);
            }
            else if (regex_match(line, match, assignment_with_index_regex)) {
                string var_name = match[1];   // Nome da variável
                string list_name = match[2];   // Nome da lista
                int index = stoi(match[3]);    // Índice para acessar na lista

                // Verifica se a lista existe
                if (symbol_table.find(list_name) != symbol_table.end()) {
                    Variable& list_var = symbol_table[list_name];
                    if (list_var.type == "list") {
                        vector<variant<int, float, string, char, bool>>& list = get<vector<variant<int, float, string, char, bool>>>(list_var.value);

                        // Verifica se o índice está dentro do intervalo da lista
                        if (index >= 0 && index < list.size()) {
                            // Obtém o valor do item da lista
                            variant<int, float, string, char, bool> list_item = list[index];
                            Variable new_var;

                            // Atribui o valor do item da lista à variável
                            if (holds_alternative<int>(list_item)) {
                                new_var.type = "int";
                                new_var.value = get<int>(list_item);
                            }
                            else if (holds_alternative<float>(list_item)) {
                                new_var.type = "float";
                                new_var.value = get<float>(list_item);
                            }
                            else if (holds_alternative<string>(list_item)) {
                                new_var.type = "string";
                                new_var.value = get<string>(list_item);
                            }
                            else if (holds_alternative<char>(list_item)) {
                                new_var.type = "char";
                                new_var.value = get<char>(list_item);
                            }
                            else if (holds_alternative<bool>(list_item)) {
                                new_var.type = "bool";
                                new_var.value = get<bool>(list_item);
                            }

                            // Armazena a variável no symbol_table
                            symbol_table[var_name] = new_var;

                            //cout << "A variável " << var_name << " foi atribuída com o valor do item da lista " << list_name << " no índice " << index << "." << endl;
                        }
                        else {
                            cerr << "Erro: Índice fora do intervalo da lista '" << list_name << "'." << endl;
                        }
                    }
                    else {
                        cerr << "Erro: A variável '" << list_name << "' não é uma lista." << endl;
                    }
                }
                else {
                    cerr << "Erro: A lista '" << list_name << "' não foi definida." << endl;
                }
            }
            // Comando para obter o tamanho de uma lista
            else if (regex_match(line, match, list_size_regex)) {
                string var_name = match[1];
                string list_name = match[2];
                symbol_table[var_name] = { "int", getSizeOfList(list_name) };
            }
            // Comando para obter índice de item da lista
            else if (regex_match(line, match, list_indexof_regex)) {
                string var_name = match[1];
                string list_name = match[2];
                int item = stoi(match[3]);
                symbol_table[var_name] = { "int", getIndexOfListItem(list_name, item) };
            }
            else if (regex_match(line, match, nothing_regex)) {
                return;
            }
            else {
                cout << "Erro na linha: " << line << " -- " << "No known command on line" << endl;
            }
        }
    }
    catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
    }
}

int main() {
    ifstream input_file("codigo.rsharp");
    string line;

    while (getline(input_file, line)) {
        allLines.insert(allLines.end(), line);
        execute(line, true);
    }

    return 0;
}