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

		INTEGER_LITERAL, IDENTIFIER,

		//symbols

		OPEN_BRACE, CLOSE_BRACE, OPEN_BRACKET, CLOSE_BRACKET, SEMICOLIN, NEGATION, SUBTRACT = NEGATION, 
		BITWISE_COMPLEMENT, LOGICAL_NEGATION, ADD, MULTIPLY, DIVIDE,
		AND, OR, EQUAL, NOT_EQUAL, LESS_THAN, LESS_OR_EQUAL, GREATER_THAN, 
		GREATER_OR_EQUAL, ASSIGNMENT, 

		//keywords

		INT, RETURN
	};
	static std::map<TokenType, string> TokenToString =
	{
		{INTEGER_LITERAL, "INTEGER_LITERAL"}, {IDENTIFIER, "IDENTIFIER"},

		{OPEN_BRACE, "OPEN_BRACE"}, {CLOSE_BRACE, "CLOSE_BRACE"}, {OPEN_BRACKET, "OPEN_BRACKET"}, {CLOSE_BRACKET, "CLOSE_BRACKET"}, {SEMICOLIN, "SEMICOLIN"}, {NEGATION, "NEGATION"}, {BITWISE_COMPLEMENT, "BITWISE_COMPLEMENT"}, {LOGICAL_NEGATION, "LOGICAL_NEGATION"}, {ADD, "ADD"}, {MULTIPLY, "MULTIPLY"}, {DIVIDE, "DIVIDE"},

		{AND, "AND"}, {OR, "OR"}, {EQUAL,"EQUAL"}, {NOT_EQUAL, "NOT_EQUAL"}, {LESS_THAN, "LESS_THAN"},{LESS_OR_EQUAL, "LESS_OR_EQUAL"}, {GREATER_THAN, "GREATER_THAN"},
		{GREATER_OR_EQUAL, "GREATER_OR_EQUAL"},{ASSIGNMENT, "ASSIGNMENT"},
		{INT, "INT"}, {RETURN, "RETURN"},
	};
	

	struct Token {
	public:
		TokenType type;
		string id;
		int line;
		void print() {
			std::cout << TokenToString[type];
			if (id != "") {
				std::cout << ", " << id;
			}
			std::cout << "\n";
		}
	};
	static void ReportError(Token token, string msg = "") {
		if (msg != "") {
			std::cout << "Error on line " << std::to_string(token.line);
			std::cout << msg;
			token.print();
		}
	}
}