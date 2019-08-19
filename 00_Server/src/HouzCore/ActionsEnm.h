#pragma once
//////////////////////////////////////
// IMPORTANT: changes must be reflected on client side

#define action_ack          0x00
#define action_rfSentOk     0x01
#define action_rfSentRetry  0x02
#define action_rfSentFail   0x03
#define action_rfReceived   0x04
#define action_conn         0x05
#define action_irReceived   0x10
#define action_wsConnected  0x11
#define action_wsOffline    0x12
#define action_rfOffline    0x14
#define action_rfOnline     0x15

#define action_swClick      0xA1
#define action_swDblClick   0xA2
#define action_swLongPress  0xB1
#define action_swScenePress 0xB2