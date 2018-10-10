#ifndef __MQTT_CLI_H
#define __MQTT_CLI_H

int MqttModuleInit(void *);
int MqttModuleExit(void *);
int MqttModuleTick(void *);
int Send2MqttModule(int type,void *msg);

#endif
