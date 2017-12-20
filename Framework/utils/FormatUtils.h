/* 
 * File:   FormatUtils.h
 * Author: bve
 *
 * Created on 24 février 2015, 14:57
 */

#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include <stdint.h>
#include <string>

#define SIZEOF_TIME_ISO8601 26
#define SIZEOF_DURATION 10

class FormatUtils {
 public:
  // STRINGS
  /**
   * @brief convert string to uppercase string
   * @param [in] string stringToSet : string to set in uppercase
   * @return string in uppercase
   */
  static std::string StringToUpperCase(const std::string& stringToSet);

  // STRING TO FLOAT

  /**
   * @brief convert a string to an float to receive data from request
   * @scope utils
   * @param [in] string valueToConvert
   * @return float converted (float)
   */
  static float convertStringToFloat(const std::string& valueToConvert);

  /**
   * @brief convert a float to a string to concat request easily
   * @scope utils
   * @param [in] float valueToConvert
   * @param [in] int p_precision optional precision
   * @return string converted (string)
   */
  static std::string convertFloatToString(const float& valueToConvert, const int& p_precision = -1);

  // STRING TO INT
  /**
   * @brief convert a string to an uint16_t to receive data from request
   * @scope utils
   * @param [in] string valueToConvert
   * @return int converted (int)
   */
  static int convertStringToInt(const std::string& valueToConvert);
  /**
   * @brief convert a string to an unsigned int32 to receive data from request
   * @scope utils
   * @param [in] string valueToConvert
   * @return int converted (uint32_t)
   */
  static uint32_t convertStringToUint32(const std::string& valueToConvert);
  /**
   * @brief convert a string to an unsigned int64 to receive data from request
   * @scope utils
   * @param [in] string valueToConvert
   * @return long converted (uint64_t)
   */
  static uint64_t convertStringToUint64(const std::string& valueToConvert);

  // INT TO STRING
  /**
   * @brief convert an uint16 to a string to concat request easily
   * hardly needed for modbus.sdb to ensure uint16 convertion before entering DB
   * @scope utils
   * @param [in] int valueToConvert
   * @return string converted (string)
   */
  static std::string convertUint16ToString(const uint16_t& valueToConvert);
  /**
   * @brief convert an int to a string to concat request easily
   * @scope utils
   * @param [in] int valueToConvert
   * @return string converted (string)
   */
  static std::string convertUint32ToString(const uint32_t& valueToConvert);
  /**
   * @brief convert an int to a string to concat request easily
   * @scope utils
   * @param [in] uint64_t valueToConvert
   * @return string converted (string)
   */
  static std::string convertUint64ToString(const uint64_t& valueToConvert);
  /**
  * @brief convert an int to a string to concat request easily
  * @scope utils
  * @param [in] uint64_t valueToConvert
  * @return string converted (string)
  */
  static std::string convertBoolToString(const bool& valueToConvert);
  /**
   * @brief fill a given uint table with a string
   * @param [in,out] uint16_t tableToFill[] : register table to fill in
   * @param [in] int tableSize : table register size
   * @param [in] string strToParse: string to put within the table
   */
  static void fillUintTableWithString(uint16_t tableToFill[], int tableSize, const std::string& strToParse);
  /**
   * @brief fill a given string with a uint table
   * @param [in] uint16_t tableToFill[] : register table to fill in
   * @param [in] int tableSize : table register size
   * @return string strToParse: string to put within the table
   */
  static std::string fillStringWithUintTable(const uint16_t tableToParse[], int tableSize);

  // HEXA STRING

  /**
   * @brief convert hexa string to integer value
   * @param [in] string hexaString : string to convert
   * @return value of the hexa string as int
   */
  static int HexaStringToInt(const std::string& hexaString);
  /**
   * @brief convert integer value to hexa string
   * @param [in] int hexaIntegerValue : in value to convert
   * @return hexa string of the integer value
   */
  static std::string IntToHexaString(const int& hexaIntegerValue);
  /**
   * @brief get hexa string
   * @param [in] string must_be_hex : string to test
   * @return if the string is full hexadecimal value or not
   */
  static bool IsAllHex(const std::string& must_be_hex);
  static std::string word_to_string(const uint16_t& n);

  // TIMESTAMP
  /**
   * Format time to string with the iso8601 format like "2014-03-23T01:09:07+01:00"
   *
   * @param ret
   * @param time
   * @param light
   * @param zulu
   */
  static std::string FormatTimeTo_ISO8601(const time_t& time, bool light = false, bool zulu = false);
  /**
   * Format a number of seconds to "hh:mm:ss"
   *
   * @param ret
   * @param durationSeconds
   */
  static std::string FormatDurationSecondsTo_TimeString(const uint32_t& durationSeconds);


  /**
   * @brief Replace all occurrences of the search string with the replacement string
   * @param [in] str : the string being searched and replaced on
   * @param [in] from : the string being searched for
   * @param [in] to : the replacement string
   */
  static void replaceAll(std::string& str, const std::string& from, const std::string& to);
};

#endif /* FORMATUTILS_H */
