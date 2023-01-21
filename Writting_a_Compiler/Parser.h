#pragma once
#include "Token.h"
namespace Parse {

	struct Node {
		string type;
		vector<Token> terminals;
		vector<Node*> Nodes;
	};

	class Parser
	{
	private:
		vector<Token>::iterator _end;

		void PushToken(Node* node);
		bool ReadSequence(vector<TokenType> sequence);
		bool IsUranaryExp();
		bool IsConstExp();

		Node* DeclareFactor();
		Node* DeclareExp(int level);
		Node* DeclareVar();
		Node* DeclareStatement();
		Node* DeclareFunction();
		Node* DeclareProgram();
	public:
		
		vector<Token>::iterator _currentToken;

		Node* ParseTokens(vector<Token> tokens);

		void PrintAST(Node* node);
	};
}
