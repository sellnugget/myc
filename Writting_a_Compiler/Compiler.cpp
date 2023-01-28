#include "Compiler.h"

namespace Parse {



	bool Compiler::IsAssignment(TokenType type)
	{
		switch (type) {
		case ASSIGNMENT:
			return true;
		case ADD_ASSIGN:
			return true;
		case SUB_ASSIGN:
			return true;
		case DIVIDE_ASSIGN:
			return true;
		case MULTIPLY_ASSIGN:
			return true;
		case MOD_ASSIGN:
			return true;
		case AND_ASSIGN:
			return true;
		case OR_ASSIGN:
			return true;
		case LSHIFT_ASSIGN:
			return true;
		case RSHIFT_ASSIGN:
			return true;
		case XOR_ASSIGN:
			return true; 
		default:
			return false;
		}
	}

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

		case MULTIPLY:
			CompileFactor(factor->Nodes[0]);
			Push("pop %eax");
			Push("movl (%eax), %eax");
			break;
		case NEGATION:
			CompileFactor(factor->Nodes[0]);
			Push("pop %eax");
			Push("neg %eax");
			break;
		case BITWISE_COMPLEMENT:
			CompileFactor(factor->Nodes[0]);
			Push("pop %eax");
			Push("neg %eax");
			break;
		case LOGICAL_NEGATION:
			CompileFactor(factor->Nodes[0]);
			Push("pop %eax");
			Push("cmpl $0, %eax");
			Push("movl $0, %eax");
			Push("sete %al");
			break;
		case INTEGER_LITERAL:
			Push("movl $" + factor->terminals[0].id + ", %eax");
			break;

		case STRING_LITERAL:
			_globals.push_back(".string \"" + factor->terminals[0].id + "\"");
			Push("lea text" + std::to_string(_globals.size() - 1) + string(", %eax"));
			break;

