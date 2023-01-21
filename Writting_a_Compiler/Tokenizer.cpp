#include "Tokenizer.h"
#include <map>
namespace Parse {

	void Tokenizer::EndToken()
	{

		std::map<string, TokenType> keywords = { {"int", INT}, {"return", RETURN} };
		if (_currentToken != "") {
			if (_currentToken[0] >= '0' && _currentToken[0] <= '9') {
				
				_output.push_back({INTEGER_LITERAL , _currentToken,_linenumber});
			}
			else if (keywords.count(_currentToken)) {
				_output.push_back({ keywords[_currentToken], "",_linenumber});
			}
			else {
				_output.push_back({ IDENTIFIER, _currentToken, _linenumber });
			}
			_currentToken = "";
		}
	}
	bool Tokenizer::Symbol()
	{
		vector<std::pair<string, TokenType>> symbols =
		{
			{"{", OPEN_BRACE}, {"}", CLOSE_BRACE}, {"(", OPEN_BRACKET}, {")", CLOSE_BRACKET}, {";", SEMICOLIN},
			{"-", NEGATION}, {"~", BITWISE_COMPLEMENT}, {"!", LOGICAL_NEGATION},
			{"+", ADD}, {"*", MULTIPLY}, {"/", DIVIDE}, 
			{"&&", AND}, {"||", OR}, {"==", EQUAL}, {"!=", NOT_EQUAL}, {"<", LESS_THAN}, {"<=", LESS_OR_EQUAL}, {">", GREATER_THAN}, {">=", GREATER_OR_EQUAL},
			{"=", ASSIGNMENT}
		};
		string best = "";
		for (int i = 0; i < symbols.size(); i++) {
			if (_index + symbols[i].first.size() < _source.size()) {
				string testsymbol = _source.substr(_index, symbols[i].first.size());
				if (symbols[i].first == testsymbol && symbols[i].first.size() > best.size()) {
					best = symbols[i].first;
				}
			}
		}
		if (best != "") {
			for (int i = 0; i < symbols.size(); i++) {
				if (symbols[i].first == best) {
					EndToken();
					_output.push_back({symbols[i].second, "", _linenumber});
					_index += best.size() - 1;
					return true;
				}
			}
		}
		else {
			return false;
		}
	}
	bool Tokenizer::WhiteSpace()
	{
		if (_source[_index] == ' ' || _source[_index] == '\t' || _source[_index] == '\r' || _source[_index] == '\n') {
			EndToken();
			if (_source[_index] == '\n') {
				_linenumber++;
			}
			return true;
		}
		return false;
	}
	vector<Token> Tokenizer::LexTokens(string source)
	{
		_source = source;
		for (_index = 0; _index < _source.size(); _index++) {
			if (Symbol())
				continue;
			if (WhiteSpace())
				continue;
			_currentToken += _source[_index];
		}
		return _output;
	}

}
