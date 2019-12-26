#include "tokenizer/tokenizer.h"

#include <cctype>
#include <sstream>

namespace miniplc0 {

	std::pair<std::optional<Token>, std::optional<CompilationError>> Tokenizer::NextToken() {
		if (!_initialized)
			readAll();
		if (_rdr.bad())
			return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(0, 0, ErrorCode::ErrStreamError));
		if (isEOF())
			return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(0, 0, ErrorCode::ErrEOF));
		auto p = nextToken();
		if (p.second.has_value())
			return std::make_pair(p.first, p.second);
		auto err = checkToken(p.first.value());
		if (err.has_value())
			return std::make_pair(p.first, err.value());
		return std::make_pair(p.first, std::optional<CompilationError>());
	}

	std::pair<std::vector<Token>, std::optional<CompilationError>> Tokenizer::AllTokens() {
		std::vector<Token> result;
		while (true) {
			auto p = NextToken();
			if (p.second.has_value()) {
				if (p.second.value().GetCode() == ErrorCode::ErrEOF)
					return std::make_pair(result, std::optional<CompilationError>());
				else
					return std::make_pair(std::vector<Token>(), p.second);
			}
			result.emplace_back(p.first.value());
		}
	}

	// 注意：这里的返回值中 Token 和 CompilationError 只能返回一个，不能同时返回。
	std::pair<std::optional<Token>, std::optional<CompilationError>> Tokenizer::nextToken() {
		// 用于存储已经读到的组成当前token字符
		std::stringstream ss;
		// 分析token的结果，作为此函数的返回值
		std::pair<std::optional<Token>, std::optional<CompilationError>> result;
		// <行号，列号>，表示当前token的第一个字符在源代码中的位置
		std::pair<int64_t, int64_t> pos;
		// 记录当前自动机的状态，进入此函数时是初始状态
		DFAState current_state = DFAState::INITIAL_STATE;
		// 这是一个死循环，除非主动跳出
		// 每一次执行while内的代码，都可能导致状态的变更
		while (true) {
			auto current_char = nextChar();
			switch (current_state) {

			case INITIAL_STATE: {
				if (!current_char.has_value())
					return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(0, 0, ErrEOF));

				auto ch = current_char.value();
				auto invalid = false;

				// 使用了自己封装的判断字符类型的函数，定义于 tokenizer/utils.hpp
				if (miniplc0::isspace(ch)) // 读到的字符是空白字符（空格、换行、制表符等）
					current_state = DFAState::INITIAL_STATE; // 保留当前状态为初始状态，此处直接break也是可以的
				else if (!miniplc0::isprint(ch)) // control codes and backspace
					invalid = true;
				else if (miniplc0::isdigit(ch)) // 读到的字符是数字
					current_state = DFAState::UNSIGNED_INTEGER_STATE; // 切换到无符号整数的状态
				else if (miniplc0::isalpha(ch)) // 读到的字符是英文字母
					current_state = DFAState::IDENTIFIER_STATE; // 切换到标识符的状态
				else {
					switch (ch) {
					case '=': // 如果读到的字符是`=`，则切换到等于号的状态
						current_state = DFAState::DY_STATE;
						break;
					case '-':
						current_state = DFAState::MINUS_STATE;
						break;
						// 请填空：切换到减号的状态
					case '+':
						current_state = DFAState::PLUS_STATE;
						break;
					case '*':
						current_state = DFAState::STAR_STATE;
						break;
					case '/':
						current_state = DFAState::DIV_STATE;
						break;
					case '>':
						current_state = DFAState::GREATER_STATE;
						break;
					case '<':
						current_state = DFAState::LESS_STATE;
						break;
					case '!':
						current_state = DFAState::GT_STATE;
						break;
					case ':':
						current_state = DFAState::COLON_STATE;
						break;
					case ';':
						current_state = DFAState::SEMICOLON_STATE;
						break;
					case '(':
						current_state = DFAState::ZKH_STATE;
						break;
					case ')':
						current_state = DFAState::YKH_STATE;
						break;
					case '[':
						current_state = DFAState::ZZKH_STATE;
						break;
					case ']':
						current_state = DFAState::YZKH_STATE;
					case '{':
						current_state = DFAState::ZDKH_STATE;
						break;
					case '}':
						current_state = DFAState::YDKH_STATE;
						break;
					case ',':
						current_state = DFAState::DH_STATE;
						break;
					default:
						invalid = true;
						break;
					}
				}
				// 如果读到的字符导致了状态的转移，说明它是一个token的第一个字符
				if (current_state != DFAState::INITIAL_STATE)
					pos = previousPos(); // 记录该字符的的位置为token的开始位置
				// 读到了不合法的字符
				if (invalid) {
					// 回退这个字符
					unreadLast();
					// 返回编译错误：非法的输入
					return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));
				}
				// 如果读到的字符导致了状态的转移，说明它是一个token的第一个字符
				if (current_state != DFAState::INITIAL_STATE) // ignore white spaces
					ss << ch; // 存储读到的字符
				break;
			}

							  // 当前状态是无符号整数
			case UNSIGNED_INTEGER_STATE: {
				// 请填空：
				// 如果当前已经读到了文件尾，则解析已经读到的字符串为整数
				if (!current_char.has_value()) {
					std::string sss = ss.str();
					int n = atoi(sss.c_str());
					return std::make_pair(std::make_optional<Token>(TokenType::UNSIGNED_INTEGER, n, pos, currentPos()), std::optional<CompilationError>());

				}
				auto ch = current_char.value();

				if (miniplc0::isdigit(ch))
					ss << ch;
				else if (ch == 'x' || ch == 'X') {
					ss << ch;
					current_state = DFAState::HEX_STATE;
				}
				else if (miniplc0::isalpha(ch) && (ch != 'x' && ch != 'X')) {
					ss << ch;
					current_state = DFAState::IDENTIFIER_STATE;
				}
				else {
					unreadLast();
					std::string sss = ss.str();
					int n = atoi(sss.c_str());
					return std::make_pair(std::make_optional<Token>(TokenType::UNSIGNED_INTEGER, n, pos, currentPos()), std::optional<CompilationError>());
				}
				if (current_state == DFAState::IDENTIFIER_STATE)
					return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(0, 0, ErrEOF));
				break;
			}

			case HEX_STATE: {
				if (!current_char.has_value()) {
					std::string sss = ss.str();
					int n = atoi(sss.c_str());
					return std::make_pair(std::make_optional<Token>(TokenType::UNSIGNED_INTEGER, n, pos, currentPos()), std::optional<CompilationError>());

				}
				auto ch = current_char.value();

				if (miniplc0::isdigit(ch))
					ss << ch;
				else if (miniplc0::isalpha(ch)) {
					ss << ch;
				}
				else {
					unreadLast();
					std::string sss = ss.str();
					char* meme;
					long long me = strtol(sss.c_str(), &meme, 16);
					int n = (int)me;
					//少了个溢出检查不过现在先不管啦。
					return std::make_pair(std::make_optional<Token>(TokenType::UNSIGNED_INTEGER, n, pos, currentPos()), std::optional<CompilationError>());
				}
				if (current_state == DFAState::IDENTIFIER_STATE)
					return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(0, 0, ErrEOF));
				break;
			}

			case IDENTIFIER_STATE: {
				// 请填空：
				// 如果当前已经读到了文件尾，则解析已经读到的字符串
				auto gg = false;
				if (!current_char.has_value())
					gg = true;
				auto ch = current_char.value();
				if (miniplc0::isdigit(ch) || miniplc0::isalpha(ch))
					ss << ch;
				else {
					unreadLast();
					gg = true;
				}

				//     如果解析结果是关键字，那么返回对应关键字的token，否则返回标识符的token
				// 如果读到的是字符或字母，则存储读到的字符
				// 如果读到的字符不是上述情况之一，则回退读到的字符，并解析已经读到的字符串
				//     如果解析结果是关键字，那么返回对应关键字的token，否则返回标识符的token

				if (gg) {
					if (ss.str().compare("const") == 0) {
						return std::make_pair(std::make_optional<Token>(TokenType::CONST, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					}
					else if (ss.str().compare("void") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::VOID, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("int") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::INT, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("char") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::CHAR, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("double") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::DOUBLE, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("struct") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::STRUCT, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("if") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::IF, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("else") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::ELSE, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("switch") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::SWITCH, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("case") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::CASE, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("default") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::DEFAULT, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("while") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::WHILE, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("for") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::FOR, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("do") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::DO, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("return") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::RETURN, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("break") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::BREAK, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("continue") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::CONTINUE, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("print") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::PRINT, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else if (ss.str().compare("scan") == 0)
						return std::make_pair(std::make_optional<Token>(TokenType::SCAN, ss.str(), pos, currentPos()),
							std::optional<CompilationError>());
					else return  std::make_pair(std::make_optional<Token>(TokenType::IDENTIFIER, ss.str(), pos, currentPos()), std::optional<CompilationError>());
				}
				break;
			}
								 // 如果当前状态是加号
			case PLUS_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::PLUS, '+', pos, currentPos()), std::optional<CompilationError>());
			}
						   // 当前状态为减号的状态
			case MINUS_STATE: {
				// 请填空：回退，并返回减号token
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::MINUS, '-', pos, currentPos()), std::optional<CompilationError>());
			}

			case STAR_STATE: {
				// 请填空：回退，并返回减号token
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::STAR, '*', pos, currentPos()), std::optional<CompilationError>());

			}
			case DIV_STATE: {
				// 请填空：回退，并返回减号token
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::_DIV, '/', pos, currentPos()), std::optional<CompilationError>());

			}
			case DY_STATE: {
				// 请填空：回退，并返回减号token
				auto ch = current_char.value();
				if (ch == '=')
					return std::make_pair(std::make_optional<Token>(TokenType::EQ, "==", pos, currentPos()), std::optional<CompilationError>());
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::FZ, '=', pos, currentPos()), std::optional<CompilationError>());

			}
			case LESS_STATE: {
				// 请填空：回退，并返回减号token
				auto ch = current_char.value();
				if (ch == '=')
					return std::make_pair(std::make_optional<Token>(TokenType::LOE, "<=", pos, currentPos()), std::optional<CompilationError>());
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::LESS, '<', pos, currentPos()), std::optional<CompilationError>());

			}
			case GREATER_STATE: {
				// 请填空：回退，并返回减号token
				auto ch = current_char.value();
				if (ch == '=')
					return std::make_pair(std::make_optional<Token>(TokenType::GOE, ">=", pos, currentPos()), std::optional<CompilationError>());
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::GREATER, '>', pos, currentPos()), std::optional<CompilationError>());

			}
			case GT_STATE: {
				// 请填空：回退，并返回减号token
				auto ch = current_char.value();
				if (ch == '=')
					return std::make_pair(std::make_optional<Token>(TokenType::NE, "!=", pos, currentPos()), std::optional<CompilationError>());
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::optional<Token>(), std::make_optional<CompilationError>(pos, ErrorCode::ErrInvalidInput));

			}
			case SEMICOLON_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::SEMICOLON, ';', pos, currentPos()), std::optional<CompilationError>());
			}
			case COLON_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::COLON, ':', pos, currentPos()), std::optional<CompilationError>());
			}
			case ZKH_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::ZKH, '(', pos, currentPos()), std::optional<CompilationError>());
			}
			case YKH_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::YKH, ')', pos, currentPos()), std::optional<CompilationError>());
			}
			case ZZKH_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::ZZKH, '[', pos, currentPos()), std::optional<CompilationError>());
			}
			case YZKH_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::YZKH, ']', pos, currentPos()), std::optional<CompilationError>());
			}
			case ZDKH_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::ZDKH, '{', pos, currentPos()), std::optional<CompilationError>());
			}
			case YDKH_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::YDKH, '}', pos, currentPos()), std::optional<CompilationError>());
			}
			case DH_STATE: {
				unreadLast(); // Yes, we unread last char even if it's an EOF.
				return std::make_pair(std::make_optional<Token>(TokenType::DOUHAO, ',', pos, currentPos()), std::optional<CompilationError>());
			}

			default:
				DieAndPrint("unhandled state.");
				break;
			}
			// ddd
		}
		// 预料之外的状态，如果执行到了这里，说明程序异常

		return std::make_pair(std::optional<Token>(), std::optional<CompilationError>());
	}

	std::optional<CompilationError> Tokenizer::checkToken(const Token& t) {
		switch (t.GetType()) {
		case IDENTIFIER: {
			auto val = t.GetValueString();
			if (miniplc0::isdigit(val[0]))
				return std::make_optional<CompilationError>(t.GetStartPos().first, t.GetStartPos().second, ErrorCode::ErrInvalidIdentifier);
			break;
		}
		default:
			break;
		}
		return {};
	}

	void Tokenizer::readAll() {
		if (_initialized)
			return;
		for (std::string tp; std::getline(_rdr, tp);)
			_lines_buffer.emplace_back(std::move(tp + "\n"));
		_initialized = true;
		_ptr = std::make_pair<int64_t, int64_t>(0, 0);
		return;
	}

	// Note: We allow this function to return a postion which is out of bound according to the design like std::vector::end().
	std::pair<uint64_t, uint64_t> Tokenizer::nextPos() {
		if (_ptr.first >= _lines_buffer.size())
			DieAndPrint("advance after EOF");
		if (_ptr.second == _lines_buffer[_ptr.first].size() - 1)
			return std::make_pair(_ptr.first + 1, 0);
		else
			return std::make_pair(_ptr.first, _ptr.second + 1);
	}

	std::pair<uint64_t, uint64_t> Tokenizer::currentPos() {
		return _ptr;
	}

	std::pair<uint64_t, uint64_t> Tokenizer::previousPos() {
		if (_ptr.first == 0 && _ptr.second == 0)
			DieAndPrint("previous position from beginning");
		if (_ptr.second == 0)
			return std::make_pair(_ptr.first - 1, _lines_buffer[_ptr.first - 1].size() - 1);
		else
			return std::make_pair(_ptr.first, _ptr.second - 1);
	}

	std::optional<char> Tokenizer::nextChar() {
		if (isEOF())
			return {}; // EOF
		auto result = _lines_buffer[_ptr.first][_ptr.second];
		_ptr = nextPos();
		return result;
	}

	bool Tokenizer::isEOF() {
		return _ptr.first >= _lines_buffer.size();
	}

	// Note: Is it evil to unread a buffer?
	void Tokenizer::unreadLast() {
		_ptr = previousPos();
	}
}