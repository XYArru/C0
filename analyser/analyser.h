#pragma once

#include "error/error.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"

#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>
#include <cstddef> // for std::size_t

namespace miniplc0 {

	typedef struct {
		char16_t type;
		int32_t index;
		bool	_const;
		bool _init;
	}Var;
	typedef struct {
		const char* _opr;
		std::string _x;
		std::string _y;
	}Opr;

	//value 在map中作为索引
	typedef struct {
		int32_t index;
		char16_t type;
	}ConstTable;

	typedef struct {
		int32_t index;
		int32_t name_index;
		int32_t num_par;
		int32_t level;
		char16_t type;
		//std::vector<Var> pars;//参数在LDT中的索引
	}Func;

	class Analyser final {
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _Sins({}), Sins({}), Ains({}), _current_pos(0, 0),
			_gdt({}), _ldt({}), _consts({}), _nextGp(0), _nextLp(0) {}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		// 唯二接口
		std::pair<std::vector<Opr*>, std::optional<CompilationError>> Analyse();

		void Analyser::printBinary(std::ostream& output);
	private:
		// 所有的递归子程序


		std::optional<CompilationError> analyseC0Program();
		std::optional<CompilationError> analyseVarDec();
		std::optional<CompilationError> Analyser::analyseInitDeclist();
		std::optional<CompilationError> analyseFunDef();
		std::optional<CompilationError> Analyser::analyseExp();
		std::optional<CompilationError> Analyser::analyseAExp();
		std::optional<CompilationError> Analyser::analyseMExp();
		std::optional<CompilationError> Analyser::analyseUExp();
		std::optional<CompilationError> Analyser::analysePExp();
		std::optional<CompilationError> analyseExpression();

		std::optional<CompilationError> analyseComp();
		std::optional<CompilationError> analyseStmtSeq();
		std::optional<CompilationError> analyseStmt();
		std::optional<CompilationError> analyseLoopStmt();
		std::optional<CompilationError> analyseJumpStmt();
		std::optional<CompilationError> analysePrintStmt();
		std::optional<CompilationError> analyseScanStmt();
		std::optional<CompilationError> Analyser::analysePar();
		std::optional<CompilationError> Analyser::analysePDL();
		std::optional<CompilationError> Analyser::analysePD();
		std::optional<CompilationError> Analyser::analyseFunCall();
		void Analyser::binary2byte(int number, std::ostream& output);
		void Analyser::binary4byte(int number, std::ostream& output);
		void Analyser::printBinaryInstruction(Opr* opr, std::ostream& output);


		// Token 缓冲区相关操作

		// 返回下一个 token
		std::optional<Token> nextToken();
		// 回退一个 token
		void unreadToken();

		// 下面是符号表相关操作

		// 添加变量、常量、未初始化的变量
		void addGdt(const Token& tk);
		void addLdt(const Token& tk);
		void clrLdt();
		// 是否被声明过
		bool isDeclared(const std::string&);
		// 是否是未初始化的变量
		// 是否是已初始化的变量
		bool isInitializedVariable(const std::string&);
		// 是否是常量
		bool isConstant(const std::string&);
		// 是否是常量
		bool Analyser::isConst(const std::string& s);
		bool Analyser::isInit(const std::string& s);
		bool Analyser::isVoid(const std::string& s);
		bool Analyser::isDclr(const std::string& s);
		void Analyser::addFunc(const Token& tk);
		Func* Analyser::getFunc(const std::string& s);
		ConstTable* Analyser::getConst(const std::string& s);
		void Analyser::addConstantF(const Token& tk);
		std::optional<CompilationError> Analyser::analyseCond();
		std::optional<CompilationError> Analyser::analyseCondStmt();
		std::optional<CompilationError> Analyser::analyseExpl();
		std::optional<CompilationError> Analyser::analysePrint();
		bool isFunc(const std::string&);
		// 获得 {变量，常量} 在全局栈上的偏移
		Var* getG(const std::string&);

		// 获得 {变量，常量} 在局部栈上的偏移
		Var* getL(const std::string&);

	public:
		std::vector<Token> _tokens;
		std::size_t _offset;
		std::vector<Instruction> _Sins;
		std::pair<uint64_t, uint64_t> _current_pos;
		// 函数名 -> 对应的指令集
		std::map <std::string, std::vector<Instruction>> _Ains;

		// 为了简单处理，我们直接把符号表耦合在语法分析里

		//std::string-> identifier
		std::map <std::string, ConstTable*> _consts;
		std::vector<Opr*> Sins;
		std::map<std::string, std::vector<Opr*>> Ains;
		std::map <std::string, Func*> _funcs;
		std::map <std::string, Var*> _gdt;
		std::map <std::string, Var*> _ldt;
		int32_t _nextGp = 0;
		int32_t _nextLp = 0;
		int32_t _nextConst = 0;
		int32_t _nextVar = 0;
		int32_t _nextFunc = 0;


	};
}
