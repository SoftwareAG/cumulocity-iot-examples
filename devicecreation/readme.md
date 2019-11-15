# IoT Device Creation

This script aims to showcase typical steps for creating and registering Cumulocity devices.
In this example, we demonstrate necessary steps to create a device with a location attribute through
Cumulocity's REST API.

Please refer to the [online documentation](https://cumulocity.com/guides/reference/rest-implementation) for further details on the API.


The location attribute is used to render the device asset within the Cumulocity map widgets.


## Setup

Pre-requisites:
- Python (tested on Python 3.6+)
- 'json' and 'requests' packages installed (e.g. pip install xxx)
- Cumulocity tenant with Basic Authentication enabled and your user role allows for creation of new managed objects.

Open the IoTDeviceCreation.py script, and modify the host, name, and password.

## Running

- Run the python script though your console, ensuring the Python environment and required packages are available in the path.
	
	>> python ./IoTDeviceCreation.py
	
This will create a new device with location information.  Navigate to the 'Device Management' section of your tenant and open 'All Devices'.  You should see the new device created and registered.

![trigger](https://github.com/SoftwareAG/cumulocity-iot-examples/blob/master/devicecreation/c8y-screenshot-1.png) 

Select the newly created device and navigate to the "Location" menu option, where you should now see its location on the map.

![trigger](https://github.com/SoftwareAG/cumulocity-iot-examples/blob/master/devicecreation/c8y-screenshot-2.png) 


