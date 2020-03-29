# Linco2

模拟Cobalt Strike的Beacon与C2通信过程，实现了基于HTTP协议的Linux C2，客户端可以通过curl就能下发Beacon任务。

**本项目旨在交流编程技术，初衷未曾想把它作为一个超大项目进行开发。**

> 由于对开源库依赖很少，因此不需要在上线机器上安装库，兼容性较强。


![](/image/2020-03-29-07-12-25.png)

## 构建方法

```
$ git clone https://github.com/Rvn0xsy/Linco2.git
$ cd Linco2
$ make
$ pip3 install flask
$ python3 server/webserver.py
```

最后执行`Linco2`目录下的`beacon`即可上线。

## 使用方式

## 查看上线主机

`curl http://127.0.0.1:5000/beacons/`

```
root@kali:~# curl http://127.0.0.1:5000/beacons/               
[                                                                   
  {                                                                 
    "HASH": "1540",                                                 
    "IP": [
      "127.0.0.1"
    ], 
    "LAST_HEARTBEART": 2, 
    "PID": "1540", 
    "TIMEOUT": "1", 
    "UID": "0", 
    "USERNAME": "root"
  }, 
  {
    "HASH": "35395", 
    "IP": "127.0.0.1|192.168.3.145|172.17.0.1|127.0.0.1", 
    "LAST_HEARTBEART": 1585479717, 
    "PID": "35395", 
    "TIMEOUT": "1", 
    "UID": "0", 
    "USERNAME": "root"
  }
]

```

其中`HASH`与`PID`相等，我通过PID来作为beacon的唯一性判定标准，这样就能对指定的beacon下发任务。




## 下发任务

`curl -X POST --data "task_data=id&task_type=1&beacon_id=35395" http://127.0.0.1:5000/push_task/`




```
root@kali:~# curl -X POST --data "task_data=id&task_type=1&beacon_id=35395" http://127.0.0.1:5000/push_task/
{
  "msg": "Success", 
  "status": 0
}
```

## 查看任务队列


`curl http://127.0.0.1:5000/tasks/`

```
root@kali:~# curl http://127.0.0.1:5000/tasks/[
  {
    "BEACON_ID": "1540", 
    "HANDLE": true, 
    "TASK_DATA": "id", 
    "TASK_ID": 0, 
    "TASK_RESPONSE": "", 
    "TASK_TYPE": 1
  }, 
  {
    "BEACON_ID": "35395", 
    "HANDLE": true, 
    "TASK_DATA": "id", 
    "TASK_ID": "205e9cfe-5696-473c-9eca-e6eb2959ed37", 
    "TASK_RESPONSE": "uid=0(root) gid=0(root) groups=0(root) ", 
    "TASK_TYPE": "1"
  }, 
  {
    "BEACON_ID": "35395", 
    "HANDLE": true, 
    "TASK_DATA": "id", 
    "TASK_ID": "63c9dab5-0931-4e4c-a3c6-7b405574f00c", 
    "TASK_RESPONSE": "uid=0(root) gid=0(root) groups=0(root) ", 
    "TASK_TYPE": "1"
  }
]

```

## 优点


- HTTP协议通信（能够很方便的域前置）
- 纯HTTP接口控制

不足点：

- 暂无心跳更改功能
- 协议暂时纯明文


