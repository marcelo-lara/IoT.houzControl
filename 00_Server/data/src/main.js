const deviceHandler = {
  bind: (_upd)=>{
    if(!_upd || !_upd.dev) return;

    //update device values
    for (const val of _upd.dev) {
      let target=devices.find(x=>x.id==val.id);
      if(target){
        if(val.iVal) target.iVal=val.iVal;
        if(val.fVal) target.fVal=val.fVal;
      }
    }

    //bind
    bind.all();
    devices.forEach(render.device); 
  },

  update: (_upd)=>{

    //status lights
    status.render(_upd.act);

    //check if device exists
    if(!_upd.dev || _upd.dev.id==0) return;
    let dev=devices.find(x=>x.id==_upd.dev.id);
    if(!dev) return;
    
    //update data
    dev.iVal=_upd.dev.iVal;
    dev.fVal=_upd.dev.fVal;

    //render device
    render.device(dev);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////
// application status
// . ws disconnected
// . ws connecting
// . ws connected
// . rf offline
// . rf online

const status = {
  rx: undefined,
  tx: undefined,
  st: undefined,
  setup: ()=>{
    status.st=document.getElementById('st');
    status.rx=document.getElementById('rx');
    status.tx=document.getElementById('tx');
  },
  _render: (elem, status, timeout)=>{
    if(!timeout) timeout=200;
    elem.className=status;
    setTimeout(()=>{elem.className='';},timeout);
  },
  render: stat=>{
    console.log("render status:", stat);
    switch(stat){
      case actEnm.action_rfReceived:  status._render(status.rx, 'rfOk'); break;

      case actEnm.action_rfSentOk:    status._render(status.tx, 'rfOk'); break;
      case actEnm.action_rfSentFail: status._render(status.tx, 'rfFail', 2000); break;
      case actEnm.action_rfSentRetry: status._render(status.tx, 'rfRetry'); break;

      case actEnm.action_rfOffline: 
        console.log("status: action_rfOffline", stat);
        status.st.className='err'; break;
      case actEnm.action_wsOffline: 
        console.log("status: action_wsOffline", stat);
        status.st.className='err'; break;
      case actEnm.action_wsConnected:
        console.log("status: action_wsConnected", stat);
        status.st.className='online'; break;
      case actEnm.action_rfOnline: 
        console.log("status: action_rfOnline", stat);
        status.st.className='online'; break;
    }
  }
};



(()=>{
  status.setup();
  deviceHandler.bind();
  bind.scene(document.querySelector("div.scene"));
})();