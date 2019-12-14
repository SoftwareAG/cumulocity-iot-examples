'''
This script aims to showcase typical steps for creating and registering Cumulocity devices.
In this example, we demonstrate necessary steps to create a device with a location attribute through
Cumulocity's REST API.

Please refer to the documentation for further details on the API:
https://cumulocity.com/guides/reference/rest-implementation/

The location attribute is used to render the device asset within the Cumulocity map widgets.

Created on Nov 1, 2019

@author: JHUM
'''

import requests
from requests.auth import HTTPBasicAuth
import json

host = 'https://humblescience.us.cumulocity.com'
username = 'testuser@softwareag.com'
password = 'test123!'

auth = HTTPBasicAuth(username, password)
session = requests.Session()
session.auth = auth
session.headers.update({
                'Content-Type': 'application/json',
                'Accept': 'application/json'
                })


# Prepare templates
NEW_DEVICE_TEMPLATE = """{
    "name": "",
    "type": "",
    "c8y_IsDevice": {},
    "com_cumulocity_model_Agent": {},
    "c8y_SupportedOperations": [ "c8y_Restart", "c8y_Configuration", "c8y_Software", "c8y_Firmware" ],
    "c8y_Hardware": {
        "revision": "N/A",
        "model": "",
        "serialNumber": ""
    },
    "c8y_Position": {
        "alt": 67,
        "lng": -80.138106,
        "lat": 25.779828,
        "trackingProtocol" : "TELIC",
        "reportReason" : "Time Event"
    }
}"""
deviceDescriptionJson = json.loads(NEW_DEVICE_TEMPLATE)


# Check whether device is already registered.
def isDeviceRegistered(deviceIdPath, deviceId):
    return getInternalDeviceId(deviceIdPath, deviceId) != None

def getInternalDeviceId(deviceId):
    url = host + '/identity/externalIds/c8y_Serial/' + deviceId
    response = session.get(url)
    #print ('Response -> ' + str(response.status_code))
    if response.status_code == 200:
            jsonResponse = response.json()
            return jsonResponse['managedObject']['id']
    return None

# Create the device from template
def createDevice(deviceName, deviceType, deviceId):
    url = host + '/inventory/managedObjects'
    
    deviceDescriptionJson['name'] = deviceName
    deviceDescriptionJson['type'] = deviceType
    deviceDescriptionJson['c8y_Hardware']['serialNumber'] = deviceId
    print (json.dumps(deviceDescriptionJson, indent = 4))
    response = session.post(url, json=deviceDescriptionJson)
    #print ('Response -> ' + str(response.status_code))
    if (response.status_code == 201):
        newDeviceDetails = response.json()
        #print ("New device created [id=%s]" % newDeviceDetails['id'])
        return newDeviceDetails;
    return None          
        # print ('Response -> ' + json.dumps(response.json(), indent = 4))

# register the device in the inventory
def registerDevice(deviceId, newDeviceDetails):
    internalId = newDeviceDetails['id']
    # create an external identifier for association by our own id
    # assign to c8y_serial attribute for now. 
    url = host + '/identity/globalIds/' + internalId + '/externalIds'
    associateDescriptionJson = json.loads("""{
        "type" : "c8y_Serial",
        "externalId" : ""
        }""")
    associateDescriptionJson['externalId'] = deviceId   
    response = session.post(url, json=associateDescriptionJson)
    # print ('Response -> ' + str(response.status_code))
    #print(response.text)
    if (response.status_code == 201):
        newDeviceAssoc = response.json()
        print ("New device [id=%s] associated with external id ['c8y_Serial'=%s]" % (newDeviceAssoc['managedObject']['id'], newDeviceAssoc['externalId']))
        # print ('Response -> ' + json.dumps(response.json(), indent = 4))


def checkAndRegisterDevice(deviceName, deviceType, externalId):
    internalDeviceId = getInternalDeviceId(externalId)
    if internalDeviceId != None:        
        print('Managed object [external id=%s] already registered with internal id=%s.' % (externalId, internalDeviceId))
        return internalDeviceId
    else:
        print('Managed object [id=%s] not registered, registering as new device ...' % externalId)
        newDeviceDetails = createDevice(deviceName, deviceType, externalId)
        if (newDeviceDetails != None):
            registerDevice(externalId, newDeviceDetails)
            return newDeviceDetails['id'] 
        else:
            print("Error creating new device! [id=%s]", externalId)
            return None
     
if __name__ == "__main__":
    checkAndRegisterDevice('MyDevice', 'MyDeviceType', 'my-device-1')
        

