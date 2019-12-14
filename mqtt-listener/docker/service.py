
import urllib.request, json, base64
import os
from datetime import datetime, date, time
from random import randint
from urllib.request import Request
from urllib.request import urlopen
import logging

import listener

logger = logging.getLogger('Service')
logger.debug('Logger for Listener was initialised')

# values provided into environment by cumulocity platform during deployment
C8Y_BASEURL = os.getenv('C8Y_BASEURL')
C8Y_BOOTSTRAP_USER = os.getenv('C8Y_BOOTSTRAP_USER')
C8Y_BOOTSTRAP_TENANT = os.getenv('C8Y_BOOTSTRAP_TENANT')
C8Y_BOOTSTRAP_PASSWORD = os.getenv('C8Y_BOOTSTRAP_PASSWORD')


# result is Base64 encoded "tenant/user:password"
def base64_credentials(tenant, user, password):
    str_credentials = tenant + "/" + user + ":" + password
    return 'Basic ' + base64.b64encode(str_credentials.encode()).decode()


# subscriber has form of dictionary with 3 keys {tenant, user, password}
def get_subscriber_for(tenant_id):
    req = Request(C8Y_BASEURL + '/application/currentApplication/subscriptions')
    req.add_header('Accept', 'application/vnd.com.nsn.cumulocity.applicationUserCollection+json')
    req.add_header('Authorization', base64_credentials(C8Y_BOOTSTRAP_TENANT, C8Y_BOOTSTRAP_USER, C8Y_BOOTSTRAP_PASSWORD))
    response = urlopen(req)
    subscribers = json.loads(response.read().decode())["users"]
    return [s for s in subscribers if s["tenant"] == tenant_id][0]

try:
    logger.info('Starting listener')
    Listener = listener.Listener()
    Listener.start()
except KeyboardInterrupt:
    logger.warning('KeyboardInterrupt was called, stopping listener and raising SystemExit')
    Listener.stop()
    raise SystemExit
