import time


def get_current_time():
    current_time = time.time()
    current_time = int(current_time)
    return current_time

def check_beacon_list(beacon_id, beacon_list):
    for beacon in beacon_list:
        if beacon['HASH'] == beacon_id:
            return True
        continue
    return False

def update_beacon_heartbeat(beacon_id, beacon_list):
    for beacon in beacon_list:
        if beacon['HASH'] == beacon_id:
            beacon['LAST_HEARTBEART'] = get_current_time()
        continue

def add_beacon(beacon_dict, beacon_list):
    beacon = {'HASH':'','TIMEOUT': '','IP':[],'UID':"",'USERNAME':'','PID':'','LAST_HEARTBEART': 1,}
    beacon['USERNAME'] , beacon['UID'] = beacon_dict['uid'].split('|')
    beacon['IP'] = beacon_dict['ip'] + beacon_dict['remote_addr']
    beacon['PID'] = beacon_dict['pid']
    beacon['LAST_HEARTBEART'] = get_current_time()
    beacon['TIMEOUT'] = '1'
    beacon['HASH'] = beacon_dict['pid']
    beacon_list.append(beacon)

def send_beacon_task(beacon_id, task_list):
    for task in task_list:
        if task['BEACON_ID'] == beacon_id and task['HANDLE'] is False:
            return "type={task_type}&data={task_data}&task_id={task_id}".format(task_type=str(task['TASK_TYPE']), task_data=task['TASK_DATA'], task_id=task['TASK_ID'])
    return "type={task_type}&data=None&task_id=None".format(task_type=0)
    