		case IDENTIFIER:
			if (_local_vars.count(factor->terminals[0].id)) {
				Push("movl " + std::to_string(_local_vars[factor->terminals[0].id]) + "(%ebp)" + ", %eax");
			}
			else {
				CompileError("undefined identifier " + factor->terminals[0].id, factor->terminals[0].line);
			}
		}
		

		}
		
		Push("push %eax");
	}

	void Compiler::CompileAssign(Node* set)
	{
		switch(set->terminals[0].type)
		{
			case IDENTIFIER:
				if (_local_vars.count(set->terminals[0].id)) {
					_activevar = std::to_string(_local_vars[set->terminals[0].id]) + "(%ebp)";
				}
				else {
					CompileError("undefined identifier " + set->terminals[0].id, set->terminals[0].line);
				}
				break;
			default:
				CompileExpression(set->Nodes[0]);
				Push("pop %eax");
				_activevar = "(%eax)";
				break;
		}
	}

	
	void Compiler::CompileExpression(Node* exp)
	{
		bool addcmp = 0;
		if (exp->type == "Factor") {
			CompileFactor(exp);
			return;
		}
		else if (exp->type == "Call") {
			string func_name = exp->terminals[0].id;
			if (_functions.count(func_name)) {
				if (_functions[func_name].args != exp->Nodes.size()) {
					CompileError("must have same number of args as function", exp->terminals[0].line);
				}
				for (int i = exp->Nodes.size() - 1; i > -1; i--) {
					CompileExpression(exp->Nodes[i]);
				}
			}
			else {
				CompileError("cannot call undeclared function", exp->terminals[0].line);
			}
			Push("call " + _function_prefix + func_name);
			Push("addl " + string("$") + std::to_string(_functions[func_name].args * 4) + string(", %esp"));
			Push("push %eax");
			return;
		}

		if (IsAssignment(exp->terminals[0].type)) {
			CompileAssign(exp->Nodes[0]);
			string active = _activevar;
			CompileExpression(exp->Nodes[1]);
		
			switch (exp->terminals[0].type) {
				case ASSIGNMENT:
					Push("pop %eax");
					Push("movl %eax, " + active);
					break;
				case ADD_ASSIGN:
					Push("pop %eax");
					Push("addl %eax, " + active);
					break;
				case SUB_ASSIGN:
					Push("pop %eax");
					Push("subl %eax, " + active);
					break;
				case DIVIDE_ASSIGN:
					Push("pop %ecx");
					Push("cdq");
					Push("movl " + active + ", %eax");
					Push("idivl %ecx"); 
					Push("movl %eax, " + active);
					break;
				case MULTIPLY_ASSIGN:
					Push("pop %eax");
					Push("imul %eax, " + active);
					break;
				case MOD_ASSIGN:
					Push("pop %ecx");
					Push("cdq");
					Push("movl " + active + ", %eax");
					Push("idivl %ecx");
					Push("movl %edx, " + active);
					break;
				case AND_ASSIGN:
					Push("pop %eax");
					Push("andl %eax, " + active);
					break;
				case OR_ASSIGN:
					Push("pop %eax");
					Push("orl %eax, " + active);
					break;
				case LSHIFT_ASSIGN:
					Push("pop %eax");
					Push("shlw %eax, " + active);
					break;
				case RSHIFT_ASSIGN:
					Push("pop %eax");
					Push("shrw %eax, " + active);
					break;
				case XOR_ASSIGN:
					Push("pop %eax");
					Push("xorl %eax, " + active);
					break;
			}
			Push("push %eax");
			return;

		}
		else {
			CompileNext(exp, 0);
		}
		
	
		if (exp->terminals.size() == 0) {
			Push("pop %eax");
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
				case MOD:
					Push("cdq");
					Push("idivl %ecx");
					Push("movl %edx, %eax");
					break;
				case LSHIFT:
					Push("shlw %ecx, %eax");
					break;
				case RSHIFT:
					Push("shrw %ecx, %eax");
					break;
				case BITAND:
					Push("andl %ecx, %eax");
					break;
				case BITOR:
					Push("orl %ecx, %eax");
					break;
				case XOR:
					Push("xorl %ecx, %eax");
					break;
			}
			Push("push %eax");
		}
		if (addcmp == 1) {
			Push("endcmp" + std::to_string(_cmps) + string(":"));
			_cmps++;
		}

	}
	void Compiler::CompileStatement(Node* state)
	{
		if (state->type == "Scope") {
			_varscopes.push(_currentscope);
			_currentscope = new vector<string>;
			if (state->terminals[0].id != "0") {
				Push("subl $" + state->terminals[0].id + ",%esp");
			}
			for (int i = 0; i < state->Nodes.size(); i++) {
				CompileStatement(state->Nodes[i]);
			}
			for (int i = 0; i < _currentscope->size(); i++) {
				_local_vars.erase((* _currentscope)[i]);
			}
			delete _currentscope;
			_currentscope = _varscopes.top();
			_varscopes.pop();
			if (state->terminals[0].id != "0") {
				Push("addl $" + state->terminals[0].id + ",%esp");
			}
			

		}
		else if (state->type == "Return") {
			CompileExpression(state->Nodes[0]);
			Push("pop %eax");
			Push("movl %ebp, %esp");
			Push("pop %ebp");
			Push("ret");
		}
		else if (state->type == "Var") {
			Node* var1;
			for (int i = 0; i < state->Nodes.size(); i++) {
				var1 = state->Nodes[i];
				if (_local_vars.count(var1->terminals[0].id)) {
					CompileError("redefinition of var " + var1->terminals[0].id, var1->terminals[0].line);
				}
				_local_vars[var1->terminals[0].id] = -_stackoffset;
				_currentscope->push_back(var1->terminals[0].id);
				_stackoffset+=4;
				if (var1->Nodes.size() > 0) {
					CompileExpression(var1->Nodes[0]);
				}
			}
		}
		else if (state->type == "IF") {
			CompileExpression(state->Nodes[0]);
			string elselable = "else" + std::to_string(_cmps);
			_cmps++;
			Push("pop %eax");
			Push("cmpl $0, %eax");
			Push("je " + elselable);
			CompileStatement(state->Nodes[1]);

			if (state->Nodes.size() > 2) {
				std::string iftrue = "if" + std::to_string(_cmps);
				_cmps++;
				Push("jmp " + iftrue);
				Push(elselable + ":");
				CompileStatement(state->Nodes[2]);
				Push(iftrue + ":");
			}
			else {
				Push(elselable + ":");
			}
		}
		else if (state->type == "FOR") {

			if (_loopexit != "") {
				_loopstack.push({ _loopexit, _loopcontinue });
			}
			CompileStatement(state->Nodes[0]);
			string beginforloop = "begin_for_loop" + std::to_string(_cmps);
			Push(beginforloop + string(":"));
			_loopexit = "exit_for" + std::to_string(_cmps);
			_loopcontinue = "for_continue" + std::to_string(_cmps);
			_cmps++;
			CompileExpression(state->Nodes[1]);
			Push("pop %eax");
			Push("cmpl $0, %eax");
			Push("je " + _loopexit);
			CompileStatement(state->Nodes[3]);
			Push(_loopcontinue + ":");
			CompileExpression(state->Nodes[2]);
			Push("pop %eax");
			Push("jmp " + beginforloop);
			Push(_loopexit + ":");
			if (_loopstack.size() > 0) {
				_loopexit = _loopstack.top().first;
				_loopcontinue = _loopstack.top().second;
				_loopstack.pop();
			}

		}

		else if (state->type == "WHILE") {


			if (_loopexit != "") {
				_loopstack.push({ _loopexit, _loopcontinue });
			}
			_loopcontinue = "while_loop" + std::to_string(_cmps);
			_loopexit = "exit_while" + std::to_string(_cmps);
			_cmps++;
			Push(_loopcontinue + ":");
			CompileExpression(state->Nodes[0]);
			Push("pop %eax");
			Push("cmpl $0, %eax");
			Push("je " + _loopexit);
			CompileStatement(state->Nodes[1]);
			Push("jmp " + _loopcontinue);
			Push(_loopexit + ":");
			if (_loopstack.size() > 0) {
				_loopexit = _loopstack.top().first;
				_loopcontinue = _loopstack.top().second;
				_loopstack.pop();
			}
		}
		else if (state->type == "DO") {
			
		}
		else if (state->type == "BREAK") {
			if (_loopcontinue != "") {
				Push("jmp " + _loopexit);
			}
			else {
				CompileError("you can only break in loops", state->terminals[0].line);
			}

		}
		else if (state->type == "CONTINUE") {
			if (_loopcontinue != "") {
				Push("jmp " + _loopcontinue);
			}
			else {
			CompileError("you can only continue in loops", state->terminals[0].line);
			}
		}
		else if (state->type == "Expression" || state->type == "Call" || state->type == "Factor") {
			CompileExpression(state);
			Push("pop %eax");
		}
	}

	void Compiler::CompileFunction(Node* func)
	{
		string function_name = func->terminals[0].id;

		bool forward_declared = _functions.count(function_name);
		if (forward_declared && _functions[function_name].defined == true) {
			CompileError("Function " + function_name + " already declared", func->terminals[0].line);
		}

		
		vector<string> args;
		for (int i = 1; i < func->terminals.size(); i++) {
			string current_arg = func->terminals[i].id;
			args.push_back(current_arg);
		}
		if (forward_declared && args.size() != _functions[function_name].args) {
			CompileError("can't redefine function with different number of args", func->terminals[0].line);
		}
		_functions[function_name] = { function_name, (int)args.size()};
		if (func->type == "Forward_Declare") {
			return;
		}
		_functions[function_name].defined = true;
		Push(".globl " + _function_prefix + function_name);
		Push(_function_prefix + function_name + ":");
		Push("push %ebp");
		Push("movl %esp, %ebp");
		for (int i = 0; i < args.size(); i++) {
			_local_vars[args[i]] = 4 * i + 4;
		}
		CompileStatement(func->Nodes[0]);
		Push("movl $0, %eax");
		Push("movl %ebp, %esp");
		Push("pop %ebp");
		Push("ret");
		_local_vars.clear();
	}

	string Compiler::Compile(Node* entry)
	{

		for (int i = 0; i < entry->Nodes.size(); i++) {
			string noneterminal = entry->Nodes[i]->type;
			if (noneterminal == entry->Nodes[i]->type) {
				CompileFunction(entry->Nodes[i]);
			}
		}
		for (int i = 0; i < _globals.size(); i++) {
			Push(".globl " + string("text") + std::to_string(i));
			Push(".data");
			Push("text" + std::to_string(i) + ":");
			Push(_globals[i]);
		}
		string output = "";
		string litem = "";
		string word = "";
		string item = "";
		int index = 0;
		bool npop = 0;
		for (int i = 0; i < _code.size(); i++) {
			int offset = 0;
			while (offset < _code[i].size() && _code[i][offset] != ' ') {
				word += _code[i][offset];
				offset++;
			}
			if (offset >= _code[i].size()) {
				output += _code[i] + "\n";
				item = "";
				word = "";
				continue;
			}
			offset++;
			while (offset < _code[i].size()) {
				item += _code[i][offset];
				offset++;
			}
			if (word == "push") {
				npop = 1;
			}
			else if (npop && word == "pop" && litem == item) {
				item = "";
				word = "";
				litem = "";
				npop = 0;
				continue;
			}
			else if(npop) {
				output += _code[i - 1] + '\n';
				output += _code[i] + '\n';
			}
			else {
				output += _code[i] + '\n';
			}
			if (word != "push") {
				npop = 0;
			}
			litem = item;
			item = "";
			word = "";
		}
		return output;
	}

}
