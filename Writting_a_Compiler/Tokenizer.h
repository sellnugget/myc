#pragma once
#include "Token.h"
namespace Parse {

	class Tokenizer
	{
	private:
		void EndToken();
		bool Symbol();
		bool WhiteSpace();
		string _currentToken;
		string _source;
		int _linenumber;
		int _index;
		vector<Token> _output;
	
	public:

		vector<Token> LexTokens(string source);
	};

}

