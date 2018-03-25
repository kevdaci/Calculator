/*
 * Name: Kevin Bayas
 * Date: 3-20-17
 * Assignment: Project 1 Calculator
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <exception>


using namespace std;


/*
 * The InvalidInputException class is created in order to throw the
 * "Undeclared-Variable" error.
 */
class InvalidInputException: public exception{
    public:
        virtual const char* what() const throw(){
            return "Undeclared-Variable";
        }
};

/*
 * The DivideByZeroException class is created in order to throw the
 * "Division-By-Zero" error.
 */
class DivideByZeroException: public exception{
    public:
        virtual const char* what() const throw(){
            return "Division-By-Zero";
        }
};

class Calculator{
    private:
        double PI;
        double e;

        //This variable is used to rank the ^,*,/,+,- operators.
        map<string, int> ops_ranking;

        //This map is used to store variables and their expressions
        map<string, double> variables;   

        //This map is used to check if any variables were assigned from the user
        map<string, bool> variable_checker;       
     
        InvalidInputException invalid_input;
        DivideByZeroException division_by_zero;

        //This method is used to clear any whitespace from the user's input
        void clear_whitespace(string& s){
            while(s.find(' ') != string::npos){
                s.replace(s.find(' '), 1, "");
            }
        }

        /*
         * This method is used to evaluate simple mathematical expressions such
         * as the ^,*,/,+,- operators.
         */
        double evaluate(string op, double val1, double val2){
            if(op.compare("^") == 0)
                return pow(val2, val1);
            if(op.compare("*") == 0)
                return val1 * val2;
            if(op.compare("/") == 0){
                if(val1 == 0)
                    throw division_by_zero;
                return val2/val1;
            }
            if(op.compare("+") == 0)
                return val1 + val2;
            if(op.compare("-") == 0)
                return val2 - val1;
        }

        /*
         * This method is used to evaluate more complex mathematical expressions. In other 
         * word, this method evaluates sin, cos, and log functions.
         */
        double evaluate_function(string func, string sub_expression){
            if(func.compare("sin") == 0){
                return sin(calculate(sub_expression));
            }
            if(func.compare("cos") == 0){
                return cos(calculate(sub_expression));
            }
            if(func.compare("log") == 0){
                return log(calculate(sub_expression));
            }
        }

        //This method assign variables to expressions.
        void set_variable(string input){
            clear_whitespace(input);
            int c = 3;
            while(input.substr(c,1) != "="){
                c++;
            }
            string var = input.substr(3, c-3);
            string expression = input.substr(c+1);

            //Try to check if the variable being assigned from the user has any errors. 
            try{
                variables[var] = calculate(expression);
                variable_checker[var] = true;
            }
            catch(...){
                return;
            }
        }

