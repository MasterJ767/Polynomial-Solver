#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cmath>

struct term {
    int coefficient;
    int exponent;
};

class Equation {
private:
    std::vector<term*> expression;
    std::vector<int> sequence;
    int start_index;
    int end_index;
    char variable_char;
    bool expression_found;

    term* evaluate_expression_token(std::string token) {
        term* temp = new term;
        temp->coefficient = token[2] == variable_char ? stoi(token.substr(0, 2)) : stoi(token);
        temp->exponent = token.length() > 3 && token[3] == '^' ? stoi(token.substr(4, 1)) : token[2] == variable_char ? 1 : 0;
        return temp;
    }

    std::string build_output_string() {
        std::string output_string = std::to_string(sequence.at(0));
        for (int i = 1; i <= end_index - start_index; ++i) {
            output_string += ", ";
            output_string += std::to_string(sequence.at(i));
        }
        output_string += "\n";
        return output_string;
    }

    int factorial(int value) {
        int result = 1;
        for (int i = 1; i <= value; ++i) {
            result *= i;
        }
        return result;
    }

    void determine_highest_power() {
        std::vector<int> difference = sequence;
        std::set<int> unique_differences(difference.begin(), difference.end());
        int n = 0;
        while (unique_differences.size() > 1) {
            unique_differences.clear();
            for (int i = 0; i < difference.size() - 1; ++i) {
                difference.at(i) = difference.at(i + 1) - difference.at(i);
            }
            difference.pop_back();
            std::copy(difference.begin(), difference.end(), std::inserter(unique_differences, unique_differences.end()));
            ++n;
        }
        term* temp = new term;
        temp->coefficient = n <= 1 ? difference.at(0) : difference.at(0) / factorial(n);
        temp->exponent = n;
        expression.push_back(temp);
    }

    int radd(int addend, int repeats) {
        return repeats > 0 ? addend + radd(addend, repeats - 1) : addend;
    }

    int rdiv(int dividend, int divisor, int repeats) {
        return repeats > 0 ? rdiv(dividend / divisor, divisor, repeats - 1) : dividend / divisor;
    }

    int inrt(int radicand, int index) {
        if (radicand <= 0) return 0;
        int x0 = 1 << (((int)std::log2(radicand)) / index + 1);
        int x1 = (radd(x0, index - 2) + rdiv(radicand, x0, index - 2)) / index;
        while (x1 < x0)
        {
            x0 = x1;
            x1 = (radd(x0, index - 2) + rdiv(radicand, x0, index - 2)) / index;
        }
        return x0;
    }

    void guess_start_index() {
        int guess = inrt(sequence.at(0) / expression.at(0)->coefficient, expression.at(0)->exponent);
        start_index += guess;
        end_index += guess;
    }

    int check_gaussian_result() {
        for (auto i : expression) {
            if (i->exponent > 0 && std::abs(i->coefficient) > 10) return 1;
            if (i->exponent == 0 && std::abs(i->coefficient) > 1000) return 1;
        }
        return 0;
    }

    int gaussian_elimination() {
        int unknowns = expression.at(0)->exponent;
        std::vector<float> matrix(unknowns * (unknowns + 1), 0);
        int offset = start_index == 0 ? 1 : 0;
        for (int i = 0; i < unknowns; ++i) {
            for (int j = 0; j < unknowns; ++j) {
                matrix.at(i * (unknowns + 1) + j) = std::pow(start_index + i + offset, unknowns - j - 1);
            }
            matrix.at(i * (unknowns + 1) + unknowns) = sequence.at(i + offset) - (expression.at(0)->coefficient * std::pow(start_index + i + offset, unknowns));
        }
        for (int i = 0; i < unknowns - 1; ++i) {
            for (int j = unknowns - 1; j > i; --j) {
                float ratio = matrix.at(j * (unknowns + 1) + i) / matrix.at(i * (unknowns + 1) + i);
                for (int k = i; k <= unknowns; ++k) {
                    matrix.at(j * (unknowns + 1) + k) -= (ratio * matrix.at(i * (unknowns + 1) + k));
                }
            }
        }
        for (int i = unknowns - 1; i >= 0; --i) {
            float coefficient = 0;
            for (int j = unknowns - 1; j > i; --j) {
                if (i == unknowns - 1 && j == unknowns - 1 && matrix.at(i * (unknowns + 1) + j) == 0) return 1;
                coefficient += (matrix.at(i * (unknowns + 1) + j) * expression.rbegin()[std::abs(j - (unknowns - 1))]->coefficient);
            }
            term* temp = new term;
            temp->coefficient = (matrix.at(i * (unknowns + 1) + unknowns) - coefficient) / matrix.at(i * (unknowns + 1) + i);
            temp->exponent = std::abs(i - (unknowns - 1));
            expression.insert(expression.begin() + 1, temp);
        }
        return check_gaussian_result() ? 1 : 0;
    }

