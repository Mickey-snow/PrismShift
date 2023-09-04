#ifndef ARGS_H
#define ARGS_H

#include<string>
#include<vector>

namespace args{
  class Parser{
  private:
    class ArgumentMarshaler{
    public:
      static ArgumentMarshaler* NewWithOpt(const std::string opt);
      static ArgumentMarshaler* NewWithPara(const std::string para);
      static ArgumentMarshaler* NewWithOptAndPara(const std::string opt, const std::string para);

      bool HasOption() const;
      bool HasParameter() const;
      std::string GetOption() const;
      std::string GetParameter() const;
    private:
      ArgumentMarshaler(bool,std::string,bool,std::string);
      bool hasopt,haspara;
      std::string opt,para;
    };
    
  public:
    Parser(int argc, char** argv){
      this->argc = argc;
      for(int i=0;i<argc;++i)
	this->argv.emplace_back(std::string(argv[i]));
      Parse();
    }

    Parser(const std::vector<std::string>& init_argv) : argv(init_argv) { Parse(); }

    const char* RawArg_at(const int&) const;
    int ArgCount(void) const;
    int Raw_ArgCount(void) const;

    ArgumentMarshaler Arg_At(const int&) const;

    std::vector<ArgumentMarshaler> Parsed(void) const{ return founded_argument; }
  private:
    int argc;
    std::vector<std::string> argv;

    void Parse(void);

    std::vector<ArgumentMarshaler> founded_argument;
  };
} // namespace args

#endif