#ifndef CLI_COMMUNICATION_TIMEZONES_H_
#define CLI_COMMUNICATION_TIMEZONES_H_

#include <array>

namespace vibra {

static constexpr std::array<const char*, 60> kEuropeTimezones = {
    {"Europe/Amsterdam",  "Europe/Andorra",    "Europe/Astrakhan",
     "Europe/Athens",     "Europe/Belgrade",   "Europe/Berlin",
     "Europe/Bratislava", "Europe/Brussels",   "Europe/Bucharest",
     "Europe/Budapest",   "Europe/Busingen",   "Europe/Chisinau",
     "Europe/Copenhagen", "Europe/Dublin",     "Europe/Gibraltar",
     "Europe/Guernsey",   "Europe/Helsinki",   "Europe/Isle_of_Man",
     "Europe/Istanbul",   "Europe/Jersey",     "Europe/Kaliningrad",
     "Europe/Kiev",       "Europe/Kirov",      "Europe/Lisbon",
     "Europe/Ljubljana",  "Europe/London",     "Europe/Luxembourg",
     "Europe/Madrid",     "Europe/Malta",      "Europe/Mariehamn",
     "Europe/Minsk",      "Europe/Monaco",     "Europe/Moscow",
     "Europe/Oslo",       "Europe/Paris",      "Europe/Podgorica",
     "Europe/Prague",     "Europe/Riga",       "Europe/Rome",
     "Europe/Samara",     "Europe/San_Marino", "Europe/Sarajevo",
     "Europe/Saratov",    "Europe/Simferopol", "Europe/Skopje",
     "Europe/Sofia",      "Europe/Stockholm",  "Europe/Tallinn",
     "Europe/Tirane",     "Europe/Ulyanovsk",  "Europe/Uzhgorod",
     "Europe/Vaduz",      "Europe/Vatican",    "Europe/Vienna",
     "Europe/Vilnius",    "Europe/Volgograd",  "Europe/Warsaw",
     "Europe/Zagreb",     "Europe/Zaporozhye", "Europe/Zurich"}};

constexpr unsigned int kEuropeTimezonesSize = kEuropeTimezones.size();

}  // namespace vibra

#endif  // CLI_COMMUNICATION_TIMEZONES_H_
