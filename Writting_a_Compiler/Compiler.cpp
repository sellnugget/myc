#include "Compiler.h"

namespace Parse {





	void Compiler::CompileNext(Node* input, int index)
	{
		if (input->Nodes[0]->type == "Factor") {
			CompileFactor(input->Nodes[0]);
		}
		else {
			CompileExpression(input->Nodes[0]);
		}

	}

	void Compiler::CompileFactor(Node* factor)
	{

		if (factor->terminals.size() == 0) {
			CompileExpression(factor->Nodes[0]);
		}
		else {
			switch (factor->terminals[0].type)
		{
		case NEGATION:
			CompileFactor(factor->Nodes[0]);
			_output += "pop %eax\n";
			_output += "neg %eax\n";
			break;
		case BITWISE_COMPLEMENT:
			CompileFactor(factor->Nodes[0]);
			_output += "pop %eax\n";
			_output += "neg %eax\n";
			break;
		case LOGICAL_NEGATION:
			CompileFactor(factor->Nodes[0]);
			_output += "pop %eax\n";
			_output += "cmpl $0, %eax\n";
			_output += "movl $0, %eax\n";
			_output += "sete %al\n";
			break;
		case INTEGER_LITERAL:
			_output += "movl $" + factor->terminals[0].id + ", %eax\n";
			break;
		}
		}
		
		_output += "push %eax\n";
	}

	
	void Compiler::CompileExpression(Node* exp)
	{
		bool addcmp = 0;
		if (exp->type == "Factor") {
			CompileFactor(exp);
			return;
		}
		CompileNext(exp, 0);
		if (exp->terminals.size() == 0) {
			_output += "pop %eax\n";
		}

		
		for (int i = 0; i < exp->terminals.size(); i++) {

			CompileExpression(exp->Nodes[i + 1]);
			Push("pop %ecx");
			Push("pop %eax");
			switch (exp->terminals[i].type) {
				case ADD:
					Push("addl %ecx, %eax");
					break;
				case SUBTRACT:
					Push("subl %ecx, %eax");
					break;
				case DIVIDE:
					Push("cdq");
					Push("idivl %ecx");
					break;
				case MULTIPLY:
					Push("imul %ecx, %eax");
					break;
				case EQUAL:
					Push("cmpl %ecx, %eax");
					Push("movl $0, %eax");
					Push("sete %al");
					break;
				case NOT_EQUAL:
					Push("cmpl %ecx, %eax");
					Push("movl $0, %eax");
					Push("setne %al");
					break;
				case GREATER_THAN:
					Push("cmpl %ecx, %eax");
					Push("movl $0, %eax");
					Push("setg %al");
					break;
				case GREATER_OR_EQUAL:
					Push("cmpl %ecx, %eax");
					Push("movl $0, %eax");
					Push("setge %al"); 
					break;
				case LESS_THAN:
					Push("cmpl %ecx, %eax");
					Push("movl $0, %eax");
					Push("setl %al");
					break;
				case LESS_OR_EQUAL:
					Push("cmpl %ecx, %eax");
					Push("movl $0, %eax");
					Push("setle %al");
					break;
				case OR:
					Push("cmpl $1, %eax");
					Push("je endcmp" + std::to_string(_cmps));
					Push("cmpl $1, %ecx");
					Push("je endcmp" + std::to_string(_cmps));
					addcmp = true;
					break;
				case AND:
					Push("cmpl $0, %eax");
					Push("je endcmp" + std::to_string(_cmps));
					Push("movl %ecx, %eax");
					Push("cmpl $0, %eax");
					Push("je endcmp" + std::to_string(_cmps));
					addcmp = true;
					break;
			}
			if (i + 1 < exp->terminals.size()) {
				Push("push %eax");
			}
		}
		if (addcmp == 1) {
			Push("endcmp" + std::to_string(_cmps) + string(":"));
			_cmps++;
		}

	}
	void Compiler::CompileStatement(Node* state)
	{
		if (state->terminals[0].type == RETURN) {
			CompileExpression(state->Nodes[0]);
			_output += "ret\n";
		}
	}

	void Compiler::CompileFunction(Node* func)
	{
		_output += ".globl _" + func->terminals[0].id + "\n";
		_output += "_" + func->terminals[0].id + ":\n";
		CompileStatement(func->Nodes[0]);
	}

	string Compiler::Compile(Node* entry)
	{

		for (int i = 0; i < entry->Nodes.size(); i++) {
			string noneterminal = entry->Nodes[i]->type;
			if (noneterminal == entry->Nodes[i]->type) {
				CompileFunction(entry->Nodes[i]);
			}
		}
		return _output;
	}

}
