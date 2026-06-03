#ifndef CLI_COMMUNICATION_SHAZAM_H_
#define CLI_COMMUNICATION_SHAZAM_H_

#include <string>

// forward declaration
struct Fingerprint;
//

namespace vibra {

class Shazam {
  static const char* const kHost;

 public:
  static std::string Recognize(const Fingerprint* fingerprint);

 private:
  static std::string getShazamHost();
  static std::string getUserAgent();
  static std::string getRequestContent(const std::string& uri,
                                       unsigned int sample_ms);
  static std::string getTimezone();
};

}  // namespace vibra

#endif  // CLI_COMMUNICATION_SHAZAM_H_
