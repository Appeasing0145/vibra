#ifndef CLI_CLI_H_
#define CLI_CLI_H_

#include <string>

#include "../include/vibra.h"

namespace vibra {

class CLI {
 public:
  int Run(int argc, char** argv);

 private:
  Fingerprint* getFingerprintFromMusicFile(const std::string& music_file);
  Fingerprint* getFingerprintFromStdin(int chunk_seconds, int sample_rate,
                                       int channels, int bits_per_sample,
                                       bool is_signed);
};

}  // namespace vibra

#endif  // CLI_CLI_H_
