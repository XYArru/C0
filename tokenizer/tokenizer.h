#pragma once

#include "tokenizer/token.h"
#include "tokenizer/utils.hpp"
#include "error/error.h"

#include <utility>
#include <optional>
#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace miniplc0 {

	class Tokenizer final {
	private:
		using uint64_t = std::uint64_t;

		// 状态机的所有状态
		enum DFAState {
			INITIAL_STATE,
			UNSIGNED_INTEGER_STATE,
			HEX_STATE,
			PLUS_STATE,
			MINUS_STATE,
			DIV_STATE,
			STAR_STATE,
			DY_STATE,  // =
			IDENTIFIER_STATE,
			DH_STATE,
			SEMICOLON_STATE,
			COLON_STATE,
			GT_STATE,
			GREATER_STATE,
			LESS_STATE,
			ZKH_STATE,
			YKH_STATE,
			ZZKH_STATE,
			YZKH_STATE,
			ZDKH_STATE,
			YDKH_STATE
		};
	public:
		Tokenizer(std::istream& ifs)
			: _rdr(ifs), _initialized(false), _ptr(0, 0), _lines_buffer() {}
		Tokenizer(Tokenizer&& tkz) = delete;
		Tokenizer(const Tokenizer&) = delete;
		Tokenizer& operator=(const Tokenizer&) = delete;

		// 核心函数，返回下一个 token
		std::pair<std::optional<Token>, std::optional<CompilationError>> NextToken();
		// 一次返回所有 token
		std::pair<std::vector<Token>, std::optional<CompilationError>> AllTokens();
	private:
		// 检查 Token 的合法性
		std::optional<CompilationError> checkToken(const Token&);

		std::pair<std::optional<Token>, std::optional<CompilationError>> nextToken();

		void readAll();
		std::pair<uint64_t, uint64_t> nextPos();
		std::pair<uint64_t, uint64_t> currentPos();
		std::pair<uint64_t, uint64_t> previousPos();
		std::optional<char> nextChar();
		bool isEOF();
		void unreadLast();
	private:
		std::istream& _rdr;
		// 如果没有初始化，那么就 readAll
		bool _initialized;
		// 指向下一个要读取的字符
		std::pair<uint64_t, uint64_t> _ptr;
		// 以行为基础的缓冲区
		std::vector<std::string> _lines_buffer;
	};

}
