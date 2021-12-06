/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Jazz Faye Olario
 */

#include "std_lib_facilities.h"


class Token { //Holds the operators, operands and variables
public:

	char kind;  
	double value; 
	string name;

	Token(char ch) :kind{ch} { }
	Token(char ch, double val) :kind{ch}, value{val} { }
	Token(char ch, string n) : kind{ch}, name{n} { }
};

class Token_stream { //Stream of tokens
public:
    Token_stream() :full{false}, buffer{0} { }
	Token get();
	void putback(Token t) { buffer=t; full=true; }
	void ignore(char c);
private:
	bool full;
	Token buffer;
};

const char con = 'C';
const char let = 'L';  
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char square_root = 's';
const char power = 'p';
//Built in functions
const string quit_key = "exit";
const string constant_key = "const";
const string Declare = "let";
const string sqrt_key = "sqrt";
const string pow_key = "pow";

Token Token_stream::get() //Evaluate each char in the stream and determine what it is 
{
	if (full) { full=false; return buffer; } //If not empty, return the value from buffer
	char ch; 
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
	case ',':   
		return Token{ch};  
		//Numerical literals
	case '.':
	case '0': 
	case '1': 
	case '2': 
	case '3': 
	case '4':
	case '5': 
	case '6': 
	case '7': 
	case '8': 
	case '9':   
	{	cin.putback(ch);
		double val;
		cin >> val;
		return Token{number, val};
	} 
	default:
	    //For Strings. Keywords for declaration, stopping the program and variable names.
		if (isalpha(ch)) { 
			string s;
			s += ch;
			//Ex.01 = Allowing underscores in the calculator's variable
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
			cin.putback(ch);
			if (s == constant_key) return Token(con);
			if (s == Declare) return Token{let};
			if (s == quit_key) return Token{quit};
			if (s == sqrt_key) return Token{square_root};
			if (s == pow_key) return Token{power};
			return Token{name,s};
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
    //First look in the buffer
	if (full && c == buffer.kind) { 
		full = false;
		return;
	}
	//Dismiss buffered token
	full = false;

    
	char ch;
    while (cin >> ch)
        if(ch == c)
    return;
}

class Variable { //Holds the variable names and its values
public:
	//Declaration of Variables
	string name; 
	double value; 
	bool Constant;
	Variable(string n, double v, bool va = true) :name(n), value(v), Constant(va) { }
};

vector<Variable> names;	//Stores the variable names

double get_value(string s) //Get variables value
{
	for (int i = 0; i < names.size(); ++i)
	    if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
}
//Ex.03 check for constants and variable declaration
void set_value(string s, double d) //Set the variable value
{
    for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			if(names[i].Constant) error(s, " is a constant.");
				names[i].value = d;
				return;
		}
				
	error("set: undefined name ", s);
}

bool is_declared(string s) //Check if variable has already been declared
{
	for (int i = 0; i < names.size(); ++i)
	    if (names[i].name == s) return true;
	return false;
}

Token_stream ts;

double expression(); //Declaring the expression

double getpow(double base, int power)
{
    if (power == 0) {
        if (base == 0) return 0;    // special case such as pow(0,0)
        return 1;                   // something to power of 0
    }
    double x = base;             
    for (int i = 2; i <= power; ++i)   // power of 2 and more
        x *= base;
    return x;
}

double primary() //Process the numbers, parentheses, sqrt etc.
{
	Token t = ts.get(); 
	switch (t.kind) {
	case '(': //For '(' ')'
	{	double d = expression(); 
		t = ts.get(); 
		if (t.kind != ')') error("')' expected");  //Shows an Error if there is no ')'
		return d;
	}
	case '-': //For Negative Numbers
		return - primary(); 
	case number: //For Numbers 
		return t.value;
	case name: //For variable names and also Ex.02
	{ 
		Token t2 = ts.get();
		if (t2.kind == '=') 
		{
			set_value(t.name,expression()); //If there is '=' then update the value of the variable 
		} 
		else
		{
			ts.putback(t2);
		} 
		return get_value(t.name); // Return the value of the variable
	}
	case square_root: //For Square Root
	{
		double d = primary();
		if (d < 0) error("Unable to get the Square Root of a negative number"); // Print error message if negative number was thrown to sqrt
		return sqrt(d);
	}
	case power: //For Power
	{
		 t = ts.get();
        if (t.kind != '(') error("'(' expected");
        double d = expression();
        t = ts.get();
        if (t.kind != ',') error("',' expected");
        t = ts.get();
		int i = int(t.value);
        if (t.kind != number) error("Please provide the second argument");
        if (i != t.value) error("Please make sure that the second argument is an integer.");
        t = ts.get();
        if (t.kind != ')') error("')' expected");
        return getpow(d,i);
		}

	default:
		error("primary expected"); //Default Error is something unusual was provided
	}
}

double term() //To perform mathematical operations(*, /, %)
{
	double left = primary();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '*': //Multiplication
			left *= primary();
			break;
		case '/': //Division
		{	double d = primary();
			if (d == 0) error("divide by zero"); //Print an error if divided by zero
			left /= d;
			break;
		}
        case '%': //Modulo
        {   double d = primary();
            if (d == 0) error("divide by zero"); //Print an error if divided by zero
            left = fmod(left, d);
            break;
        }
		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression() //Perform mathematical operations (+, -)
{
	double left = term();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '+': //Addition
			left += term();
			break;
		case '-': //Subtraction
			left -= term();
			break;
		default:
			ts.putback(t); 
			return left;
		}
	}
}

double declaration(bool constant) //Declare name of variable
{
	

	Token t2  = ts.get();
	if (t2.kind != name) error ("name expected in declaration"); //Prints an error if there is no name
	string name = t2.name;
	if (is_declared(name)) error(name, " declared twice"); //Prints an error if variable has already been declared

	Token t3 = ts.get();
	if (t3.kind != '=') error(" '=' missing in declaration of " ,name); //Prints an error is there is '=' symbol

	double d = expression(); 
	names.push_back(Variable{name,d, constant}); //Add new variable to the vector
	return d;
}

double statement() //Handles Declaration and Expression
{
	Token t = ts.get();
	switch(t.kind) {
	case let:
		return declaration(false);
	case con:
		return declaration(true);
	default:
		ts.putback(t);
		return expression();
	}
}

void clean_up_mess() //Ignores all characters before ';' if an exception was thrown
{
	ts.ignore(print);   
}

const string prompt = "> ";
const string result = "= ";

void calculate() //To perform calculations
{
	while(true)
	try {
		cout << prompt; //Prints the prompt symbol '>'
		Token t = ts.get();
		while (t.kind == print) t = ts.get(); 
		if (t.kind == quit) return; //Ends the program when user types 'exit'
		ts.putback(t);
		cout << result << statement() << '\n'; //Shows the calculation results
	}
	catch(exception& e) {
		cerr << e.what() << '\n';
		clean_up_mess();    
	}
}

int main()
try {
	cout << "Please enter an equation. Or type 'exit' to stop.\n\n";
    // Predefined variables
    names.push_back(Variable{"k", 1000, true});
	names.push_back(Variable{"pi", 3.14, true});
	names.push_back(Variable{"e", 2.71828, true});
    
    calculate(); //To perform calculation
    return 0;
}
catch (exception& e) {
    cerr << "exception: " << e.what() << '\n';
    return 1;
}
catch (...) {
    cerr << "Uknown exception!\n";
	return 2;
}
