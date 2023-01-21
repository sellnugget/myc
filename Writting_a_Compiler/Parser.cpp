#include "Parser.h"

namespace Parse {
	void Parser::PushToken(Node* node)
	{
		node->terminals.push_back(*_currentToken);
		_currentToken++;
	}
	bool Parser::ReadSequence(vector<TokenType> sequence)
	{
		vector<Token>::iterator start = _currentToken;
		for(int i = 0; i < sequence.size(); i++) {
			if (_currentToken >= _end) {
				_currentToken = start;
				return false;

			}
			if (_currentToken->type != sequence[i]) {
				_currentToken = start;
				return false;
			}
			_currentToken++;
		}
		_currentToken = start;
		return true;
	}

	bool Parser::IsUranaryExp()
	{
		vector<TokenType> uranary = {
			NEGATION, BITWISE_COMPLEMENT, LOGICAL_NEGATION
		};
		if (std::count(uranary.begin(), uranary.end(), _currentToken->type)) {
			return true;
		}
		return false;
	}
	bool Parser::IsConstExp()
	{
		vector<TokenType> consts = {
			INTEGER_LITERAL
		};
		if (std::count(consts.begin(), consts.end(), _currentToken->type)) {
			return true;
		}
		return false;
	}

	Node* Parser::DeclareFactor()
	{
		Node* out = new Node();
		out->type = "Factor";
		if (_currentToken->type == OPEN_BRACKET) {
			_currentToken++;
			out->Nodes.push_back(DeclareExp(0));
			if (_currentToken->type != CLOSE_BRACKET) {
				//error
				exit(13);
			}
			_currentToken++;
			return out;
		}
		else if (IsUranaryExp()) {
			PushToken(out);
			out->Nodes.push_back(DeclareFactor());
			return out;
		}
		else if (IsConstExp()) {
			PushToken(out);
			return out;
		}
		else if (_currentToken->type == IDENTIFIER) {
			PushToken(out);
			return out;
		}
		//error
		exit(13);
	}

	
	Node* Parser::DeclareExp(int level) {


		vector<vector<TokenType>> OperatorPrecedence = {
			{ASSIGNMENT},
			{OR},
			{AND},
			{EQUAL, NOT_EQUAL},
			{GREATER_THAN, LESS_THAN, GREATER_OR_EQUAL, LESS_OR_EQUAL},
			{ADD, SUBTRACT},
			{MULTIPLY, DIVIDE}
		};

		if (level == OperatorPrecedence.size()) {
			return DeclareFactor();
		}
		Node* out = new Node();
		out->type = "Expression";
		
		
		
		out->Nodes.push_back(DeclareExp(level + 1));

		TokenType next = _currentToken->type;

		bool optrue = std::count(OperatorPrecedence[level].begin(), OperatorPrecedence[level].end(), next);
		if (!optrue) {
			return out->Nodes[0];
		}
		while(optrue){
			out->terminals.push_back(*_currentToken);
			_currentToken++;
			out->Nodes.push_back(DeclareExp(level + 1));
			next = _currentToken->type;
			optrue = std::count(OperatorPrecedence[level].begin(), OperatorPrecedence[level].end(), next);
		}
		
		return out;
	}

	Node* Parser::DeclareVar()
	{
		Node* out = new Node();
		out->type = "Var1";

		PushToken(out);
		PushToken(out);
		if (_currentToken->type != SEMICOLIN) {
			_currentToken--;
				out->Nodes.push_back(DeclareExp(0));
		}
		_currentToken++;
		return out;
	}


	Node* Parser::DeclareStatement()
	{
		Node* out = new Node();
		if (_currentToken->type == OPEN_BRACE) {
			out->type = "Scope";
			_currentToken++;
			while (_currentToken->type != CLOSE_BRACE) {
				out->Nodes.push_back(DeclareStatement());
			}
			_currentToken++;
		}
		else if (_currentToken->type == RETURN) {
			out->type = "return";
			PushToken(out);
			out->Nodes.push_back(DeclareExp(0));
		}
		else if (_currentToken->type == INT) {
			out->type = "Var";
			PushToken(out);
			PushToken(out);
			out->Nodes.push_back(DeclareVar());
		}
		else {
			DeclareExp(0);
			if (_currentToken->type != SEMICOLIN) {
				exit(69);
			}
			_currentToken++;
		}

		return out;
	}
	Node* Parser::DeclareFunction()
	{
		Node* out = new Node();
		out->type = "Function";
		
		_currentToken++;
		//pushes name
		PushToken(out);

		//skips brackets
		_currentToken++;
		_currentToken++;
		out->Nodes.push_back(DeclareStatement());
		_currentToken++;

		return out;
	}
	Node* Parser::DeclareProgram()
	{
		Node* out = new Node();
		out->type = "Program";

		if (ReadSequence({ INT, IDENTIFIER, OPEN_BRACKET, CLOSE_BRACKET, OPEN_BRACE })) {
			out->Nodes.push_back(DeclareFunction());
		}
		else {
			throw("Error");
		}

		return out;
	}
	Node* Parser::ParseTokens(vector<Token> tokens)
	{
		_currentToken = tokens.begin();
		_end = tokens.end();
		return DeclareProgram();
	}
	int tabs = -1;
	void Parser::PrintAST(Node* node)
	{
		tabs++;
		string tab = "";
		for (int i = 0; i < tabs; i++) {
			tab += "\t";
		}
		std::cout << tab << node->type << "\n";
		for (int i = 0; i < node->terminals.size(); i++) {
			std::cout << "\t" << tab;
			node->terminals[i].print();
		}
		for (int i = 0; i < node->Nodes.size(); i++) {
			PrintAST(node->Nodes[i]);
		}
		tabs--;
	}

}
