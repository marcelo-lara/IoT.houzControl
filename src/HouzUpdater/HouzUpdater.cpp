// updateTimer
#define nodeUpdate_pending 0
#define nodeUpdate_waiting 1
#define nodeUpdate_ok      2
#define nodeUpdate_idle    3

#include "Arduino.h"
#include "../HouzRfLink/HouzRfLink.h"
#include "HouzUpdater.h"

//TODO: keep node status on deviceModel
struct NodeUpdate{
  u8 nodeId;
  unsigned long timeout;
  u8 status;
  int currNode;
};

//TODO: instead of calling nodes, call devices
int nodeList[] = {living_node, office_node, suite_node};
int nodeListLenght = 2;
NodeUpdate nodeUpdate;

//TODO: update enviroment

HouzUpdater::HouzUpdater(HouzRfLink &houzLink){
    link = &houzLink;

//TODO: persist data
  nextInterval=5000;   //5s 
  idleInterval=600000; //10m

}

void HouzUpdater::init(){
  if(!link->status()){
    Serial.println("RF: timer not enabled..");
    return;
  }
  nodeUpdate.nodeId = nodeList[0];
  nodeUpdate.currNode = 0;
  nodeUpdate.status = nodeUpdate_pending;
}

void HouzUpdater::update(){
  switch (nodeUpdate.status)
  {
  case nodeUpdate_idle:
  case nodeUpdate_pending:
    if(millis()<nodeUpdate.timeout) return;
    return timerCall();
    break;

  case nodeUpdate_ok:
  case nodeUpdate_waiting:
    if(millis()<nodeUpdate.timeout) return;
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
  nodeUpdate.currNode++;
  if(nodeUpdate.currNode>nodeListLenght){
    nodeUpdate.status=nodeUpdate_idle;
    nodeUpdate.currNode=0;
    nodeUpdate.timeout=millis()+idleInterval; //10m: (5*60*1000));
  }else{
    nodeUpdate.status=nodeUpdate_pending;
    nodeUpdate.timeout=millis()+nextInterval;  //10s: (2*1000));
  }
  nodeUpdate.nodeId = nodeList[nodeUpdate.currNode];
};

//call node
void HouzUpdater::timerCall(){
    nodeUpdate.status=nodeUpdate_waiting;
    nodeUpdate.timeout=millis()+1000;
    DevicePkt pkt;
    pkt.id=nodeUpdate.nodeId;
    pkt.node=nodeUpdate.nodeId;
    pkt.payload=0;
    pkt.cmd=10;
    if(link->send(pkt)){
      nodeUpdate.status=nodeUpdate_ok;
    };
};


