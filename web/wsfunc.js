// Nanonet Library (c) 2015 Ruhsen.k

///////////////////////////////////////////////////
var ws_ping=0;
var ws_t;
var ws=null;

function startping()
{
if (ws_ping){
	console.log("PONG not received!"); 
	ws.close();
	}
else
 {
    nnet_send_pkg(nnet_nodeid,"PIN","PING");
    ws_ping=1;
    ws_t=setTimeout(startping,60000);
 }
}

function stopping(){
	ws_ping=0;
	if (ws_t) clearTimeout(ws_t);
}

function init_ws(dest){
	ws = new WebSocket(dest); 
	ws.onopen = function(evt) { onOpen(evt) }; 
	ws.onclose = function(evt) { onClose(evt) }; 
	ws.onmessage = function(evt) { onMessage(evt) }; 
	ws.onerror = function(evt) { onError(evt) };
}

function onOpen(evt) { 
	if ("statws" in window) statws.innerHTML="CONNECTED";
	ws_onOpen();
}  

function onClose(evt) { 
	if ("statws" in window) statws.innerHTML="DISCONNECTED";
	stopping();
}  
	
function onMessage(evt) { 
	nnet_get_pkg(evt.data);
}  

function onError(evt) { 
	if ("statws" in window) statws.innerHTML="ERROR";
	stopping();
}  
/////////////