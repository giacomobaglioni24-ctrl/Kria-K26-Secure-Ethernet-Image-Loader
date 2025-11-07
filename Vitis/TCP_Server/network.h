#ifndef NETWORK_H
#define NETWORK_H

#include "xparameters.h"
#include "xil_types.h"
#include "xstatus.h"



int NetworkConfig(struct netif *NetIf);
void NetworkProcessPkt(struct netif *NetIf);

#endif // NETWORK_H
