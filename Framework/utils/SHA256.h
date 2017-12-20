/*
 * SHA256.h
 *
 *  Created on: 28 août 2012
 *      Author: bmo
 */

#ifndef SHA256_H_
#define SHA256_H_

#include <string>

extern "C" {
#include "sha2.h"
}

using std::string;

class SHA256 {
 public:
  SHA256();
  virtual ~SHA256();
  SHA256& operator<<(string s);
  string GetHash();
 private:
  SHA256_CTX *m_ctxt;
};

#endif /* SHA256_H_ */
