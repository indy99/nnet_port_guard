// Nanonet Library (c) 2015 Ruhsen.k

function nnet_connect(){
    init_crypt();
    init_ws(nnet_server);
}
function nnet_login(){
var str;    
    if (nnet_isEncrypt && nnet_seccl==0)  
        str=getkey1();		            
    else
        str="uid="+nnet_uid;    
    nnet_send_pkg(nnet_nodeid,"LOG",str)
}
function nnet_send_pkg(destid,command,data){
var x, enc, pkt, out;
    x=";src="+nnet_nodeid+"&dest="+destid+"&;"+command+";"+data;
    if (nnet_seccl) {
        enc=cryptx.encrypt(x,nnet_seccl);
        pkt=btoa(enc);
    }
    else pkt=x;
    
    out="1"+pkt+"\r\n\n\r";
  //////////////////
  //if ("tx" in window) tx.innerHTML=++txsayac+"-> "+out;
try {
        ws.send(out);
    }
   catch (e) {
        alert("wserr:"+e);
   }
}
function nnet_parse_pkg(gelen1,pm){
var gelen, dec, response;
	gelen=gelen1.substring(1,gelen1.length-4);
	if (nnet_seccl) {
            dec=atob(gelen);
            response = cryptx.decrypt(dec,nnet_seccl);
	}
	else response=gelen;
			
	var bol=response.split(";");

        if (bol[2]=="PON") {
            ws_ping=0; return 1;
	}
	if (bol[2]=="MSG") {
            if (bol[3].indexOf("Logged in") >=0) startping();
            return 2;
	}
	if (bol[2]=="LOG") {
            if (bol[3].indexOf("key2=")>=0) {
		var keys=bol[3].split("-");
                if (keys[2]==null) {return 3;}
		nnet_seccl=getkey2(keys[2]);
                nnet_login();
		return 4;
            }
	}    
        var srcdst=bol[1].split("&");
        p_src=srcdst[0].substring(4);
        if (srcdst[1]) p_dest=srcdst[1].substring(5); else p_dest="NULL";
        p_command=bol[2];
        p_data=bol[3];
        return 0;
}
