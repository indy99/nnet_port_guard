// Nanonet Library (c)2015 Ruhsen.k, nnet.server@gmail.com

typedef uint32_t crypt_long;
#define CRYPT_MX (((z>>5^y<<2)+(y>>3^z<<4))^((sum^y)+(k[(p&3)^e]^z)))
#define CRYPT_DELTA 0x9e3779b9
void crypt_long_encrypt(crypt_long*v,crypt_long len,crypt_long*k){crypt_long n=len-1;crypt_long z=v[n],y=v[0],p,q=6+52/(n+1),sum=0,e;if(n<1){return;}
while(0<q--){sum+=CRYPT_DELTA;e=sum>>2&3;for(p=0;p<n;p++){y=v[p+1];z=v[p]+=CRYPT_MX;}
y=v[0];z=v[n]+=CRYPT_MX;}}
void crypt_long_decrypt(crypt_long*v,crypt_long len,crypt_long*k){crypt_long n=len-1;crypt_long z=v[n],y=v[0],p,q=6+52/(n+1),sum=q*CRYPT_DELTA,e;if(n<1){return;}
while(sum!=0){e=sum>>2&3;for(p=n;p>0;p--){z=v[p-1];y=v[p]-=CRYPT_MX;}
z=v[n];y=v[0]-=CRYPT_MX;sum-=CRYPT_DELTA;}}
static crypt_long*crypt_to_long_array(const unsigned char*data,crypt_long len,int include_length,crypt_long*ret_len){crypt_long i,n,*result;n=len>>2;n=(((len&3)==0)?n:n+1);if(include_length){result=(crypt_long*)malloc((n+1)<<2);result[n]=len;*ret_len=n+1;}else{result=(crypt_long*)malloc(n<<2);*ret_len=n;}
memset(result,0,n<<2);for(i=0;i<len;i++){result[i>>2]|=(crypt_long)data[i]<<((i&3)<<3);}
return result;}
static unsigned char*crypt_to_byte_array(crypt_long*data,crypt_long len,int include_length,crypt_long*ret_len){crypt_long i,n,m;unsigned char*result;n=len<<2;if(include_length){m=data[len-1];if((m<n-7)||(m>n-4))return NULL;n=m;}
result=(unsigned char*)malloc(n+1);for(i=0;i<n;i++){result[i]=(unsigned char)((data[i>>2]>>((i&3)<<3))&0xff);}
result[n]='\0';*ret_len=n;return result;}
unsigned char*crypt_encrypt(const unsigned char*data,crypt_long len,unsigned char*key,crypt_long*ret_len)
{unsigned char*result;crypt_long*v,*k,v_len,k_len;v=crypt_to_long_array(data,len,1,&v_len);k=crypt_to_long_array(key,16,0,&k_len);crypt_long_encrypt(v,v_len,k);result=crypt_to_byte_array(v,v_len,0,ret_len);free(v);free(k);return result;}
unsigned char*crypt_decrypt(const unsigned char*data,crypt_long len,unsigned char*key,crypt_long*ret_len)
{unsigned char*result;crypt_long*v,*k,v_len,k_len;v=crypt_to_long_array(data,len,0,&v_len);k=crypt_to_long_array(key,16,0,&k_len);crypt_long_decrypt(v,v_len,k);result=crypt_to_byte_array(v,v_len,1,ret_len);free(v);free(k);return result;}
