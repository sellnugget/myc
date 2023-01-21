#pragma once
#include "Token.h";
#include "Parser.h"
namespace Parse {
	class Compiler
	{

		string _output;
		void Push(string instruction) {
			_output += instruction + "\n";
		}
		int _cmps = 0;
		void CompileNext(Node* input, int index);
		void CompileFactor(Node* factor);
		void CompileExpression(Node* exp);
		void CompileStatement(Node* state);

		void CompileFunction(Node* func);

	public:
		string Compile(Node* entry);
	};

}


