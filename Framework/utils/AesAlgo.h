/* 
 * File:   AesAlgo.h
 * Author: romain
 *
 * Created on 16 avril 2014, 16:20
 */

#ifndef AESALGO_H
#define AESALGO_H

#include <string>

using namespace std;

class AesAlgo {
 public:
  AesAlgo();
  virtual ~AesAlgo();

  string encrypt(const string& textToCrypt) const;
  string decrypt(const string& textToDecrypt) const;
 private:
  unsigned char *key;
  unsigned char *iv;
};

#endif /* AESALGO_H */

