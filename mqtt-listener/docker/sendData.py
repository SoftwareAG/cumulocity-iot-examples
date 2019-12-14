import paho.mqtt.client as mqtt
import settings
import logging

class SendDataViaMQTT(object):


    def __init__(self, MQTTc8yConnector, CurrentEvent):
        self.logger = logging.getLogger('sendData')
        logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        self.logger.debug('Logger for sendData was initialised')
        self.MQTTc8yConnector = MQTTc8yConnector
        self.c8yTopic = CurrentEvent.c8yTopic
        self.logger.info('Current Event created following topic for C8Y Mqtt to create measurement: %s', self.c8yTopic)
        self.c8yPayload = CurrentEvent.c8yPayload
        self.logger.info('Current Event created following payload for C8Y Mqtt to create measurement: %s', self.c8yPayload)
        self.publishSeries()

    def publishSeries(self):
        self.logger.info('Publishing to C8Y Mqtt')
        self.MQTTc8yConnector.client.publish(self.c8yTopic,self.c8yPayload)


class MQTTc8yConnector(object):

    def __init__(self):
        self.logger2 = logging.getLogger('MQTTc8yConnector')
        logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        self.logger2.debug('Logger for sendData was initialised')
        self.c8yMqttHost = settings.tenant
        self.logger2.info('c8yMqtt host was set to %s', self.c8yMqttHost)
        self.c8yMqttPort = int(settings.c8yPort)
        self.logger2.info('c8yMqtt port was set to %s', self.c8yMqttPort)
        self.tenantID = settings.tenantID
        self.logger2.info('The tenant ID is %s', self.tenantID)
        self.clientID = settings.deviceID
        self.deviceID = self.clientID
        self.logger2.info('The used clientID for C8Y was set to %s', self.clientID)
        self.__c8yUser = settings.c8yUser
        self.logger2.info('c8y user was set to %s', self.__c8yUser)
        self.__c8yPassword = settings.c8yPassword
        self.logger2.info('c8y password was set to %s', self.__c8yPassword)
        self.baseTopic = 's/us/'
        self.logger2.debug('Calling connect method within MQTTc8yConnectorclass')
        self.connect()

    def connect(self):
        self.logger2.debug('Calling connect method within MQTTc8yConnectorclass')
        self.logger2.debug('Initialising MQTT client with loaded credentials for MQTTc8yConnector')
        self.client = mqtt.Client(client_id=self.deviceID)
        self.logger2.info('MQTT client with loaded credentials was initialised')
        self.logger2.info('Setting user/Password')
        self.client.username_pw_set(username=self.__c8yUser,password=self.__c8yPassword)
        self.logger2.info('Connecting')
        self.client.connect(self.c8yMqttHost, self.c8yMqttPort,60)

    def disconnect(self):
        self.logger2.info('Disconnecting')
        self.client.disconnect()
        self.logger2.debug('Disconnected')

    def __del__(self):
        self.logger2.debug('Deleting MQTTC8YConnector Object and disconnecting')
        self.disconnect()
