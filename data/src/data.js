"use strict";
const typeEnm= {
  node: 0,
  light: 1,
  ac: 2,
  fan: 3,
  enviroment: 4,
  temp: 5,
  humidity: 6,
  pressure: 7,
  array2x: 8,
  array4x: 9,
  array8x: 10,
  envlight: 11,
  fxanim: 12
};

let devices=[
    {id: 0x00, name: 'server_node', type: typeEnm.node, fVal: 0, iVal: 0, node:0, status: -1},

    // Office
    {name: 'office_node',       id:  0x1, type: typeEnm.node, node: 1}, //N1DC04F0F0
    {name: 'office_AC',         id: 0x11, type: typeEnm.ac, node: 1}, //Air Conditioner on/off
    {name: 'office_light',      id: 0x13, type: typeEnm.light, node: 1}, //ceiling light | N1DC130001
    {name: 'external_light',    id: 0x17, type: typeEnm.envlight, node: 1}, //light sensor [0-1024]
    {name: 'external_temp',     id: 0x1A, type: typeEnm.temp, node: 1}, //temperature [celsius /100] | N1DA1A0000
    {name: 'external_humidity', id: 0x1B, type: typeEnm.humidity, node: 1}, //humidity [%] | N1DA1B0000
    {name: 'external_pressure', id: 0x1C, type: typeEnm.pressure, node: 1}, //pressure [hPa /FIX THIS ((x-900) * 100?)] | N1DA1C0000
    {name: 'external_weather',  id: 0x1F, type: typeEnm.enviroment, node: 1}, //all devices | N1DA1F0099

    //suite devices
    {name: 'suite_node',	  	id: 0x2,    type: typeEnm.node,       node: 2},
    {name: 'suite_light',	  	id: 0x21,   type: typeEnm.light,      node: 2},
    {name: 'suite_fan',		  	id: 0x22,   type: typeEnm.fan,        node: 2},
    {name: 'suite_AC',		  	id: 0x23,   type: typeEnm.ac,         node: 2},
    {name: 'suite_enviroment',	id: 0x25, type: typeEnm.enviroment, node: 2},
    {name: 'suite_temp',		  id: 0x26,   type: typeEnm.temp,       node: 2},
    {name: 'suite_humidity',	id: 0x27,   type: typeEnm.humidity,   node: 2},
    {name: 'suite_pressure',	id: 0x28,   type: typeEnm.pressure,   node: 2},

    //living devices
    {name: 'living_node',       id:  0x3, type: typeEnm.node,    node: 3},
    {name: 'living_mainLight',  id: 0x33, type: typeEnm.array2x, node: 3},  
    {name: 'living_dicroLight', id: 0x34, type: typeEnm.array8x, node: 3},  
    {name: 'living_spotLight',  id: 0x35, type: typeEnm.array4x, node: 3},  
    {name: 'living_fxLight',    id: 0x36, type: typeEnm.array2x, node: 3},  
    {name: 'living_fx',         id: 0x37, type: typeEnm.fxanim,  node: 3},  
    {name: 'living_AC',         id: 0x38, type: typeEnm.ac,      node: 3}
  ];


const actEnm={
    action_ack: 0x00,
    action_rfSentOk: 0x01,
    action_rfSentRetry: 0x02,
    action_rfSentFail: 0x03,
    action_rfReceived: 0x04,
    action_conn: 0x05,
    action_irReceived: 0x10,
    action_wsConnected: 0x11,
    action_wsOffline: 0x12,
    action_rfOffline: 0x14,
    action_rfOnline: 0x15

  };

const cmdEnm={
    CMD_QUERY: 0xA,
    CMD_VALUE: 0xB,
    CMD_SET: 0xC,
    CMD_EVENT: 0xD,
    CMD_STATUS: 0xE
  };

const statusEnm={
    st_down: -1,
    st_offline: 0,
    st_online: 1,
    wsOnline: 2,
    wsOffline: 3,
    rfOnline: 4,
    rfOffline: 5
  };

const sceneEnm={
    scene_Goodbye: 0xCF0,
    scene_Sleep: 0xCF1,
    scene_Hello: 0xCF2
  };