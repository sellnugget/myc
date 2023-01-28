#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
namespace Parse {
	using std::string;
	using std::vector;
	enum TokenType 
	{
		//constants and identifier

		WHITESPACE, INTEGER_LITERAL, IDENTIFIER, STRING_LITERAL,

		//symbols

		//controll flow
		OPEN_BRACE, CLOSE_BRACE, OPEN_BRACKET, CLOSE_BRACKET, SEMICOLIN, OPEN_SQUARE, CLOSE_SQUARE,

		//preops 
		NEGATION, SUBTRACT = NEGATION, 
		BITWISE_COMPLEMENT, 
		LOGICAL_NEGATION, INCREMENT, DECREMENT,
		//binary operators

		ADD, MULTIPLY, DIVIDE, MOD, BITOR, BITAND, XOR, LSHIFT, RSHIFT,
		
		//comparisans
		AND, OR, EQUAL, NOT_EQUAL, LESS_THAN, LESS_OR_EQUAL, GREATER_THAN, GREATER_OR_EQUAL,
		
		//conditional operators

		COLON, QUESTION, COMMA,
		
		//Assignment operators
		ASSIGNMENT, ADD_ASSIGN, SUB_ASSIGN, DIVIDE_ASSIGN, MULTIPLY_ASSIGN, MOD_ASSIGN, 
		OR_ASSIGN, LSHIFT_ASSIGN, RSHIFT_ASSIGN, AND_ASSIGN, XOR_ASSIGN,

		//keywords

		INT, RETURN, IF, ELSE, FOR, WHILE, DO, BREAK, CONTINUE, CHAR
	};
	static std::map<TokenType, string> TokenToString =
	{
		{WHITESPACE, "WHITESPACE"},
		
		//consts and identifiers
		{INTEGER_LITERAL, "INTEGER_LITERAL"}, {IDENTIFIER, "IDENTIFIER"}, {STRING_LITERAL, "STRING_LITERAL"},

		//control flow
		{OPEN_BRACE, "OPEN_BRACE"}, {CLOSE_BRACE, "CLOSE_BRACE"}, {OPEN_BRACKET, "OPEN_BRACKET"}, {CLOSE_BRACKET, "CLOSE_BRACKET"}, {SEMICOLIN, "SEMICOLIN"}, {OPEN_SQUARE, "OPEN_SQAUARE"}, {CLOSE_SQUARE, "CLOSE_SQAURE"},
		
		//preops
		{NEGATION, "NEGATION"}, {BITWISE_COMPLEMENT, "BITWISE_COMPLEMENT"},
		{LOGICAL_NEGATION, "LOGICAL_NEGATION"}, {INCREMENT, "INCREMENT"}, {DECREMENT, "DECREMENT"},
		
		//binary operators
		{ADD, "ADD"}, {MULTIPLY, "MULTIPLY"}, {DIVIDE, "DIVIDE"}, {MOD, "MOD"}, {BITOR, "BITOR"},
		{BITAND, "BITAND"}, {XOR, "XOR"}, {LSHIFT, "LSHIFT"}, {RSHIFT, "RSHIFT"},

		
		//comparisons
		{AND, "AND"}, {OR, "OR"}, {EQUAL,"EQUAL"}, {NOT_EQUAL, "NOT_EQUAL"}, 
		{LESS_THAN, "LESS_THAN"},{LESS_OR_EQUAL, "LESS_OR_EQUAL"}, {GREATER_THAN, "GREATER_THAN"},
		{GREATER_OR_EQUAL, "GREATER_OR_EQUAL"},

		//conditional expressions

		{COLON, "COLIN"}, {QUESTION, "QUESTION"}, {COMMA, "COMMA"},

		//assignment
		{ASSIGNMENT, "ASSIGNMENT"}, {ADD_ASSIGN, "ADD_ASSIGN"}, {SUB_ASSIGN, "SUB_ASSIGN"},
		{DIVIDE_ASSIGN, "DIVIDE_ASSIGN"}, {MULTIPLY_ASSIGN, "MULTIPLY_ASSIGN"},{MOD_ASSIGN, "MOD_ASSIGN"},
		{LSHIFT_ASSIGN, "LSHIFT_ASSIGN"}, {RSHIFT_ASSIGN, "RSHIFT_ASSIGN"}, {AND_ASSIGN, "AND_ASSIGN"}, {XOR_ASSIGN, "XOR_ASSIGN"},

		//keywords
		{INT, "INT"}, {RETURN, "RETURN"}, {IF, "IF"}, {ELSE, "ELSE"}, {FOR, "FOR"}, {WHILE, "WHILE"}, {DO, "Do"}, {BREAK, "BREAK"}, {CONTINUE, "CONTINUE"}, {CHAR, "CHAR"}
	};
	

	struct Token {
	public:
		TokenType type;
		string id;
		int line;
		void print(int newline = -1) {
			std::cout << TokenToString[type];
			if (id != "") {
				std::cout << ", " << id;
			}
			if (newline == -1) {
				std::cout << "\n";
			}
		}
	};
	static void ReportError(Token token, TokenType expected = WHITESPACE, string msg = "") {

		if (expected != WHITESPACE) {
			if (expected == token.type) {
				return;
			}
		}
		if (msg != "") {
			std::cout << "Error on line " << std::to_string(token.line + 1);
			std::cout << msg;
			token.print(0);
		}
		else {
			std::cout << "Error on line " << std::to_string(token.line + 1) << " unexpected token ";
			token.print(0);
		}
		if (expected != WHITESPACE) {
			std::cout << " expected ";
			std::cout << TokenToString[expected];

		}
		exit(10);
	}
	static void CompileError(string message, int line) {
		std::cout << "Error on line " << line + 1 << " " << message;
		exit(1);
	}
}