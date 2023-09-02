#include<gtest/gtest.h>

#include<args.hpp>

TEST(argParser, init){
 char* program_name = "argParser-test";
 char* argv[] = { program_name };
 const int argc = 1;

  auto parser = args::Parser(argc, argv);

  EXPECT_EQ(parser.ArgCount(), argc);
  EXPECT_STREQ(parser.RawArg_at(0), program_name);
}

TEST(argParser, parse){
  const std::vector<std::string> argv{
    "argParser-test",
    "-a",
    "-123", "para1", 
    "--include", "para3"};
  const int argc = argv.size();

  auto parser = args::Parser(argv);

  EXPECT_EQ(parser.ArgCount(), 4);
}

TEST(argMarshaler, programname){
  const std::vector<std::string> argv{
    "argParser-test",
    "para"};

  auto parser = args::Parser(argv);

  auto arg_mar = parser.Arg_At(0);
  EXPECT_TRUE(arg_mar.HasOption());
  EXPECT_TRUE(arg_mar.HasParameter());
  EXPECT_EQ(arg_mar.GetOption(), std::string{"--program-name"});
  EXPECT_EQ(arg_mar.GetParameter(), argv[0]);
}
