/**
 * SHA256.cpp
 *
 *  Created on: 28 août 2012
 *      Author: bmo
 * 
 * @brief encryptation class using Sha2.c library
 *   specific use of SHA256 encryptation
 * 
 */

#include <cstdlib>
#include <iostream>
#include "SHA256.h"

#define BUFLEN 16384
using std::cout;
using std::endl;

SHA256::SHA256() {
  m_ctxt = (SHA256_CTX*) malloc(sizeof(SHA256_CTX));
  SHA256_Init(m_ctxt);
}

SHA256::~SHA256() {
  if (m_ctxt) {
    char buf[BUFLEN];
    SHA256_End(m_ctxt, buf);
    free(m_ctxt);
    m_ctxt = 0;
  }
}
/**
 * @brief redefinition of << operator
 * @param [in] string s : set SHA256 to this string
 * @return current sha256 object (SHA256&)
 */
SHA256& SHA256::operator<<(string s) {
  SHA256_Update(m_ctxt, (unsigned char*) s.c_str(), s.length());
  return *this;
}
/**
 * @brief encript the string setted
 * @return encrypted string (string)
 */
string SHA256::GetHash() {
  char buf[BUFLEN];
  SHA256_End(m_ctxt, buf);
  string res = buf;
  return res;
}
