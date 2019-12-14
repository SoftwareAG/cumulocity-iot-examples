import configparser

config = configparser.ConfigParser()
config.read('config.ini')

broker = config['MQTT']['broker']
port = config['MQTT']['port']
user = config['MQTT']['user']
password = config['MQTT']['password']

machineID = config['Machine']['machineID']
prefix = config['Machine']['prefix']

tenant = config['C8YMQTT']['tenant']
tenantID = config['C8YMQTT']['tenantID']
c8yPort = config['C8YMQTT']['port']
c8yUser = config['C8YMQTT']['user']
c8yPassword = config['C8YMQTT']['password']
deviceID = config['C8YMQTT']['deviceID']
