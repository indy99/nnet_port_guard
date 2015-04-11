//
//  Nanonet library
//  (c) by Ruhsen.k, 2015
//

#include "dh.min.c"
#include "crypt_xx.min.c"
#include "base64.min.c"

#define PAKETBASRAW     '1'
#define PAKETSON        "\x0d\x0a\x0a\x0d"
#define size_paketson   4
#define SERVERNODEIP    "000000000000"

#define COM_LOG     "LOG"
#define COM_PUT     "PUT"
#define COM_PIN     "PIN"

#define ENCRYPT_YES     1
#define ENCRYPT_NO      0

#define size_maxdatalen 1024
#define size_nodeid     20
#define size_port       20
#define size_packetmax  size_nodeid+size_port+size_maxdatalen+25+100

struct nnet_ {
    int port;
    int socket;
    unsigned char isEncrypt;
    char server[100];
    char uid[100];
    char nodeid[size_nodeid+1];
    unsigned char seccl[40];
    char m1[40];
    char g1[2];
    char privcl[40];
};

struct nnet_ nnet;

 void init_nnet(char *server, int port, char *nodeid, char *uid, unsigned char isenc) {
    strcpy(nnet.server,server);     //server ip
    nnet.port=port;                 //server port
    strcpy(nnet.nodeid,nodeid);     //node id
    strcpy(nnet.uid,uid);           //auth key
    nnet.isEncrypt=isenc;           //is encrypt?
    memset(nnet.seccl,0,16);        //sec key
    dh_key_byte_size(4);
    strcpy((char*)nnet.m1,"12A5EA1D");
    strcpy((char*)nnet.g1,"3");
    strcpy((char*)nnet.privcl,"1a5e77db");
 }

int nnet_send_pkg(char *destid, char *komut, char *data) {
char pak[size_packetmax], *p1;
unsigned char *enc;
crypt_long len;
unsigned int blen;

    if (nnet.seccl[0]) {
        sprintf(pak+1,";src=%s&dest=%s&;%s;%s",nnet.nodeid,destid,komut,data);
        enc=crypt_encrypt((unsigned char*)pak+1,strlen((char*)pak+1),nnet.seccl,&len);
        p1=base64(enc,len, &blen);
        free(enc);
        if (blen==0) {free(p1); return 2;}
        pak[0]='1';
        sprintf(pak,"1%s%s",p1,PAKETSON);
        free(p1);
    }
    else
        sprintf(pak,"1;src=%s&dest=%s&;%s;%s%s",nnet.nodeid,destid,komut,data,PAKETSON);
    if (send(nnet.socket,pak,strlen(pak),0)<0) return 1;
    return 0;
}

int nnet_get_pkg(char *pkg, unsigned int maxlen, unsigned int timeout){
fd_set rfds;
struct timeval tv;
int sel;
char *p1;

    tv.tv_usec = 0;
    tv.tv_sec  = timeout;

    FD_ZERO(&rfds);
    FD_SET(nnet.socket, &rfds);
    sel=select(nnet.socket+1, &rfds, NULL, NULL, &tv);
    if (sel==0) return 1;   //timeout
    if (sel<0) return 2;    //error
    sel=recv(nnet.socket, pkg, maxlen,0);
    if (sel<0) return 3;
    if (sel==0) return 4;   //disconnected
    pkg[sel]=0;
    if (pkg[0]!=PAKETBASRAW) return 5;  //paketbas yok
    p1=pkg+sel-size_paketson;
    if (memcmp(p1, PAKETSON, size_paketson)) return 6;    //paketson yok
    *p1=0;
   return 0;
}

int nnet_parse_pkg(char *in, char *src, char *dest, char *command, char *data){
int n;
char node[(size_nodeid+size_port)*2+20];
char *p1;
unsigned int len, blen ,ret=0;
unsigned char *dec=NULL;

    command[0]=0; data[0]=0;
    if (nnet.seccl[0]){
        p1=unbase64((unsigned char*)in+1,strlen(in+1),&len);
        if (len==0) {free(p1); return 5;}
        dec=crypt_decrypt((unsigned char*)p1,len,nnet.seccl,&blen);
        free(p1);
        if (dec==NULL) return 6;
        p1=(char*)dec+1;
    }
    else
        p1=in+2;

    n=sscanf(p1,"%[^;];%[^;];%s",node,command,data);
    if (n!=3) {ret= 1; goto son;}
    n=0;
    while (n!=2) {
        if (*p1==';') n++;
        p1++;
    }
    strcpy(data,p1);
    p1=strstr(node,"dest=");
    if (p1==NULL) strcpy(dest,"NULL");
    else {
        dest[0]=0;
        n=sscanf(p1,"dest=%[^&]",dest);
        if (n!=1) {ret= 2; goto son;}
    }
    p1=strstr(node,"src=");
    if (p1==NULL) {ret= 3; goto son;}
    src[0]=0;
    n=sscanf(p1,"src=%[^&]",src);
    if (n!=1) ret= 4;
son:
    if (nnet.seccl[0]) free(dec);
    return ret;
}

int nnet_connect(){
struct sockaddr_in serveraddr;
int yes = 1, ret=0, sock;
struct hostent *hostp;

    memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(nnet.port);

    if((serveraddr.sin_addr.s_addr = inet_addr(nnet.server)) == (unsigned long)INADDR_NONE){
        hostp = gethostbyname(nnet.server);
        if(hostp == (struct hostent*)NULL){
            //HOST NOT FOUND
            ret=1; goto son;
        }
        memcpy(&serveraddr.sin_addr, hostp->h_addr, sizeof(serveraddr.sin_addr));
    }

    if( (sock= socket(AF_INET, SOCK_STREAM, 0)) < 0){ret=2; goto son;}

    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        //reuse error
        ret=3; goto son;
    }
    if((connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0){
        //Conect error
        close(sock);
        ret=4; goto son;
    }
    nnet.socket=sock;
son:
    return ret;
}

int nnet_login(){
char paket[size_packetmax+1], pkg_uid[100];
char src[20], dest[20], command[10], data[size_maxdatalen+1];
int ret;
u puba[1024], pubsr[1024];
char *p1;
unsigned char i;

    if (nnet.isEncrypt) {
        memset(nnet.seccl,0,16);
        srandom(time(NULL));
        for (i = 0; i < 4; i++) sprintf(nnet.privcl+(i*2),"%.2x",(unsigned char)random());
        hes(nnet.g1,nnet.privcl,nnet.m1,puba);
        sprintf(pkg_uid,"key1=%s-%s-%s",nnet.g1,nnet.m1,puba);
        ret=nnet_send_pkg(nnet.nodeid,COM_LOG,pkg_uid); if (ret) return 11;
        ret=nnet_get_pkg(paket,sizeof(paket)-1,5); if (ret) return 12;
        if ((p1=strstr(paket,"key2="))==NULL) return 13;
        ret=sscanf(p1,"key2=%[^-]-%[^-]-%s",nnet.g1,nnet.m1,pubsr); if (ret!=3) return 14;
        hes((char*)pubsr,nnet.privcl,nnet.m1,nnet.seccl);
    }
    sprintf(pkg_uid,"uid=%s",nnet.uid);
    ret=nnet_send_pkg(nnet.nodeid,COM_LOG,pkg_uid); if (ret) return 1;
    ret=nnet_get_pkg(paket,sizeof(paket)-1,5); if (ret) return 2;
    ret=nnet_parse_pkg(paket,src,dest,command,data); if (ret) return 3;
    if (!strstr(data,"Logged in")) return 4;
    return 0;
}
