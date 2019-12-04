import jsonify
import csv
import pandas
import logging

logger = logging.getLogger('Mapper')
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger.debug('Logger for Mapper was initialised')
statuscodesdict = pandas.read_csv('statuscodes.csv',sep=',', header=0, names=['Status','Description'],dtype=str).to_dict(orient='records')
logger.info('Printing Statuscode csv file %s',statuscodesdict)
parametersdict = pandas.read_csv('parameters.csv',sep=',', header=0, names=['Parametername'],dtype=str).to_dict(orient='records')
logger.info('Printing parameter csv file %s',parametersdict)

def getDataFromTopic(topic):
    return topic

def getDataFromPayload(payload):
    return payload

def getStringfromStatusCode(status):
    logger.info('Calling getStringfromStatusCode function with status %s',status)
    for dict_ in statuscodesdict:
        logger.debug('Looping through statuscodesdict, actual entry is %s',dict_)
        logger.debug('Checking whether %s equals %s',dict_, status)
        if dict_['Status'] == status:
            logger.debug('Returning %s', dict_.get('Description','No Description'))
            return dict_.get('Description','No Description')
    logger.info('Did not find any Status')
    return 0

def checkWhetherParameterIsListed(parameter):
    for dict_ in parametersdict:
        if dict_['Parametername'] == parameter:
            return True
    return False
