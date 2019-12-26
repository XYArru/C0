#include "argparse.hpp"

#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include "instruction/instruction.h"

#include <iostream>
#include <fstream>
using namespace miniplc0;

	std::vector<miniplc0::Token> _tokenize(std::istream& input) {
		miniplc0::Tokenizer tkz(input);
		auto p = tkz.AllTokens();
		if (p.second.has_value()) {
			//fmt::print(stderr, "Tokenization error: {}\n", p.second.value());
			exit(2);
		}
		return p.first;
	}

	void CA(std::istream& input, std::ostream& output) {
		
		auto vc = _tokenize(input);
		miniplc0::Analyser analyser(vc);
		auto err = analyser.Analyse();
		if (err.second.has_value()) {
			//printf("sth wrong with analyser");
			exit(0);
		}

		analyser.printBinary(output);
		return;
	}

	void SA(std::istream& input, std::ostream& output) {
		

		auto vc = _tokenize(input);
		miniplc0::Analyser analyser(vc);
		auto err = analyser.Analyse();
		if (err.second.has_value()) {
			//printf("sth wrong with analyser");
			exit(0);
		}

		output << ".constants:\n";
		auto citer = analyser._consts.begin();
		while (citer != analyser._consts.end()) {
			output << citer->second->index<<"\t"<< char(citer->second->type) << "\t\""<<citer->first<<"\"\n";
			citer++;
		}
		output << ".start:\n";
		
		auto siter = analyser.Sins.begin();
		int t = analyser.Sins.size();
		int i = 0;
		while (i < t) {
			output << i << "\t" << analyser.Sins.at(i)->_opr;
			if(!analyser.Sins.at(i)->_x.empty())
				output<< "\t" << analyser.Sins.at(i)->_x ;
			if (!analyser.Sins.at(i)->_y.empty())
				output << "," << analyser.Sins.at(i)->_y;
			siter++;
			i++;

			output << "\n";
		}
		
		output << ".functions:\n";
		auto fiter = analyser._funcs.begin();
		while (fiter != analyser._funcs.end()) {
			output << fiter->second->index << "\t" << fiter->second->name_index << "\t" << fiter->second->num_par << "\t" << fiter->second->level << "\n";
			fiter++;
		}
		
		auto xeonblade = analyser._funcs.begin();
		int b_lo = 0;
		while (xeonblade != analyser._funcs.end()) {
			std::string ss = xeonblade->first;

			printf("g %s \n", ss.c_str());
			output << '.' << 'F' << b_lo << ":\n";
			std::vector<Opr*> xbld = analyser.Ains[ss];
			auto itx = xbld.begin();
			int j = 0;
			while (itx != xbld.end()) {
				
				output << j << "\t" << analyser.Ains[ss].at(j)->_opr;
				if (!analyser.Ains[ss].at(j)->_x.empty()) {

					output << "\t" << analyser.Ains[ss].at(j)->_x.c_str();
				}
				if (!analyser.Ains[ss].at(j)->_y.empty()) {
					output << "," << analyser.Ains[ss].at(j)->_y;
				}
				itx++;
				j++;
				output << "\n";
			}
			xeonblade++;
			b_lo++;
		}
		
		return;
	}
	int main(int argc, char** argv) {
		argparse::ArgumentParser program("cc0");
		program.add_argument("-c")
			.default_value(false)
			.implicit_value(true)
			.help("将输入的 c0 源代码翻译为二进制目标文件");
		program.add_argument("-s")
			.default_value(false)
			.implicit_value(true)
			.help("将输入的 c0 源代码翻译为文本汇编文件");
		program.add_argument("input")
			.help("kick your asshole.");
		program.add_argument("-o", "--output")
			.required()
			.default_value(std::string("-"))
			.help("输出到指定的文件 file");


		try {
			program.parse_args(argc, argv);
		}
		catch (const std::runtime_error & err) {
			program.print_help();
			exit(2);
		}

		auto input_file = program.get<std::string>("input");
		auto output_file = program.get<std::string>("--output");
		std::istream* input;
		std::ostream* output;
		std::ifstream inf;
		std::ofstream outf;
		if (input_file != "-") {
			inf.open(input_file, std::ios::in);
			if (!inf) {
				//fmt::print(stderr, "Fail to open {} for reading.\n", input_file);
				exit(2);
			}
			input = &inf;
		}
		else
			input = &std::cin;
		if (output_file != "-") {
			if (program["-c"] == true) {
				outf.open(output_file, std::ios::out | std::ios::binary);
				if (!outf) {
					//fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
					exit(2);
				}
				output = &outf;
			}
			if (program["-s"] == true) {
				outf.open(output_file, std::ios::out | std::ios::trunc);
				if (!outf) {
					//fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
					exit(2);
				}
				output = &outf;
			}
		}
		else {
			if (program["-c"] == true) {
				outf.open("out", std::ios::out | std::ios::binary);
				if (!outf) {
					//fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
					exit(2);
				}
				output = &outf;
			}
			if (program["-s"] == true) {
				outf.open("out", std::ios::out | std::ios::trunc);
				if (!outf) {
					//fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
					exit(2);
				}
				output = &outf;
			}
		}
		//output = &std::cout;
		if (program["-c"] == true && program["-s"] == true) {
			//fmt::print(stderr, "You can only perform tokenization or syntactic analysis at one time.");
			exit(2);
		}
		if (program["-c"] == true) {
			CA(*input, *output);
		}
		else if (program["-s"] == true) {
			SA(*input, *output);
		}
		else {
			//fmt::print(stderr, "You must choose tokenization or syntactic analysis.");
			exit(2);
		}
		return 0;
	}
