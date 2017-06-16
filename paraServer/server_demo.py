#!/usr/bin/env python

from flask import Flask
from flask_sockets import Sockets
import json
from random import randint

app = Flask(__name__)
sockets = Sockets(app)
name_ws_dict = {}


class User:
    def __init__(self, id, ws, pub_d, pub_n, p_x, p_y):
        self.ws = ws
        self.id = id
        self.pub_d = pub_d
        self.pub_n = pub_n
        self.p_x = p_x
        self.p_y = p_y


@sockets.route('/echo')
def echo_socket(ws):
    while not ws.closed:
        message = ws.receive()
        if message is None:
            remove_ws(ws)
            continue
        print("message:", message)
        parsed_msg = json.loads(message)
        sender = parsed_msg['id']
        action = parsed_msg['action']
        msg = parsed_msg['msg']
        if action == 'login':
            action_login(sender, msg, ws)
        elif action == 'move':
            action_move(sender, msg, ws)


def remove_ws(ws):
    key_to_remove = None
    for (k, v) in name_ws_dict.items():
        if v.ws == ws:
            print(k, " closed!")
            key_to_remove = k
    if key_to_remove is not None:
        del name_ws_dict[key_to_remove]


def get_init_pos():
    flag = True
    x, y = randint(0, 9), randint(0, 6)
    while flag:
        flag = False
        x, y = randint(0, 9), randint(0, 6)
        for (k, user) in name_ws_dict.items():
            if x == user.p_x and y == user.p_y:
                flag = True
                break
    return x, y


def action_login(sender, msg, ws):
    x, y = get_init_pos()
    name_ws_dict[sender] = User(sender, ws, msg['d'], msg['n'], x, y)
    jd = {'action': 'create', 'msg': {}}
    jd['msg']['id'] = sender
    jd['msg']['x'], jd['msg']['y'] = x, y
    jd['msg']['d'], jd['msg']['n'] = msg['d'], msg['n']
    send_broadcast(json.dumps(jd))


def action_move(sender, msg, ws):
    jd = {'action': 'move', 'msg': {}}
    jd['msg']['id'] = sender
    jd['msg']['x'], jd['msg']['y'] = msg['x'], msg['y']
    send_broadcast(json.dumps(jd))


def send_broadcast(msg):
    for (name, user) in name_ws_dict.items():
        user.ws.send(msg)


@app.route('/')
def hello():
    return 'Hello World!'


if __name__ == "__main__":
    from gevent import pywsgi
    from geventwebsocket.handler import WebSocketHandler

    server = pywsgi.WSGIServer(('', 7000), app, handler_class=WebSocketHandler)
    server.serve_forever()
