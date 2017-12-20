/* 
 * File:   AesAlgo.cpp
 * Author: romain
 * 
 * Created on 16 avril 2014, 16:20
 */

#include "AesAlgo.h"
#include <string>
#include "stdint.h"
#include <iostream>
#include <sstream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <vector>

#include "TraceDebug.h"
#include "libbb/libbb.h"
#include "Directory.h"
using namespace std;

TRACEDEBUG_REGISTER("[CRYPT][AES]");

AesAlgo::AesAlgo() {
  /* A 256 bit key */
  key = (unsigned char *) "53464313787615348743131848648511";
  /* A 128 bit IV */
  iv = (unsigned char *) "13548643134845313";
}

AesAlgo::~AesAlgo() {

}

string ucharToHexaString(unsigned char* myInput, int bufferSize) {
  LOG_ENTER("%s", myInput);
  char converted[bufferSize * 2 + 1];
  int i;
  for (i = 0; i < bufferSize; i++) {
    snprintf(&converted[i * 2], bufferSize * 2 + 1, "%02X", myInput[i]);
  }
  LOG_EXIT("%s", converted);
  return converted;
}

string hexaToAsciiString(const string& hexaString) {
  int length = hexaString.length();
  std::string newString = "";
  for (int i = 0; i < length; i += 2) {
    string byte = hexaString.substr(i, 2);
    char chr = (char) (int) strtol(byte.c_str(), NULL, 16);
    newString.push_back(chr);
  }
  LOG_EXIT("%s", newString.c_str());
  return newString;

}

string AesAlgo::decrypt(const string& textToDecrypt) const {
  LOG_ENTER();

#if 0
  /* Initialise the library */
  OpenSSL_add_all_algorithms();
  // MOVE IN MAIN
  OPENSSL_config(NULL);
#endif

  EVP_CIPHER_CTX *ctx;

  string retStr = "";
  string toDecrypt = hexaToAsciiString(textToDecrypt);
  int len;
  unsigned char plaintext[128];
  /* Create and initialise the context */
  ctx = EVP_CIPHER_CTX_new();
  if (!(ctx)) {
    LOG_ERROR("Error while initializing the context\ns");
    return retStr;
  }
  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
    LOG_ERROR("Error while initializing decrypting");
    return retStr;
  }
  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */

  if (1
      != EVP_DecryptUpdate(ctx, plaintext, &len, (unsigned char *) toDecrypt.c_str(),
                           toDecrypt.size())) {
    LOG_ERROR("Error while decrypting");
    return retStr;
  }
  LOG_DEBUG("plaintext : %s", plaintext);
  /* Finalise the decryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
    LOG_ERROR("Error while finalizing decryption");
    return retStr;
  }
  retStr = (char*) plaintext;
  LOG_DEBUG("converted as string : %s", retStr.c_str());
  string sizeToken = "__";
  size_t found = retStr.find(sizeToken);
  if (found != string::npos) {
    int mySize = atoi(retStr.substr(0, found).c_str());
    string mystring = retStr.substr(found + sizeToken.size(), mySize);
    LOG_DEBUG("string found with size %d :: %s ", mySize, mystring.c_str());
    retStr = mystring;
  }

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

#if 0
  /* Clean up */
  EVP_cleanup();
#endif
  LOG_EXIT("%s", retStr.c_str());
  return retStr;
}

string AesAlgo::encrypt(const string& textToCrypt) const {
  LOG_ENTER("%s", textToCrypt.c_str());

#if 0
  /* Initialise the library */
  OpenSSL_add_all_algorithms();
  // MOVE IN MAIN
  OPENSSL_config(NULL);
#endif

  EVP_CIPHER_CTX *ctx;
  string retStr = "";

  int len;
  unsigned char ciphertext[128];
  int ciphertext_len;
  stringstream ss;
  ss << textToCrypt.length() << "__" << textToCrypt;
  LOG_DEBUG("%s", ss.str().c_str());
  const string tmp = ss.str();
  unsigned char * plaintext2 = (unsigned char *) tmp.c_str();

  /* Create and initialise the context */
  ctx = EVP_CIPHER_CTX_new();
  if (!(ctx)) {
    LOG_ERROR("Error while initializing context");
    return retStr;
  }

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
    LOG_ERROR("Error while initializing encrypting");
    return retStr;
  }

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext2, ss.str().size())) {
    LOG_ERROR("Error while encrypting");
    return retStr;
  }
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
    LOG_ERROR("Error while finalizing encryption");
    return retStr;
  }
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  retStr = ucharToHexaString(ciphertext, ciphertext_len);
  LOG_EXIT("%s", retStr.c_str());

#if 0
  /* Clean up */
  EVP_cleanup();
#endif

  return retStr;
}
