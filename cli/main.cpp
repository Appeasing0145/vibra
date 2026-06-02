#include "./cli.h"

int main(int argc, char** argv) {
  vibra::CLI cli;
  return cli.Run(argc, argv);
}
