#!flask/bin/python
from flask import Flask, jsonify, request
import mapper

app = Flask(__name__)


@app.route('/health')
def health():
    return {'Microservice is up and running'}

app.route('/parameters')
def parameters():
    return {parametersdict}


@app.route('/subscriber')
def get_subscriber():
    # username has form tenant/user
    tenant_id = request.authorization["username"].split('/')[0]
    subscriber = get_subscriber_for(tenant_id)
    return jsonify(subscriber)


def get_authorization():
    tenant_id = request.authorization["username"].split('/')[0]
    subscriber = get_subscriber_for(tenant_id)
    auth = base64_credentials(subscriber["tenant"], subscriber["name"], subscriber["password"])
    return auth


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80, debug=True)
