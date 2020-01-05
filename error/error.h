#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <iostream>

namespace miniplc0 {

	inline void DieAndPrint(std::string condition) {
		std::cout << "Exception: " << condition << "\n";
		std::cout << "The program should not reach here.\n";
		std::cout << "Please check your program carefully.\n";
		std::cout << "If you believe it's not your fault, please report this to TAs.\n";
		std::abort();
		// or *((int*)114514) = 19260817;
	}

	// To keep it simple, we don't create an error system.
	enum ErrorCode {
		ErrNoError, // Should be only used internally.
		ErrStreamError,
		ErrEOF,
		ErrInvalidInput,
		ErrInvalidIdentifier,
		ErrIntegerOverflow, // int32_t overflow.
		ErrNoBegin,
		ErrNoEnd,
		ErrNeedIdentifier,
		ErrConstantNeedValue,
		ErrNoSemicolon,
		ErrInvalidVariableDeclaration,
		ErrIncompleteExpression,
		ErrNotDeclared,
		ErrAssignToConstant,
		ErrDuplicateDeclaration,
		ErrNotInitialized,
		ErrInvalidAssignment,
		ErrInvalidPrint,
		ErrConstNoInit,
		ErrCalcVoid,
		ErrTypedef,
		ErrMustBeIdentifier,
		ErrRedefine,
		ErrNoKH,
		ErrCompare,
		ErrNoIF,
		ErrNoWHILE,
		ErrNoScan
	};

	class CompilationError final {
	private:
		using uint64_t = std::uint64_t;
	public:

		friend void swap(CompilationError& lhs, CompilationError& rhs);

		CompilationError(uint64_t line, uint64_t column, ErrorCode err) :_pos(line, column), _err(err) {}
		CompilationError(std::pair<uint64_t, uint64_t> pos, ErrorCode err) : CompilationError(pos.first, pos.second, err) {}
		CompilationError(const CompilationError& ce) { _pos = ce._pos; _err = ce._err; }
		CompilationError(CompilationError&& ce) :CompilationError(0, 0, ErrorCode::ErrNoError) { swap(*this, ce); }
		CompilationError& operator=(CompilationError ce) { swap(*this, ce); return *this; }
		bool operator==(const CompilationError& rhs) const { return _pos == rhs._pos && _err == rhs._err; }

		std::string CtS(ErrorCode ec) {
			switch (ec) {
			case ErrNoKH:
				return "ErrNoKH";
				break;
			case ErrNoScan:
				return "ErrNoScan";
				break;
			case ErrNoWHILE:
				return "ErrNoWHILE";
				break;
			case ErrNoIF:
				return "ErrNoIF";
				break;
			case ErrCompare:
				return "ErrCompare";
				break;
			case ErrNoEnd:
				return "ErrNoEnd";
				break;
			case ErrStreamError:
				return "ErrStreamError";
				break;
			case ErrEOF:
				return "ErrEOF";
				break;
			case ErrInvalidInput:
				return "ErrInvalidInput";
				break;
			case ErrInvalidIdentifier:
				return "ErrInvalidIdentifier";
				break;
			case ErrIntegerOverflow:
				return "ErrIntegerOverflow";
				break;
			case ErrNoBegin:
				return "ErrNoBegin";
				break;
			case ErrNeedIdentifier:
				return "ErrNeedIdentifier";
				break;
			case ErrConstantNeedValue:
				return "ErrConstantNeedValue";
				break;
			case ErrNoSemicolon:
				return "ErrNoSemicolon";
				break;
			case ErrInvalidVariableDeclaration:
				return "ErrInvalidVariableDeclaration";
				break;
			case ErrIncompleteExpression:
				return "ErrIncompleteExpression";
				break;
			case ErrNotDeclared:
				return "ErrNotDeclared";
				break;
			case ErrAssignToConstant:
				return "ErrAssignToConstant";
				break;
			case ErrNotInitialized:
				return "ErrNotInitialized";
				break;
			case ErrDuplicateDeclaration:
				return "ErrDuplicateDeclaration";
				break;
			case ErrRedefine:
				return "ErrRedefine";
				break;
			case ErrMustBeIdentifier:
				return "ErrMustBeIdentifier";
				break;
			case ErrTypedef:
				return "ErrTypedef";
				break;
			case ErrCalcVoid:
				return "ErrCalcVoid";
				break;
			case ErrInvalidPrint:
				return "ErrInvalidPrint";
				break;
			case ErrConstNoInit:
				return "ErrConstNoInit";
				break;
			default:
				break;


			}
		}
		std::pair<uint64_t, uint64_t> GetPos()const { return _pos; }
		ErrorCode GetCode() const { return _err; }

		void print() {printf("Err at (%d,%d):\t%s\n",(int)_pos.first,(int)_pos.second,CtS(_err).c_str());}

	private:
		std::pair<uint64_t, uint64_t> _pos;
		ErrorCode _err;
	};


	inline void swap(CompilationError& lhs, CompilationError& rhs) {
		using std::swap;
		swap(lhs._pos, rhs._pos);
		swap(lhs._err, rhs._err);
	}
}