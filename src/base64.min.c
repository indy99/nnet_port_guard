// Nanonet Library (c)2015 Ruhsen.k, nnet.server@gmail.com

static const char table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";char isbase64(char c){return c&&strchr(table,c)!=NULL;}
char value(char c1){const char*p=strchr(table,c1);if(p){return p-table;}else{return 0;}}
char*unbase64(unsigned char*src,unsigned int srclen,unsigned int*len){*len=0;if(*src==0)return NULL;unsigned int olen=srclen/4*3;char*dest=(char*)malloc(olen+1);if(dest==NULL)return NULL;memset(dest,0,olen+1);char*p=dest;do
{char a=value(src[0]);char b=value(src[1]);char c=value(src[2]);char d=value(src[3]);*p++=(a<<2)|(b>>4);*p++=(b<<4)|(c>>2);*p++=(c<<6)|d;if(!isbase64(src[1]))
{p-=2;break;}
else if(!isbase64(src[2]))
{p-=2;break;}
else if(!isbase64(src[3]))
{p--;break;}
src+=4;while(*src&&(*src==13||*src==10))src++;}
while(srclen-=4);*p=0;*len=p-dest;return dest;}
char*base64(unsigned char*bytes_to_encode,unsigned int in_len,unsigned int*len){int i=0;int j;unsigned char char_array_3[3];unsigned char char_array_4[4];unsigned int ret=0;*len=0;char*buff=(char*)malloc((4*((in_len+2)/3))+1);if(buff==NULL)return NULL;while(in_len--){char_array_3[i++]=*(bytes_to_encode++);if(i==3){char_array_4[0]=(char_array_3[0]&0xfc)>>2;char_array_4[1]=((char_array_3[0]&0x03)<<4)+((char_array_3[1]&0xf0)>>4);char_array_4[2]=((char_array_3[1]&0x0f)<<2)+((char_array_3[2]&0xc0)>>6);char_array_4[3]=char_array_3[2]&0x3f;for(i=0;(i<4);i++)
buff[ret++]=table[char_array_4[i]];i=0;}}
if(i){for(j=i;j<3;j++)
char_array_3[j]='\0';char_array_4[0]=(char_array_3[0]&0xfc)>>2;char_array_4[1]=((char_array_3[0]&0x03)<<4)+((char_array_3[1]&0xf0)>>4);char_array_4[2]=((char_array_3[1]&0x0f)<<2)+((char_array_3[2]&0xc0)>>6);char_array_4[3]=char_array_3[2]&0x3f;for(j=0;(j<i+1);j++)
buff[ret++]=table[char_array_4[j]];while((i++<3))
buff[ret++]='=';}*len=ret;buff[ret]=0;return buff;}