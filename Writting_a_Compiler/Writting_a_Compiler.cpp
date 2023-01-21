// Writting_a_Compiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "Tokenizer.h"
#include "Parser.h"
#include "Compiler.h"
#include <windows.h>
int main()
{
	std::ifstream myfile;
	std::string input = "TestProgram.myc";
	myfile.open(input);
	std::string sourcefile;
	if (myfile.is_open()) {
		char mychar;
		while (myfile) {
			mychar = myfile.get();
			sourcefile += mychar;
		}
	}
	myfile.close();
	Parse::Tokenizer tokenizer;
	std::vector<Parse::Token> tokens = tokenizer.LexTokens(sourcefile);
	for (int i = 0; i < tokens.size(); i++) {
		tokens[i].print();
	}

	Parse::Parser parser;
	Parse::Node* node = parser.ParseTokens(tokens);
	parser.PrintAST(node);

	Parse::Compiler compiler;
	std::string output = compiler.Compile(node);
	std::cout << "Compiled Output\n\n";
	std::cout << output;

	std::string output_file;
	bool wait = true;
	for (int i = input.size() - 1; i > -1; i--) {
		if (input[i] == '.') {
			wait = false;
		}
		if (!wait) {
			output_file += input[i];
		}
	}
	input = "";
	for (int i = output_file.size() - 1; i > -1; i--) {
		input += output_file[i];
	}
	output_file = input;
	output_file += "s";
	std::ofstream out;
	out.open(output_file);
	out.write(output.c_str(), output.size());
	std::string assemble_command = "gcc -m32 " + output_file + " -o out";
	system(assemble_command.c_str());

	 
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
