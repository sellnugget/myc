#pragma once
#include "Token.h";
#include "Parser.h"
#include <map>




namespace Parse {


	struct Function {
		string name;
		int args;
		bool defined = false;
		//if the user wants to make program harder to reverse engineer 
		uint64_t hash() {
			std::hash<string> hasher;
			return hasher(name);
		}
	};

	const string _function_prefix = "_";
	class Compiler
	{
		vector<string> _globals;
		vector<string> _code;
		void Push(string instruction) {
			_code.push_back(instruction);
		}
		int _cmps = 0;
		int _stackoffset = 4;
		string _activevar;
		std::map<string, int> _local_vars;
		std::stack<std::vector<string>*> _varscopes;
		std::map<string, Function> _functions;
		std::stack<std::pair<string, string>> _loopstack;
		string _loopexit;
		string _loopcontinue;
		std::vector<string>* _currentscope;
		bool IsAssignment(TokenType token);
		void CompileNext(Node* input, int index);
		void CompileFactor(Node* factor);
		void CompileAssign(Node* set);
		void CompileExpression(Node* exp);
		void CompileStatement(Node* state);

		void CompileFunction(Node* func);

	public:
		string Compile(Node* entry);
	};

}