    std::string print_expression_success() {
        std::string str_expression = "";
        for (auto i : expression) {
            if (i->coefficient == 0) continue;
            str_expression == "" ? str_expression.append(i->coefficient < 0 ? "-" : "") : str_expression.append(i->coefficient < 0 ? " - " : " + ");
            str_expression.append(std::abs(i->coefficient) > 1 ? std::to_string(std::abs(i->coefficient)) : (i->exponent == 0 ? std::to_string(1) : ""));
            str_expression.append(i->exponent == 0 ? "" : std::string(1, variable_char));
            if (i->exponent <= 1) continue;
            str_expression.append("^");
            str_expression.append(std::to_string(i->exponent));
        }
        str_expression.append("\n");
        return str_expression;
    }

public:
    Equation(char var_char = 'x') {
        variable_char = var_char;
        start_index = 0;
        end_index = 0;
        expression_found = false;
    }

    ~Equation() {
        for (auto i : expression) {
            delete i;
        }
        expression.clear();
        sequence.clear();
    }

    void derive_expression_from_string(std::string str_expression) {
        size_t position = 0;
        std::string delimiter = " ";
        std::string token = str_expression[0] == '-' ? "" : "+";
        while ((position = str_expression.find(delimiter)) != std::string::npos) {
            token == "+" || token == "-" ? token += str_expression.substr(0, position) : token = str_expression.substr(0, position);
            str_expression.erase(0, position + delimiter.length());
            if (token == "+" || token == "-") continue;
            if (token[1] == variable_char) token.insert(1, "1");
            expression.push_back(evaluate_expression_token(token));
        }
        token += str_expression;
        if (token[1] == variable_char) token.insert(1, "1");
        expression.push_back(evaluate_expression_token(token));
        expression_found = true;
    }

    void derive_output_set(int start, int end) {
        start_index = start;
        end_index = end;
        sequence.resize(end - start + 1, 0);
        for (auto i : expression) {
            for (int j = 0; j <= end - start; ++j) {
                sequence.at(j) += (int)std::pow(start + j, i->exponent) * i->coefficient;
            }
        }
    }

    void print_output_set() {
        for (int i = 0; i < sequence.size(); ++i) {
            std::cout << start_index + i << ": " << sequence[i] << "\n";
        }
    }

    void write_output_set(std::string filename) {
        try {
            std::ofstream out_file(filename);
            if (out_file.bad()) throw (filename);
            out_file << build_output_string();
            out_file.close();
        }
        catch (std::string error_name)
        {
            std::cout << "Something went wrong finding the file: " << error_name;
        }
    }

    void build_sequence(std::string str_sequence) {
        size_t position = 0;
        std::string delimiter = ",";
        std::string token;
        while ((position = str_sequence.find(delimiter)) != std::string::npos) {
            token = str_sequence.substr(0, position);
            str_sequence.erase(0, position + delimiter.length());
            if (token != "") sequence.push_back(std::stoi(token));
        }
        token = str_sequence;
        if (token != "") sequence.push_back(std::stoi(token));
        end_index = start_index + sequence.size() - 1;
    }

    void read_input_set(std::string filename) {
        std::string input;
        try {
            std::ifstream in_file(filename);
            if (in_file.bad()) throw (filename);
            std::getline(in_file, input);
            build_sequence(input);
            in_file.close();
        }
        catch (std::string error_name)
        {
            std::cout << "Something went wrong finding the file: " << error_name;
        }
    }

