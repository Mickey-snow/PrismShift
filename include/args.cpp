#include "args.hpp"

#include<string>
#include<vector>

namespace args{
  
  const char* Parser::RawArg_at(const int& index) const{
  return argv[index].c_str();
  }

  int Parser::ArgCount(void) const{ return founded_argument.size(); }
  int Parser::Raw_ArgCount(void) const{ return argv.size(); }
  Parser::ArgumentMarshaler Parser::Arg_At(const int &offset) const{
    return founded_argument[offset];
  }
  
  bool isOption(const std::string& arg){ return arg[0] == '-'; }
  void Parser::Parse(void){
    founded_argument.push_back(*ArgumentMarshaler::NewWithOptAndPara("--program-name", argv[0]));

    for(int i=1;i<argv.size();++i){
      if(argv[i].length() < 1) continue;
      const std::string nowArg = argv[i];

      if(isOption(nowArg)){
	const std::string nextArg = (i+1)<argv.size() ? argv[i+1] : std::string{};
	if(not isOption(nextArg)){	// option and its parameter
	  const auto nowPara = nextArg;
	  founded_argument.push_back(*ArgumentMarshaler::NewWithOptAndPara(nowArg, nowPara));
	  ++i;
	} else			// option with no parameter
	  founded_argument.push_back(*ArgumentMarshaler::NewWithOpt(nowArg));
      } else {			// parameter with no option
	const auto nowPara = nowArg;
	founded_argument.push_back(*ArgumentMarshaler::NewWithPara(nowPara));
      }
    }
  }

#define ArgMarshaler Parser::ArgumentMarshaler
  ArgMarshaler::ArgumentMarshaler(bool hasopt_, std::string opt_, bool haspara_, std::string para_) : hasopt(hasopt_), opt(opt_), haspara(haspara_), para(para_) {}
  
  ArgMarshaler* ArgMarshaler::NewWithOpt(const std::string opt){
    return new ArgMarshaler(true,opt,false,std::string{});
  }
  ArgMarshaler* ArgMarshaler::NewWithPara(const std::string para){
    return new ArgMarshaler(false,std::string{},true,para);
  }
  ArgMarshaler* ArgMarshaler::NewWithOptAndPara(const std::string opt,const std::string para){
    return new ArgMarshaler(true, opt, true, para);
  }

  bool ArgMarshaler::HasOption() const{ return hasopt; }
  bool ArgMarshaler::HasParameter() const{ return haspara; }
  std::string ArgMarshaler::GetOption() const{ return opt; }
  std::string ArgMarshaler::GetParameter() const{ return para; }
}
