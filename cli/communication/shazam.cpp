#include "communication/shazam.h"

#include <curl/curl.h>

#include <algorithm>
#include <random>
#include <sstream>

#include "../../include/vibra.h"
#include "communication/timezones.h"
#include "communication/user_agents.h"
#include "utils/uuid4.h"

namespace vibra {

const char* const Shazam::kHost =
    "https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/";

std::size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                          void* userp) {
  auto* buffer = reinterpret_cast<std::string*>(userp);
  std::size_t realsize = size * nmemb;
  buffer->append(reinterpret_cast<char*>(contents), realsize);
  return realsize;
}

std::string Shazam::Recognize(const Fingerprint* fingerprint) {
  auto content =
      getRequestContent(vibra_get_uri_from_fingerprint(fingerprint),
                        vibra_get_sample_ms_from_fingerprint(fingerprint));
  auto user_agent = getUserAgent();
  std::string url = getShazamHost();

  CURL* curl = curl_easy_init();
  std::string read_buffer;

  if (curl) {
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br");
    headers = curl_slist_append(headers, "Accept: */*");
    headers = curl_slist_append(headers, "Connection: keep-alive");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Content-Language: en_US");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);

    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate, br");
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << '\n';
    }

    std::int64_t http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
      std::cerr << "HTTP code: " << http_code << '\n';
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  }
  return read_buffer;
}

std::string Shazam::getShazamHost() {
  std::string host =
      std::string(kHost) + uuid4::Generate() + "/" + uuid4::Generate();
  host +=
      "?sync=true&"
      "webv3=true&"
      "sampling=true&"
      "connected=&"
      "shazamapiversion=v3&"
      "sharehub=true&"
      "video=v3";
  return host;
}

std::string Shazam::getRequestContent(const std::string& uri,
                                      unsigned int sample_ms) {
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<double> dis_float(0.0, 1.0);

  auto timezone = getTimezone();
  double fuzz = dis_float(gen) * 15.3 - 7.65;

  std::stringstream json_buf;
  json_buf << "{";
  json_buf << "\"geolocation\":{";
  json_buf << "\"altitude\":" << dis_float(gen) * 400 + 100 + fuzz << ",";
  json_buf << "\"latitude\":" << dis_float(gen) * 180 - 90 + fuzz << ",";
  json_buf << "\"longitude\":" << dis_float(gen) * 360 - 180 + fuzz;
  json_buf << "},";
  json_buf << "\"signature\":{";
  json_buf << "\"samplems\":" << sample_ms << ",";
  json_buf << "\"timestamp\":" << time(nullptr) * 1000ULL << ",";
  json_buf << R"("uri":")" << uri << "\"";
  json_buf << "},";
  json_buf << "\"timestamp\":" << time(nullptr) * 1000ULL << ",";
  json_buf << "\"timezone\":"
           << "\"" << timezone << "\"";
  json_buf << "}";
  std::string content = json_buf.str();
  return content;
}

std::string Shazam::getUserAgent() {
  std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<> dis_useragent(0, kUserAgentsSize - 1);
  return kUserAgents[dis_useragent(gen)];
}

std::string Shazam::getTimezone() {
  std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<> dis_timezone(0, kEuropeTimezonesSize - 1);
  return kEuropeTimezones[dis_timezone(gen)];
}

}  // namespace vibra
