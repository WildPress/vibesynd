extern unsigned char __ctype[];
long FUN_0003a526(char *s){
 long r; char sign; int c;
 while(__ctype[(unsigned char)(*s+1)]&2) s++;
 sign=*s;
 if(sign=='+'||sign=='-') s++;
 r=0;
 while(c=(unsigned char)*s, __ctype[(unsigned char)(c+1)]&0x20){ r=r*10+c; s++; r-='0'; }
 if(sign=='-') r=-r;
 return r;
}
