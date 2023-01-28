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
			NEGATION, BITWISE_COMPLEMENT, LOGICAL_NEGATION, MULTIPLY
		};
		if (std::count(uranary.begin(), uranary.end(), _currentToken->type)) {
			return true;
		}
		return false;
	}
	bool Parser::IsConstExp()
	{
		vector<TokenType> consts = {
			INTEGER_LITERAL, STRING_LITERAL
		};
		if (std::count(consts.begin(), consts.end(), _currentToken->type)) {
			return true;
		}
		return false;
	}

	bool Parser::IsVar()
	{
		switch (_currentToken->type) {
		case INT:
			return true;
		case CHAR:
			return true;
		}
		return false;
	}

	int Parser::GetTypeSize(TokenType tokentype)
	{
		switch (tokentype) {
		case INT:
			return 4;
		case CHAR:
			return 1;
		}
		return 0;
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
			if (_currentToken->type == OPEN_BRACKET) {
				out->type = "Call";
				_currentToken++;
				bool mustcomma = 0;
				while (_currentToken->type != CLOSE_BRACKET) {
					out->Nodes.push_back(DeclareExp(0));
					if (_currentToken->type == COMMA) {
						_currentToken++;
						mustcomma = 1;
					}
					else {
						mustcomma = 0;
					}
				}
				_currentToken++;
				if (mustcomma == 1) {
					ReportError(*_currentToken, WHITESPACE, "must put expression after comma");
				}
			}
		
			return out;
		}
		ReportError(*_currentToken);
	}



	
	Node* Parser::DeclareExp(int level) {

		Node* out = new Node();
		out->type = "Expression";

		vector<vector<TokenType>> OperatorPrecedence = {
			{ASSIGNMENT, ADD_ASSIGN, SUB_ASSIGN, MULTIPLY_ASSIGN, DIVIDE_ASSIGN, 
			MOD_ASSIGN, LSHIFT_ASSIGN, RSHIFT_ASSIGN, AND_ASSIGN, XOR_ASSIGN, OR_ASSIGN},
			{OR},
			{AND},
			{BITOR},
			{XOR},
			{BITAND},
			{EQUAL, NOT_EQUAL},
			{GREATER_THAN, LESS_THAN, GREATER_OR_EQUAL, LESS_OR_EQUAL},
			{LSHIFT, RSHIFT},
			{ADD, SUBTRACT},
			{MULTIPLY, DIVIDE, MOD}
		};

		if (level == OperatorPrecedence.size()) {
			return DeclareFactor();
		}

		
		
		
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

	Node* Parser::DeclareVar(int size)
	{
		Node* out = new Node();
		out->type = "Var1";
		currentscope += size;
		ReportError(*_currentToken, IDENTIFIER);
		PushToken(out);
		if (_currentToken->type != COMMA && _currentToken->type != SEMICOLIN) {
			_currentToken--;
				out->Nodes.push_back(DeclareExp(0));
		}
		return out;
	}
	Node* Parser::DeclareType()
	{
		return nullptr;
	}

	Node* Parser::DeclareStatement()
	{
		
		Node* out = new Node();


		if (IsVar()) {
			out->type = "Var";
			PushToken(out);
			int ptr = 0;
			while (_currentToken->type == MULTIPLY) {
				ptr++;
				_currentToken++;
			}
			int typesize = GetTypeSize(out->terminals[0].type);
			if (ptr > 0) {
				typesize = 4;
			}
			//how much pointer is it?
			out->terminals.push_back({INTEGER_LITERAL, std::to_string(ptr), 0});
			while (_currentToken->type != SEMICOLIN) {
				
				out->Nodes.push_back(DeclareVar(typesize));
				if (_currentToken->type == COMMA) {
					_currentToken++;
				}
				else if (_currentToken->type != SEMICOLIN) {
					ReportError(*_currentToken, SEMICOLIN);
				}

			}
			
			return out;
		}

		switch (_currentToken->type) {
			case OPEN_BRACE:
				out->type = "Scope";
				scopes.push(currentscope);
				currentscope = 0;
				_currentToken++;
				while (_currentToken->type != CLOSE_BRACE) {
					out->Nodes.push_back(DeclareStatement());
				}
				out->terminals.push_back({ INTEGER_LITERAL, std::to_string(currentscope), 0 });
				_currentToken++;
				currentscope = scopes.top();
				scopes.pop();
				break;

			case RETURN:
				out->type = "Return";
				_currentToken++;
				out->Nodes.push_back(DeclareExp(0));
				ReportError(*_currentToken, SEMICOLIN);
				_currentToken++;
				break;

			case IF:
				out->type = "IF";
				_currentToken++;
				ReportError(*_currentToken, OPEN_BRACKET);
				_currentToken++;
				out->Nodes.push_back(DeclareExp(0));
				ReportError(*_currentToken, CLOSE_BRACKET);
				_currentToken++;
				out->Nodes.push_back(DeclareStatement());

				if (_currentToken->type == ELSE) {
					PushToken(out);
					out->Nodes.push_back(DeclareStatement());
				}
				break;

			case FOR:
				out->type = "Scope";
				scopes.push(currentscope);
				currentscope = 0;
				{
					Node* fornode = new Node();
					fornode->type = "FOR";
					_currentToken++;
					ReportError(*_currentToken, OPEN_BRACKET);
					_currentToken++;
					fornode->Nodes.push_back(DeclareStatement());
					fornode->Nodes.push_back(DeclareExp(0));
					ReportError(*_currentToken, SEMICOLIN);
					_currentToken++;
					fornode->Nodes.push_back(DeclareExp(0));
					ReportError(*_currentToken, CLOSE_BRACKET);
					_currentToken++;
					fornode->Nodes.push_back(DeclareStatement());
					out->Nodes.push_back(fornode);
					out->terminals.push_back({ INTEGER_LITERAL, std::to_string(currentscope), 0 });
					currentscope = scopes.top();
					scopes.pop();

				}
				break;
			
			case WHILE:
				out->type = "WHILE";
				_currentToken++;
				ReportError(*_currentToken, OPEN_BRACKET);
				_currentToken++;
				out->Nodes.push_back(DeclareExp(0));
				ReportError(*_currentToken, CLOSE_BRACKET);
				_currentToken++;
				out->Nodes.push_back(DeclareStatement());
				break;
			case DO:
				out->type = "DO";
				_currentToken++;
				out->Nodes.push_back(DeclareStatement());
				ReportError(*_currentToken, WHILE);
				_currentToken++;
				ReportError(*_currentToken, OPEN_BRACKET);
				_currentToken++;
				out->Nodes.push_back(DeclareExp(0));
				ReportError(*_currentToken, CLOSE_BRACKET);
				_currentToken++;
				ReportError(*_currentToken, SEMICOLIN);
				_currentToken++;
				break;
			case BREAK:
				out->type = "BREAK";
				PushToken(out);
				ReportError(*_currentToken, SEMICOLIN);
				_currentToken++;
				break;
			case CONTINUE:
				out->type = "CONTINUE";
				PushToken(out);
				ReportError(*_currentToken, SEMICOLIN);
				_currentToken++;
				break;
			default:
			
				if (_currentToken->type == SEMICOLIN) {
					_currentToken++;
					Node* empty = new Node();
					empty->terminals.push_back({ INTEGER_LITERAL, "1", 0 });
					empty->type = "Factor";
					out->Nodes.push_back(empty);
					break;
				}
				delete out;
				out = DeclareExp(0);
				ReportError(*_currentToken, SEMICOLIN);
				_currentToken++;
		}

		return out;
	}
	Node* Parser::DeclareFunction()
	{
		Node* out = new Node();
		out->type = "Function";
		

		//return type
		_currentToken++;
		
		//pushes name
		PushToken(out);

		//skips brackets
		ReportError(*_currentToken, OPEN_BRACKET);
		_currentToken++;
		
		//get function args
		if (_currentToken->type == CLOSE_BRACKET) {
			_currentToken++;
			goto skip_args;
		}
		while (1) {
			ReportError(*_currentToken, INT);
			_currentToken++;
			ReportError(*_currentToken, IDENTIFIER);
			PushToken(out);
			if (_currentToken->type == COMMA) {
				_currentToken++;
				continue;
			}
			else if (_currentToken->type == CLOSE_BRACKET) {
				_currentToken++;
				break;
			}
			else {
				ReportError(*_currentToken);
			}
		}
		skip_args:
		if (_currentToken->type == SEMICOLIN) {
			_currentToken++;
			out->type = "Forward_Declare";
		}
		else {
			out->Nodes.push_back(DeclareStatement());
		}
		

		return out;
	}
	Node* Parser::DeclareProgram()
	{ 
		Node* out = new Node();
		out->type = "Program";
		while (_currentToken != _end &&  _currentToken->type == INT) {
			if (_currentToken->type == INT) {
				out->Nodes.push_back(DeclareFunction());
			}
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
