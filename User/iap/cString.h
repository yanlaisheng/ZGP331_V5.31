#ifndef CSTRING_H_
#define CSTRING_H_

#ifndef CSTRING_C_//如果没有定义
#define CSTRING_Ex_ extern
#else
#define CSTRING_Ex_
#endif

#include <stdint.h>

void cStringFree(void);
void cStringRestore(void);
char *StrBetwString(char *Str,char *StrBegin,char *StrEnd);
int split(char *src,const char *separator,char **dest,int DestLen);
void HexToStr(char *pbSrc,char *pbDest,int nlen);
void StrToHex(char *pbSrc, char *pbDest, int nLen);
uint8_t net_dns_str_ip(const char* str, void *ip);
#endif

