/********************************************************************
Copyright 2010-2017 K.C. Wang
********************************************************************/

#define strlen kstrlen
#define strcmp kstrcmp
#define strcpy kstrcpy
#define memcpy kmemcpy
#define memset kmemset
#define bcopy  kbcopy
#define bzero  kbzero

int kbcopy(const void *src, void *dest, unsigned int n) {
   const char *s = (const char *)src;
   char *d = (char *)dest;

   if (s <= d){
      for (; n>0; --n)
         d[n-1] = s[n-1];
   }
   else{
      for (; n>0; --n)
         *d++ = *s++;
   }
}


int kstrlen(char *s)
{
  int i = 0;
  while(*s){
    i++; s++;
  }
  return i;
}

int kstrcmp(char *s1, char *s2)
{
  while((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if ((*s1==0) && (*s2==0))
     return 0;
  return(*s1 - *s2);

}

int kstrcpy(char *dest, char *src)
{
  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}
char *kmemset(void *dest, int c, unsigned int n) {
	char *d = (char *)dest;
	for (; n>0; --n)
		*d++ = (char)c;
	return dest;
}

char *kmemcpy(void *dest, const void *src, unsigned int n) {
	kbcopy(src, dest, n);
	return dest;
}
int kbzero(void *dest, unsigned int n) {
   kmemset(dest, 0, n);
}

