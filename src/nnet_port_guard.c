//
//  Nanonet library
//  (c) by Ruhsen.k, 2015
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "nnet_lib.c"

#define file_conf_local     "nnet_port_guard.conf"
#define file_conf_etc       "/etc/nnet_port_guard/nnet_port_guard.conf"
char file_conf[256];
unsigned char isDaemon=0;

void log_print(const char *fmt, ...) {

    if (isDaemon) return;
#define MAXSTRING 1024
    va_list args;
    static char message[MAXSTRING+1];
    message[MAXSTRING] = 0;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    printf("%s",message); fflush(stdout);
}

int get_conf(char *id, char *key){
FILE *f;
int idok=0, keyok=0;
char str[100], s1[100];

    strcpy(file_conf,file_conf_local);
    f=fopen(file_conf,"r");
    if (f==NULL) {
        strcpy(file_conf,file_conf_etc);
        f=fopen(file_conf,"r");
        if (f==NULL) return 1;
    }
    log_print("Conf file found:%s\n",file_conf);
    while (1) {
        if (fgets(str,sizeof(str),f)==NULL) break;
        if (sscanf(str,"%s",s1)!=1) continue;
        if (sscanf(s1,"NodeID=%s",id)==1) idok=1;
        else
        if (sscanf(s1,"UidKey=%s",key)==1) keyok=1;
    }
    fclose(f);
    if (idok && keyok) return 0; else return 1;
}

void getstat(char *s) {
time_t tm;
    FILE* pipe = popen("service ssh status", "r");
    if (!pipe) {strcpy(s,"ERROR on command"); return;}
    char buf[128]="...";
    fgets(buf, 128, pipe);
    tm = time(NULL);
    sprintf(s,"[%s] %s",ctime(&tm),buf);
    while(!feof(pipe)) {
    	fgets(buf, 128, pipe);
    }
    pclose(pipe);
    return;
}

void SSH_start(unsigned char x){
    if (x) {
        log_print("SSH daemon starting ...\n");
        system("service ssh start");
    }
    else {
        log_print("SSH daemon stopping ...\n");
        system("service ssh stop");
    }
}

int main(int argc, char *argv[]) {
char buffer[size_packetmax+1];
int ret;
char src[20], dest[20], command[10], data[size_maxdatalen+1];
//char *client_id="S12233445500:1";
int stat=0, setStop=0;

//char *NodeID="S12233445500:1";
//char *UidKey="eXTSbDvmkFD6OtChrKAWcWJkUKenaot78pRCQyjqXnH3Hn6InCK5f7+hwkoOITYY";
char NodeID[50], UidKey[100];

//////////
    for (ret=0; ret<argc; ret++){
        if (!strcmp(argv[ret],"--help")) {
            printf(\
"Nanonet Port Guard V1.0.0, 2015 by Ruhsen.k, nnet.server [at] gmail.com\n\n"
"   Options     :   -d run as daemon\n"
"*******************************************\n");
            return 0;
        }
        else
        if (!strcmp(argv[ret],"-d")) isDaemon=1;
    }
    if (isDaemon) {
        switch (fork()){
            case -1:
            log_print("[lnk] fork");
            return -1;

        case 0:
            break;

        default:
            return 0;
        }
        umask(0);
        if (setsid() < 0){
            log_print("[lnk] setsid");
            return -1;
        }
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
   }
//////////

connect:
    log_print("Start ...\n");

    if (get_conf(NodeID, UidKey)) {log_print("[%s] Config file not found/or read error!\n",file_conf); return 1;}
    init_nnet("69.28.92.166",9090,NodeID,UidKey,ENCRYPT_YES);
    //init_nnet("192.168.1.12",9090,NodeID,UidKey,ENCRYPT_YES);
    ret=nnet_connect(); if (ret) {log_print("error connect:%d\n",ret); sleep(30); goto connect;}
    ret=nnet_login();   if (ret) {log_print("error login:%d\n",ret); return 1;}
    log_print("Logged in ...\n");

    if (setStop) SSH_start(0);

    while(1){
        ret=nnet_get_pkg(buffer,size_packetmax,60);  //60->timeout 60 sn.
        if (ret==0) {
            log_print("Data received->%s\n",buffer);
            ret=nnet_parse_pkg(buffer,src,dest,command,data); if (ret) {log_print("nnet_parse_pkg error:%d\n",ret); continue;}
            else log_print("Data parsed:\nsrc=%s, dest=%s, command=%s, data=%s\n",src,dest,command,data);
            if (!strcmp(data,"Status")) {
                log_print("Sending Status ... ");
                getstat(buffer);
                ret=nnet_send_pkg(src,COM_PUT,buffer); if (ret) log_print("Send error:%d\n",ret); else log_print("OK\n");
            }
            else
            if (strstr(data,"SSH=")) {
                ret=sscanf(data,"SSH=%d",&stat);
                if (ret!=1) continue;
                if (stat==1) {
                    SSH_start(1); setStop=0;
                }
                else {
                    SSH_start(0); setStop=1;
                }
                log_print("Sending Status ... ");
                getstat(buffer);
                ret=nnet_send_pkg(src,COM_PUT,buffer); if (ret) log_print("Send error:%d\n",ret); else log_print("OK\n");
            }
        }
        else
        if (ret==1) {
            nnet_send_pkg(NodeID,COM_PIN,"PIN");    //timeout, send PING
        }
        else {
            if (ret==4) log_print("Server Disconnected\n");
            else log_print("Error :%d\n",ret);
            SSH_start(1);
            sleep(30);
            goto connect;
        }
    }
return 0;
}
