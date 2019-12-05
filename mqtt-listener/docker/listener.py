import paho.mqtt.client as mqtt
import sys
import os
import jsonify
import logging

import sendData
import mapper
import event
import settings

class Listener(object):

    def __init__(self):
        self.logger = logging.getLogger('Listener')
        logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        self.logger.debug('Logger for Listener was initialised')
        self.Settings = settings
        self.logger.debug('Setting machineID variable from config file')
        self.machineID = settings.machineID
        self.logger.info('MachineID was set to %s', self.machineID)
        self.logger.debug('Setting prefix within MQTT broker for machine from config file')
        self.prefix = settings.prefix
        self.logger.info('Prefix was set to %s', self.prefix)
        self.logger.debug('Setting broker for listener from config file')
        self.broker = settings.broker
        self.logger.info('Broker was set to %s', self.broker)
        self.logger.debug('Setting broker port for listener from config file')
        self.port = int(settings.port)
        self.logger.info('Broker port was set to %s', self.port)
        self.logger.debug('Setting user for listener from config file')
        self.__user = settings.user
        self.logger.info('Broker user was set to %s', self.__user)
        self.logger.debug('Setting password for listener from config file')
        self.__pw = settings.password
        self.logger.info('Broker password was set to %s', self.__pw)
        self.logger.debug('Initialising MQTT client with loaded credentials for listener')
        self.client = mqtt.Client()
        self.logger.info('MQTT client with loaded credentials was initialised')

    def on_message_msgs(self,mosq, obj, msg):
        #print("Withing Callback")
        # This callback will only be called for messages with topics that match
        # prefix/machineID/# and will be hand over to event
        self.logger.debug('Callback function was initiated')
        self.logger.info('The following topic triggered a callback function: %s', msg.topic)
        self.logger.info('The following payload arrived: %s', msg.payload)
        self.logger.debug('Object with Event-Class will be created')
        self.CurrentEvent = event.Event(msg.topic,msg.payload)
        self.logger.debug('Object with Event-Class was created')
        if self.CurrentEvent.valid is True:
            self.logger.info('Event on topic and payload are valid')
            self.logger.debug('Creating connector to C8Y´s MQTT')
            self.C8YConnector = sendData.MQTTc8yConnector()
            self.logger.debug('Connector to C8Y´s MQTT created, Creating SendMQTT to C8Y object and inserting event')
            self.C8YSendMQTTData = sendData.SendDataViaMQTT(self.C8YConnector,self.CurrentEvent)
            self.logger.debug('Connector to C8Y´s MQTT created, Creating SendMQTT to C8Y object and inserting event')

    def start(self):
        self.logger.info('Starting listener')
        self.topic = str(settings.prefix) + str(settings.machineID) + '/#'
        self.logger.info('Listening for callback on all messsages on topic %s: ', self.topic)
        self.client.message_callback_add(str(self.topic), self.on_message_msgs)
        self.logger.debug('Checking whether user/pw is required')
        if len(self.__user) > 0:
                self.logger.info('Setting user %s', self.__user)
                self.logger.info('Setting password %s', self.__pw)
                self.client.username_pw_set(username=self.__user,password=self.__pw)
        self.logger.info('Connecting')
        self.client.connect(self.broker, self.port, 60)
        self.client.subscribe("#", 0)
        self.logger.info('Start Loop forever and listening')
        self.client.loop_forever()

    def stop(self):
        self.client.loop_stop()
        self.logger.info('Loop forever stopped, disconnecting')
        self.client.disconnect()
        self.logger.debug('disconnected')


    def __del__(self):
        self.logger.debug('Deleting Listener Object and stoping loop_forever')
        self.client.loop_stop()
        self.client.disconnect()
