/* 
 * File:   testAes.cpp
 * Author: romain
 *
 * Created on 16 avril 2014, 16:28
 */

#include "AesAlgo.h"
#include <cstdio>
#include <openssl/conf.h>
#include <cstring>
#include <string>

void test(string initialText) {
  string encryptedText;
  string decryptedText;

  AesAlgo algo;
  printf("InitialText is : \n");
  printf("%s\n", initialText.c_str());
  /* Encrypt the plaintext */
  encryptedText = algo.encrypt(initialText);

  printf("Evcrypted text is:\n");
  printf("%s\n", encryptedText.c_str());
  /* Decrypt the ciphertext */
  decryptedText = algo.decrypt(encryptedText);

  /* Show the decrypted text */
  printf("Decrypted text is:\n");
  printf("%s\n", decryptedText.c_str());
  if (decryptedText == initialText) {
    printf("====> TEST OK\n");
  } else {
    printf("<==== TEST KO\n");
  }
}

int main(int arc, char *argv[]) {

  string test1 = "The quick brown fox jumps over the lazy d11";
  string test2 = "Azerty1234";
  string test3 =
      "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit...";
  string test4 = "1234567891011121314151617181920";
  /* Message to be encrypted */
  puts("################## TEST AES 1 ########################");
  test(test1);
  puts("################## TEST AES 2 ########################");
  test(test2);
  puts("################## TEST AES 3 ########################");
  test(test3);
  puts("################## TEST AES 4 ########################");
  test(test4);
  return 0;
}
