import logging

import mapper
import settings


class Event(object):

    def __init__(self,topic,payload):
        self.logger = logging.getLogger('Event')
        logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        self.logger.debug('Logger for Event was initialised')
        self.topic = topic.split('/')
        self.logger.info('The following topic arrived %s', self.topic)
        self.payload = payload.decode("utf-8").split(',')
        self.logger.info('The following topic arrived %s', self.payload)
        self.actualParameterName = self.topic[-1]
        self.logger.info('The actual parameter name extracted from topic is  %s', self.actualParameterName)
        self.actualParameterValue = self.payload[0]
        self.logger.info('The actual parameter value extracted from payload is  %s', self.actualParameterValue)
        self.logger.debug('Checking whether Parametername is listed in the parameter file')
        if mapper.checkWhetherParameterIsListed(self.actualParameterName) is False:
            self.logger.info('Parameter name is not listed inside the parameter file, setting event.valid to false and skipping event')
            self.valid = False
            return
        self.logger.info('Parametername is listed in the parameter file')
        self.valid = True
        self.logger.debug('Checking whether Paramtername is a Status or else')
        self.c8yTopic = 's/us'
        if self.actualParameterName == 'Status':
            self.logger.info('Paramtername is a Status')
            self.c8yPayload = '400,' + str(mapper.getStringfromStatusCode(self.actualParameterValue)) + ',' + str(self.actualParameterValue)
        else:
            self.actualParameterUnit = self.payload[1]
            self.logger.info('Paramtername is a Parameter')
            self.c8yPayload = '200,c8y_Data,' + str(self.actualParameterName) + ',' + str(self.actualParameterValue) + ',' + str(self.actualParameterUnit)
        self.logger.info('C8yPayload was set inside the Event to %s', self.c8yPayload)
        self.logger.info('c8yTopic was set inside the Event to %s', self.c8yTopic)
