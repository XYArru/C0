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

		void printBinary(std::ostream& output);
	private:
		// 所有的递归子程序


		std::optional<CompilationError> analyseC0Program();
		std::optional<CompilationError> analyseVarDec();
		std::optional<CompilationError> analyseInitDeclist();
		std::optional<CompilationError> analyseInitDec();
		std::optional<CompilationError> analyseFunDef();
		std::optional<CompilationError> analyseExp();
		std::optional<CompilationError> analyseAExp();
		std::optional<CompilationError> analyseMExp();
		std::optional<CompilationError> analyseUExp();
		std::optional<CompilationError> analysePExp();
		std::optional<CompilationError> analyseComp();
		std::optional<CompilationError> analyseStmtSeq();
		std::optional<CompilationError> analyseStmt();
		std::optional<CompilationError> analyseLoopStmt();
		std::optional<CompilationError> analyseForinitStmt();
		std::optional<CompilationError> analyseJumpStmt();
		std::optional<CompilationError> analysePrintStmt();
		std::optional<CompilationError> analyseScanStmt();
		std::optional<CompilationError> analysePar();
		std::optional<CompilationError> analysePDL();
		std::optional<CompilationError> analysePD();
		std::optional<CompilationError> analyseFunCall();
		void binary2byte(int number, std::ostream& output);
		void binary4byte(int number, std::ostream& output);
		void printBinaryInstruction(Opr* opr, std::ostream& output);


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
		// 是否是未初始化的变量
		// 是否是已初始化的变量
		// 是否是常量
		// 是否是常量
		bool isConst(const std::string& s);
		bool isInit(const std::string& s);
		bool isVoid(const std::string& s);
		bool isDclr(const std::string& s);
		bool isClDclr(const std::string& s);
		void addFunc(const Token& tk);
		Func* getFunc(const std::string& s);
		ConstTable* getConst(const std::string& s);
		void addConstantF(const Token& tk);
		std::optional<CompilationError> analyseCond();
		std::optional<CompilationError> analyseCondStmt();
		std::optional<CompilationError> analyseExpl();
		std::optional<CompilationError> analysePrint();
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
