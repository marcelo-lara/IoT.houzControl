//WebSocket mix

//const socket = new WebSocket("ws:" + window.location.href.replace(window.location.protocol,""));
let _socket = new WebSocket("ws://192.168.1.100/");

_socket.onopen = (ev)=>{
  status.render(actEnm.action_wsConnected);
  console.log("ws connected..", ev);
};
_socket.onclose = (ws, ev)=>{
  status.render(actEnm.action_wsOffline);
  console.log("ws disconnected..", ws, ev);
};

_socket.onmessage=(ev)=>{
  const _upd=JSON.parse(ev.data);
  console.log("ws:", actEnm.action_wsConnected);
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

//Socket.Io mix
// const socket = io.connect("ws:" + window.location.href.replace(window.location.protocol,""));

// socket.on('connected', () => {
//   console.log('Socket Connected')
// })
// socket.on('disconnect', () => {
//   console.log('Socket Disconnected')
// })

// socket.on('data', data => {
//   console.log('data>>', data);
//   if(data.devices) devices=data.devices;
//   if(data.typeEnm) typeEnm=data.typeEnm;
//   if(data.cmdEnm) cmdEnm=data.cmdEnm;
//   if(data.actEnm) actEnm=data.actEnm;
//   if(data.statusEnm) statusEnm=data.statusEnm;
//   deviceHandler.bind();
// });

// socket.on('update', deviceHandler.update) //delegate update handling
