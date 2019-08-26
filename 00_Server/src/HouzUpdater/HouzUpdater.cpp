// updateTimer
#define devUpdate_pending 0
#define devUpdate_waiting 1
#define devUpdate_ok      2
#define devUpdate_idle    3

#include "Arduino.h"
#include "../HouzLink_rf/HouzLink_rf.h"
#include "HouzUpdater.h"

//TODO: keep node status on deviceModel
struct DevUpdate{
  u8 devId;
  unsigned long timeout;
  u8 status;
  int currNode;
};

//TODO: instead of calling nodes, call devices
int devList[] = { 
  living_node, 
  suite_node,
  external_weather
};
int devListLenght = 2;
DevUpdate devUpdate;

//TODO: update enviroment

HouzUpdater::HouzUpdater(HouzLink_rf &houzLink, HouzCore &houzCore ){
    link = &houzLink;
    core = &houzCore;

//TODO: persist data
  nextInterval=5000;   //5s 
  idleInterval=600000; //10m

}

void HouzUpdater::init(){
  if(!link->status()){
    Serial.println("RF: timer not enabled..");
    return;
  }
  devUpdate.devId = devList[0];
  devUpdate.currNode = 0;
  devUpdate.status = devUpdate_pending;
}

void HouzUpdater::update(){
  //TODO: timerUpdates should not happen when a user is logged in
  switch (devUpdate.status)
  {
  case devUpdate_idle:
  case devUpdate_pending:
    if(millis()<devUpdate.timeout) return;
    return timerCall();
    break;

  case devUpdate_ok:
  case devUpdate_waiting:
    if(millis()<devUpdate.timeout) return;
    //TODO: store node status
    timerNext();
    break;
  
  default:
    break;
  }
};

void HouzUpdater::setInterval(int _action, int _seconds){
  switch (_action)
  {
  case 0: //nextInterval
    nextInterval=_seconds*1000;
    break;
  case 1: //idleInterval
    idleInterval=_seconds*1000;
    break;
  }
  //TODO:persist changes
};


//seek next node
void HouzUpdater::timerNext(){
  devUpdate.currNode++;
  if(devUpdate.currNode>devListLenght){
    devUpdate.status=devUpdate_idle;
    devUpdate.currNode=0;
    devUpdate.timeout=millis()+idleInterval; //10m: (5*60*1000));
  }else{
    devUpdate.status=devUpdate_pending;
    devUpdate.timeout=millis()+nextInterval;  //10s: (2*1000));
  }
  devUpdate.devId = devList[devUpdate.currNode];
};

//call node
void HouzUpdater::timerCall(){
    // devUpdate.status=devUpdate_waiting;
    // devUpdate.timeout=millis()+1000;
    // DevicePkt dev = (DevicePkt)core->getDevice(devUpdate.devId);
    // if(link->send(dev)){
    //   devUpdate.status=devUpdate_ok;
    // };
};