    void derive_expression_from_sequence() {
        determine_highest_power();
        if (expression.at(0)->exponent == 0) {
            expression_found = true;
            return;
        };
        int n = 0;
        while (gaussian_elimination() != 0) {
            n++;
            if (n == 1) {
                guess_start_index();
            }
            else {
                start_index -= 1;
                end_index -= 1;
            }
            for (int i = 1; i < expression.size(); ++i) {
                delete expression.at(i);
            }
            expression.resize(1);
            if (n == 10) {
                start_index += 18;
                end_index += 18;
            }
            if (n == 19) break;
        }
        expression_found = n < 10;
    }

    std::string get_expression() {
        return expression_found ? print_expression_success() : "This program could not determine the equation for this sequence.\n";
    }

    void print_expression() {
        std::cout << "f(x) = " << get_expression();
    }

    void write_expression(std::string filename) {
        try {
            std::ofstream out_file(filename);
            if (out_file.bad()) throw (filename);
            out_file << get_expression();
            out_file.close();
        }
        catch (std::string error_name)
        {
            std::cout << "Something went wrong finding the file: " << error_name;
        }
    }
};

inline std::string trim(std::string& str) {
    std::string whitespaces(" \t\f\v\n\r\'\"");
    std::size_t start = str.find_first_not_of(whitespaces);
    std::size_t end = str.find_last_not_of(whitespaces);
    str.erase((end + 1) < std::string::npos ? end + 1 : std::string::npos, std::string::npos);
    str.erase(0, start);
    return str;
}

std::string read_batch_file(std::string filename) {
    std::string input;
    std::string file_contents;
    try {
        std::ifstream in_file(filename);
        if (in_file.bad()) throw (filename);
        while (std::getline(in_file, input)) {
            file_contents += input;
            file_contents.push_back('\n');
        }
    }
    catch (std::string error_input) {
        std::cout << "An error occured while reading the input. User input: " << error_input;
    }
    return trim(file_contents);
}

std::vector<Equation*> create_equations(std::string sequence_batch) {
    std::vector<Equation*> equations;
    size_t position = 0;
    std::string delimiter = "\n";
    std::string token;
    while ((position = sequence_batch.find(delimiter)) != std::string::npos) {
        token = sequence_batch.substr(0, position);
        sequence_batch.erase(0, position + delimiter.length());
        Equation* eq = new Equation('x');
        eq->build_sequence(token);
        equations.push_back(eq);
    }
    token = sequence_batch;
    if (token != "") {
        Equation* eq = new Equation('x');
        eq->build_sequence(token);
        equations.push_back(eq);
    }
    return equations;
}

void write_batch_file(std::string filename, std::string file_contents) {
    try {
        std::ofstream out_file(filename);
        if (out_file.bad()) throw (filename);
        out_file << file_contents;
        out_file.close();
    }
    catch (std::string error_name)
    {
        std::cout << "Something went wrong finding the file: " << error_name;
    }
}

void derive_sequence_from_function(char* argv[]) {
    Equation* eq = new Equation('x');
    std::string str_expression = argv[2];
    int start = std::stoi(argv[3]);
    int end = std::stoi(argv[4]);
    std::string out_filename = argv[5];
    eq->derive_expression_from_string(trim(str_expression));
    eq->derive_output_set(start, end);
    eq->print_output_set();
    eq->write_output_set(out_filename);
    delete eq;
}

void derive_function_from_sequence(char* argv[]) {
    Equation* eq = new Equation('x');
    std::string in_filename = argv[2];
    eq->read_input_set(in_filename);
    eq->derive_expression_from_sequence();
    eq->print_expression();
    delete eq;
}

void batch_derive_functions(char* argv[]) {
    std::string in_filename = argv[2];
    std::string out_filename = argv[3];
    std::string sequences = read_batch_file(in_filename);
    std::vector<Equation*> eqs = create_equations(sequences);
    std::string output;
    for (auto i : eqs) {
        i->derive_expression_from_sequence();
        output += i->get_expression();
        delete i;
    }
    write_batch_file(out_filename, output);
    eqs.clear();
}

int main(int argc, char* argv[])
{
    switch(std::stoi(argv[1])) {
    case 0:
        derive_sequence_from_function(argv);
        break;
    case 1:
        derive_function_from_sequence(argv);
        break;
    case 2:
        batch_derive_functions(argv);
        break;
    }
}