        /*
         * This method will evalute the mathematical expression that the user
         * inputs.
         */
        double calculate(string expression){
            clear_whitespace(expression);
            vector<double> values;
            vector<string> ops;

            //Each character of the expression will be evaluated and will be used according 
            //to one of the test cases.
            for(int i = 0; i < expression.size(); i++){

                //Check if the character is a number.
                if(isdigit(expression[i])){
                    int j = i;
                    while(j < expression.size() && (isdigit(expression[j]) || expression[j] == '.')){
                        j++;
                    }
                    string num = expression.substr(i, j-i);
                    values.push_back(atof(num.c_str()));
                    i = j-1;
                }

                //If the character is not a number, check if it is a letter. If it is, then
                //check if the character turns out to be one of the math functions, one of the
                //constants, or a variable.
                else if(isalpha(expression[i])){
                    string word;
                    int j = i;
                    while(j < expression.size() && isalpha(expression[j])){
                        j++;
                    }
                    word = expression.substr(i, j-i);

                    //If the word read from the characters turns out to be the name of a mathematical
                    //function, then the expression in between the parenthesis will be evaluated.
                    if(word.compare("sin") == 0 || word.compare("cos") == 0 || word.compare("log") == 0){
                        int k = j+1;

                        //The parenthesis_check variable will help read the expression in between parenthesis
                        //properly.
                        vector<char> parenthesis_check;
                        parenthesis_check.push_back('(');
                        while(k < expression.size() && parenthesis_check.size() != 0){
                            if(expression[k] == '('){
                                parenthesis_check.push_back(expression[k]);
                            }
                            if(expression[k] == ')'){
                                parenthesis_check.pop_back();
                            }
                            k++;
                        }
                        string sub_expression = expression.substr(j+1, k-j-2);
                        double num = evaluate_function(word, sub_expression);
                        values.push_back(num);
                        j = k-1;
                        i = j;
                    }

                    //If the word read from the characters turns out to be one of the constants push
                    //the value of the constant to the values stack.
                    else if(word.compare("e") == 0){
                        values.push_back(e);
                        i = j - 1;
                    }
                    else if(word.compare("Pi") == 0){
                        values.push_back(PI);
                        i = j - 1;
                    }

                    //Else, if the word read from the characters turns out to be a possible variable,
                    //then check if it is a variable and push the value to the values stack.
                    else{
                        if(!variable_checker[word]){
                            throw invalid_input;
                        }
                        values.push_back(variables[word]);
                        i = j - 1;
                    }
                }

                //If the character is a left parenthesis, push it to the operator stack.
                else if(expression[i] == '('){
                    ops.push_back(expression.substr(i,1));
                }

                //If the character is a right parenthesis, then do the following:
                else if(expression[i] == ')'){
                    //Evaluate the expression and pop the operators from the stack until a left
                    //parenthesis is found.
                    while(ops.size() > 0 && ops[ops.size()-1].compare("(") != 0){
                        string op = ops[ops.size()-1];
                        ops.pop_back();
                        double value1= values[values.size()-1];
                        values.pop_back();
                        double value2 = values[values.size()-1];
                        values.pop_back();
                        values.push_back(evaluate(op, value1, value2));
                    }
                    if(ops.size() > 0){
                        ops.pop_back();
                    }
                }

                //If the character is a mathematical operator, then do the following,
                else if(expression[i] == '^' || expression[i] == '*' || expression[i] == '/' || expression[i] == '+' || expression[i] == '-'){
                    //Evaluate the expression until the operator in the stack has a lower precedence than 
                    //the operator that is being evaluated.
                     while(ops.size() != 0 && ops_ranking[ops[ops.size()-1]] >= ops_ranking[expression.substr(i,1)]){
                        string op = ops[ops.size()-1];
                        ops.pop_back();
                        double value1= values[values.size()-1];
                        values.pop_back();
                        double value2 = values[values.size()-1];
                        values.pop_back();
                        values.push_back(evaluate(op, value1, value2));
                    }
                    //Push the current character, which is an operator, to the stack.
                    ops.push_back(expression.substr(i,1));
                }
            }

            //Evaluate the remaining expression.
            while(ops.size() != 0){
                string op = ops[ops.size() - 1];
                ops.pop_back();
                double value1 = values[values.size()-1];
                values.pop_back();
                double value2 = values[values.size()-1];
                values.pop_back();
                values.push_back(evaluate(op, value1, value2));
            }

            return values[values.size() - 1];
        }

    public:
        Calculator(){
            PI = 3.14169;
            e = 2.718;
            ops_ranking["^"] = 3;
            ops_ranking["*"] = 2;
            ops_ranking["/"] = 2;
            ops_ranking["+"] = 1;
            ops_ranking["-"] = 1;
        }

        //This method will take the input from the user and will either assign a variable or
        //evaluate an expression.
        void take_input(string input){
            if((input.substr(0,4)).compare("let ") == 0){
                set_variable(input);
            }
            else{
                try{
                    cout << calculate(input) << endl;
                }
                catch(InvalidInputException& ie){
                    cout << ie.what() << endl;
                }
                catch(DivideByZeroException& de){
                    cout << de.what() << endl;
                }
            }
        }

};

int main(){
    string input;
    Calculator calc;

    while(getline(cin, input)){
        if(input.compare("quit") == 0){
            break;
        }
        calc.take_input(input);
    }

    return 0;
}
