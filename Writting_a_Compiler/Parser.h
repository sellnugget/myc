#pragma once
#include "Token.h"
#include <stack>
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
		bool IsVar();
		int GetTypeSize(TokenType tokentype);
		int currentscope;
		std::stack<int> scopes;
		Node* DeclareFactor();
		
		Node* DeclareExp(int level);
		Node* DeclareVar(int size);
		Node* DeclareType();
		Node* DeclareStatement();
		Node* DeclareFunction();
		Node* DeclareProgram();
	public:
		
		vector<Token>::iterator _currentToken;

		Node* ParseTokens(vector<Token> tokens);

		void PrintAST(Node* node);
	};
}
