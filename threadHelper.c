#include "threadHelper.h"

int isAlphaNumeric(const char* check){
  char* AlphaNum = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";
  for (unsigned int i = 0; i < strlen(check); i++){
    if (strchr(AlphaNum, check[i]) == NULL){
      return false;
    }
  }
  return true;
}

int isStringNumber(const char* c){
  for (unsigned int i = 0; i < strlen(c); i++){
    if (isdigit(c[i]) == 0){
      return 0;
    }
  }
  return 1;
}

int findIndexString (const char* checking, const char* check){
  bool found = false;
  for (unsigned int i = 0; i < strlen(checking); i++){
    if (checking[i] == check[0]){
      if (strlen(checking + i) >= strlen(check)){
        found = true;
        for (unsigned int j = 0; j < strlen(check); j++){
          if (checking[i+j] != check[j]){
            found = false;
            break;
          }
        }
        if (found == true){
          return i+strlen(check);
        }
      }
    }
  }
  return 0;
}

int continuable(int m, int n, const char* checking, const char* check){
  for(int i = 0; i < m; i++){
    if (strcmp((checking+(i*n)),check) == 0){
      return 0;
    }
  }
  return 1;
}
