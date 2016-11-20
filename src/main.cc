#include <iostream>
#include "optionparser.h"

#include "ast.hh"
#include "parser.hh"

struct Arg : public option::Arg {
  static void printError(const char* msg1, const option::Option& opt, const char* msg2) {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Required(const option::Option& option, bool msg) {
    if (option.arg != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires an argument\n");
    return option::ARG_ILLEGAL;
  }
};

enum OptionIndex { UNKNOWN, FILE_NAME, HELP };
const option::Descriptor usage[] = {
  { UNKNOWN, 0, "", "", option::Arg::None, "USAGE: pbc files [options]"
                                            "\n\nOptions:"},
  { HELP, 0, "h", "help", option::Arg::None, "  --help: Display this message" },
  { 0, 0, 0, 0, 0, 0 }
};

int main(int argc, char *argv[]) {
  argc -= (argc > 0); argv += (argc>0);
  option::Stats stats(usage, argc, argv);
  option::Option *options = new option::Option[stats.options_max];
  option::Option *buffer = new option::Option[stats.buffer_max];
  option::Parser parse(usage, argc, argv, options, buffer);

  if(parse.error()) {
    return 1;
  }

  if(options[HELP] || argc == 0) {
    option::printUsage(std::cout, usage);
    return 0;
  } 

  for(option::Option* opt = options[UNKNOWN]; opt; opt = opt->next()) {
    std::cout << "Unknown option: " << opt->name << std::endl;
    return 1;
  }

  if(parse.nonOptionsCount() > 1) {
    std::cout << "Currently only single file parsing is supported" << std::endl;
    return 1;
  }

  for(int i = 0; i < parse.nonOptionsCount(); ++i) {
    std::string fname;
    std::cout << parse.nonOption(i) << std::endl;
  }

  return 0;
}
