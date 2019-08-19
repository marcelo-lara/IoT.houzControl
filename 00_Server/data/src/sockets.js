let _socket = new WebSocket("ws://192.168.1.100/");

_socket.onopen = (ev)=>{
  status.render(actEnm.action_wsConnected);
};
_socket.onclose = (ws, ev)=>{
  status.render(actEnm.action_wsOffline);
};

_socket.onmessage=(ev)=>{
  const _upd=JSON.parse(ev.data);
  console.log("received", _upd);
  if(_upd.act==actEnm.action_wsConnected){
    deviceHandler.bind(_upd);
  }else{
    deviceHandler.update(_upd);
  }
}
const socket ={
  emit:(channel, jsonMessage)=>{
  
    if(_socket.readyState==_socket.CLOSED){
      _socket = new WebSocket("ws://192.168.1.100/");
    };
    
    _socket.send(JSON.stringify(jsonMessage));
  }
  
}
