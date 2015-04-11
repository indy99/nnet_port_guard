// Nanonet Library (c)2015 Ruhsen.k, nnet.server@gmail.com

typedef unsigned char u;
#define size_byte 32
#define size_S size_byte+1
struct vardh_{u m[1024],g[1024],e[1024],b[1024];int n,v,d,z;int S;};struct vardh_ vardh;void dh_key_byte_size(int x){vardh.S=x+1;}
void a(u*x,u*y,int o){vardh.d=0;for(vardh.v=vardh.S;vardh.v--;){vardh.d+=x[vardh.v]+y[vardh.v]*o;x[vardh.v]=vardh.d;vardh.d=vardh.d>>8;}}
void s(u*x){for(vardh.v=0;(vardh.v<vardh.S-1)&&(x[vardh.v]==vardh.m[vardh.v]);)vardh.v++;if(x[vardh.v]>=vardh.m[vardh.v])a(x,vardh.m,-1);}
void r(u*x){vardh.d=0;for(vardh.v=0;vardh.v<vardh.S;){vardh.d|=x[vardh.v];x[vardh.v++]=vardh.d/2;vardh.d=(vardh.d&1)<<8;}}
void M(u*x,u*y){u X[1024],Y[1024];bcopy(x,X,vardh.S);bcopy(y,Y,vardh.S);bzero(x,vardh.S);for(vardh.z=vardh.S*8;vardh.z--;){if(X[vardh.S-1]&1){a(x,Y,1);s(x);}
r(X);a(Y,Y,1);s(Y);}}
void h(char*x,u*y){bzero(y,vardh.S);for(vardh.n=0;x[vardh.n]>0;vardh.n++){for(vardh.z=4;vardh.z--;)a(y,y,1);x[vardh.n]|=32;y[vardh.S-1]|=x[vardh.n]-48-(x[vardh.n]>96)*39;}}
void p(u*x,u*o){char c1,c2;int say=0;for(vardh.n=0;!x[vardh.n];)vardh.n++;for(;vardh.n<vardh.S;vardh.n++){c1=48+x[vardh.n]/16+(x[vardh.n]>159)*7;c2=48+(x[vardh.n]&15)+7*((x[vardh.n]&15)>9);o[say]=c1;o[say+1]=c2;say+=2;}
o[say]=0;}
void hes(char*g1,char*e1,char*m1,u*o){h(g1,vardh.g);h(e1,vardh.e);h(m1,vardh.m);bzero(vardh.b,vardh.S);vardh.b[vardh.S-1]=1;for(vardh.n=vardh.S*8;vardh.n--;){if(vardh.e[vardh.S-1]&1)M(vardh.b,vardh.g);M(vardh.g,vardh.g);r(vardh.e);}
p(vardh.b,o);}
