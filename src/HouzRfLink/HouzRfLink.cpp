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
    }
}

bool HouzRfLink::hasData(){

	//if radio is not enabled, discard anything
	if (!radioReady) return false;

  //deliver next packet 
  sendNext();

	uint8_t _radioNode;
	if (!radio.available(&_radioNode)) return false;

	//get payload
	unsigned long _radioPayLoad;
	while (radio.available()) {
		radio.read(&_radioPayLoad, sizeof(unsigned long));
	}

	//prepare for next packet
	radio.startListening();

  //decode packet
  DevicePkt pkt = rfDecode(_radioPayLoad, _radioNode);
  rfInQueue.enqueue(pkt);
  Serial.print("RFrec> n");
  Serial.print(_radioNode, HEX);
  Serial.print(":");
  Serial.println(_radioPayLoad, HEX);

  return true;
}

bool HouzRfLink::status(){
    return radioReady;
}

DevicePkt HouzRfLink::update(){
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

  //notify
  Serial.print("RFsnd> n");
  Serial.print(dev.node);
  Serial.print(":");
  Serial.print(msg, HEX);

  //rf send
  radio.stopListening();
  radio.openWritingPipe(writeAddress);
  bool result = 0;
  result = radio.write(&msg, sizeof(unsigned long), 0);

  radio.startListening();
  Serial.println(result?" ok":" error");
};




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
