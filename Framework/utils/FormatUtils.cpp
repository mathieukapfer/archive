/* 
 * File:   FormatUtils.h
 * Author: bve
 *
 * Created on 24 février 2015, 14:57
 */

#include "FormatUtils.h"
#include <sstream>
#include <stdlib.h>
#include <iomanip> // setprecision

#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[UTILS][FORMAT]")


std::string FormatUtils::FormatTimeTo_ISO8601(const time_t& time, bool light, bool zulu) {
  LOG_ENTER();
  char date[30];
  LOG_DEBUG("%s", ctime_r(&time, date));
  std::string str;

  if (time == 0) {
    str = "0";
  } else {
    char ret[SIZEOF_TIME_ISO8601];

    if (light) {
      //Format date to a light "ISO8601" : "2013-06-01 11:45"
      strftime(ret, SIZEOF_TIME_ISO8601, "%Y-%m-%d %H:%M", localtime(&time));
    } else if (zulu) {
      // If the time is in UTC, add a Z directly after the time without a space. Z is the zone designator for the zero UTC offset. "09:30 UTC" is therefore represented as "09:30Z" or "0930Z". "14:45:15 UTC" would be "14:45:15Z" or "144515Z".
      // UTC time is also known as Zulu time, since Zulu is the NATO phonetic alphabet word for Z
      //Format date to ISO8601 UTC+Z : "2013-06-01T11:45:30Z"
      strftime(ret, SIZEOF_TIME_ISO8601, "%FT%TZ:", localtime(&time));

    } else {
      //Format date to ISO8601 UTC+Offset : "2013-06-01T11:45:30+0200"
      strftime(ret, SIZEOF_TIME_ISO8601, "%FT%T%z:", localtime(&time));

      //strftime format the timezone like "+0100" -> we want : "+01:00"
      //insert ':' between hours and min
      char ret_22 = ret[22];
      char ret_23 = ret[23];
      ret[22] = ':';
      ret[23] = ret_22;
      ret[24] = ret_23;
    }
    str = ret;
  }

  LOG_DEBUG("ret = %s", str.c_str());
  return str;
}

std::string FormatUtils::FormatDurationSecondsTo_TimeString(const uint32_t& durationSeconds) {
  LOG_ENTER();

  char ret[SIZEOF_DURATION];
  uint32_t hours = durationSeconds / 3600;
  uint32_t tmpSeconds = durationSeconds % 3600;
  uint32_t minutes = tmpSeconds / 60;
  uint32_t seconds = tmpSeconds % 60;

  snprintf(ret, SIZEOF_DURATION, "%02d:%02d:%02d", hours, minutes, seconds);

  std::string str = ret;

  LOG_DEBUG("returnValue = %s", str.c_str());
  return str;
}

std::string FormatUtils::convertFloatToString(const float& valueToConvert, const int& p_precision) {
  std::stringstream strFloat;
  if (p_precision >= 0) {
    strFloat << std::fixed << std::setprecision(p_precision);
  }

  strFloat << valueToConvert;
  return strFloat.str();
}

std::string FormatUtils::convertUint16ToString(const uint16_t& valueToConvert) {
  std::stringstream strInt;
  strInt << valueToConvert;
  return strInt.str();
}

std::string FormatUtils::convertUint32ToString(const uint32_t& valueToConvert) {
  std::stringstream strInt;
  strInt << valueToConvert;
  return strInt.str();
}

std::string FormatUtils::convertUint64ToString(const uint64_t& valueToConvert) {
  std::stringstream strInt;
  strInt << valueToConvert;

  return strInt.str();
}

std::string FormatUtils::convertBoolToString(const bool& valueToConvert) {
  std::stringstream strInt;
  strInt << valueToConvert;

  return strInt.str();
}

float FormatUtils::convertStringToFloat(const std::string& valueToConvert) {
  LOG_ENTER();
  std::stringstream floatStr(valueToConvert);
  int newValue = 0;
  floatStr >> newValue;

  return newValue;
}

int FormatUtils::convertStringToInt(const std::string& valueToConvert) {
  LOG_ENTER();
  std::stringstream intStr(valueToConvert);
  int newValue = 0;
  intStr >> newValue;

  return newValue;
}

uint32_t FormatUtils::convertStringToUint32(const std::string& valueToConvert) {
  LOG_ENTER("%s", valueToConvert.c_str());
  std::stringstream intStr(valueToConvert);
  uint32_t newValue = 0;
  intStr >> newValue;
  LOG_EXIT("%lld", newValue);
  return newValue;
}

uint64_t FormatUtils::convertStringToUint64(const std::string& valueToConvert) {
  std::stringstream intStr(valueToConvert);
  uint64_t newValue = 0;
  intStr >> newValue;

  return newValue;
}

int FormatUtils::HexaStringToInt(const std::string& hexaString) {
  int retValue = 0;
  std::stringstream stream;
  stream << std::hex << hexaString;
  stream >> retValue;
  return retValue;
}

std::string FormatUtils::IntToHexaString(const int& hexaIntegerValue) {
  std::stringstream stream;
  stream << std::hex << std::uppercase << hexaIntegerValue;
  return stream.str();
}

bool FormatUtils::IsAllHex(const std::string& must_be_hex) {
  // make a char container
  char cstr[must_be_hex.length() + 1];
  // copy the const char* into the char*
  snprintf(cstr, must_be_hex.length() + 1, "%s", must_be_hex.c_str());
  // return false if the string contain something else not in this list
  char* ptrsave = NULL;
  char* result = "";
  result = strtok_r(cstr, "0123456789ABCDEFabcdef", &ptrsave);
  bool ret = (result == NULL);
  return ret;
}


std::string FormatUtils::StringToUpperCase(const std::string& stringToSet) {
  std::stringstream stream;
  stream << std::uppercase << stringToSet;
  return stream.str();
}

std::string FormatUtils::word_to_string(const uint16_t& n) {
  int wordSize = 4;
  char buf[wordSize + 1];
  snprintf(buf, sizeof(buf), "%04X", n);
  return buf;
}


void FormatUtils::fillUintTableWithString(uint16_t tableToFill[], int tableSize, const std::string& strToParse) {
  LOG_ENTER("%s", strToParse.c_str());
  for (int i = 0; i < tableSize; i++) {
    unsigned int lsb = i * 2;
    unsigned int msb = lsb + 1;
    //check the upper 8 bits
    if (msb < strToParse.size()) {
      tableToFill[i] = (int) (strToParse[msb]) << 8;
    } else {
      tableToFill[i] = 0;
    }

    //check the lower 8 bits
    if (lsb < strToParse.size()) {
      tableToFill[i] += (int) (strToParse[lsb]);
    } else {
      tableToFill[i] += 0;
    }
  }
}


std::string FormatUtils::fillStringWithUintTable(const uint16_t tableToParse[], int tableSize) {
  LOG_ENTER_("%s", tableToParse);
  // +1 for the \0
  int realStrSize = (tableSize * 2) + 1;
  char buf[realStrSize];
  snprintf(buf, realStrSize, "%s", (char *) tableToParse);
  LOG_EXIT("%s", buf);
  return std::string(buf);
}

void FormatUtils::replaceAll(std::string& str, const std::string& from, const std::string& to) {
  if (from.empty()) {
    return;
  }
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();  // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}
