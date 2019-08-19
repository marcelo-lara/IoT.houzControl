#include "HouzRfLink.h"
#include "Arduino.h"

//Radio
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define rf_ce   0 //D3
#define rf_csn 15 //D8
RF24 radio(rf_ce,rf_csn);

#include <QueueArray.h>
QueueArray <DevicePkt> rfInQueue;
QueueArray <DevicePkt> rfOutQueue;

// rf pipes
#define rfChannel		0x5B   
#define rf_server_tx	0xA0
#define rf_server_rx	0xB0
#define rf_office_tx	0xA1
#define rf_office_rx	0xB1
#define rf_suite_tx		0xA2
#define rf_suite_rx		0xB2
#define rf_living_tx	0xA3
#define rf_living_rx	0xB3
#define rf_wemos_tx		0xA4
#define rf_wemos_rx		0xB4


HouzRfLink::HouzRfLink(){

}

HouzRfLink::~HouzRfLink(){
    Serial.print("HouzRfLink::~HouzRfLink()");
}

void HouzRfLink::init(){
    radioReady=false;
    radio.begin();
    radio.setPALevel(RF24_PA_HIGH); //RF24_PA_HIGH | RF24_PA_LOW | RF24_PA_MAX
    radio.setDataRate(RF24_1MBPS);
    radio.enableDynamicAck();
    radio.setCRCLength(RF24_CRC_8);
    radio.setChannel(rfChannel);
    radio.setRetries(10, 5);  

    //pipes setup | server node
    radio.openWritingPipe(rf_server_tx);
    radio.openReadingPipe(1, rf_office_tx);
    radio.openReadingPipe(2, rf_suite_tx);
    radio.openReadingPipe(3, rf_living_tx);
    radio.openReadingPipe(4, rf_wemos_tx);

    //debug
    radioReady = (rfChannel == radio.getChannel()); //test if radio is enabled
    if(!radioReady){
      Serial.println("RF ERROR <<<<<<<<<");
      radio.printDetails();
    }else{
      Serial.println("radio\tonline");
    };

    //
    rfInQueue.enqueue(
      getDevice(server_node, server_rf, (radioReady?action_rfOnline:action_rfOffline))
    );

}

bool HouzRfLink::hasData(){

	//if radio is not enabled, discard anything
	if (!radioReady) return false;

  //deliver next packet 
  sendNext();

	uint8_t _radioNode;
	if (!radio.available(&_radioNode)) 
    return !rfInQueue.isEmpty();

	//get payload
	unsigned long _radioPayLoad;
	while (radio.available()) {
		radio.read(&_radioPayLoad, sizeof(unsigned long));
	}

	//prepare for next packet
	radio.startListening();

  Serial.print("rfIn\t");
  Serial.print(_radioNode, HEX);
  Serial.print(":");
  Serial.println(_radioPayLoad, HEX);


  //decode packet
  DevicePkt pkt = rfDecode(_radioPayLoad, _radioNode);
  rfInQueue.enqueue(pkt);
  return true;
}

bool HouzRfLink::status(){
    return radioReady;
}

DevicePkt HouzRfLink::getData(){
  try
  {
    return rfInQueue.dequeue();
  }
  catch(const std::exception& e)
  {
    DevicePkt dev;
    dev.id=-1;
    return dev;
  }
}

bool HouzRfLink::send(DevicePkt dev){
  rfOutQueue.enqueue(dev);
  return !rfOutQueue.isFull();
};

void HouzRfLink::sendNext(){
  if(rfOutQueue.isEmpty()) return;
  DevicePkt dev = rfOutQueue.dequeue();

  //open write pipe
  uint64_t writeAddress;
  switch (dev.node) {
		case office_node: writeAddress = rf_office_rx; break;
		case suite_node: writeAddress = rf_suite_rx; break;
		case living_node: writeAddress = rf_living_rx; break;
		case wemos_node: writeAddress = rf_wemos_rx; break;
    default:
      Serial.printf("[error] node %i not found\n", dev.node);
      return;
  };
  u32 msg = rfEncode(dev);

  //rf send
  radio.stopListening();
  radio.openWritingPipe(writeAddress);
  bool result = 0;
  result = radio.write(&msg, sizeof(unsigned long), 0);
  radio.startListening();

  Serial.print("rfOut\t");
  Serial.print(dev.node, HEX);
  Serial.print(":");
  Serial.print(msg, HEX);
  Serial.println(result?" ok":" err");
  
  //notify clients
  rfInQueue.enqueue(getActNotification(result?action_rfSentOk:action_rfSentFail));
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HELPERS
DevicePkt HouzRfLink::getActNotification(int action){
  DevicePkt actPkt;
  actPkt.id=server_node;
  actPkt.payload=action;
  return actPkt;
}

DevicePkt HouzRfLink::getDevice(int nodeId, int deviceId, u32 payload){
  DevicePkt actPkt;
  actPkt.node=nodeId;
  actPkt.id=deviceId;
  actPkt.payload=payload;
  return actPkt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RF CODEC
unsigned long HouzRfLink::rfEncode(DevicePkt dev){
	unsigned long retVal = 0xD;
	retVal = (retVal << 4) + dev.cmd;
	retVal = (retVal << 8) + dev.id;
	retVal = (retVal << 16) + dev.payload;
	return retVal;
}

DevicePkt HouzRfLink::rfDecode(unsigned long payload, int nodeId){
	DevicePkt decoded;

	if (((payload >> 28) == 0xD)) {
		decoded.node = nodeId;

		//device payload
		decoded.cmd = ((payload >> 24) & 0x0F);
		decoded.id = ((payload >> 16) & 0x0FF);
		decoded.payload = ((payload) & 0x0000FFFF);
	}
	return decoded;

};
