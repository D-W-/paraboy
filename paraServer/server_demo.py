#!/usr/bin/env python

from flask import Flask
from flask_sockets import Sockets
import json
from random import randint

app = Flask(__name__)
sockets = Sockets(app)
name_ws_dict = {}
name_list = []


class User:
    def __init__(self, id, ws, pub_d, pub_n, p_x, p_y, b_x, b_y):
        self.ws = ws
        self.id = id
        self.pub_d = pub_d
        self.pub_n = pub_n
        self.p_x = p_x
        self.p_y = p_y
        self.b_x = b_x
        self.b_y = b_y


@sockets.route('/echo')
def echo_socket(ws):
    while not ws.closed:
        message = ws.receive()
        if message is None:
            remove_ws(ws)
            continue
        print("message:", message)
        try:
            parsed_msg = json.loads(message)
        except ValueError:
            print("this message is not a json string, skipped!")
            continue

        sender = parsed_msg['id']
        action = parsed_msg['action']
        msg = parsed_msg['msg']
        if action == 'login':
            action_login(sender, msg, ws)
        elif action == 'move':
            action_move(sender, msg, ws)
        elif action == 'auth':
            action_auth(sender, msg, ws)
        elif action == 'auth2':
            action_auth2(sender, msg, ws)


def remove_ws(ws):
    key_to_remove = None
    for (k, v) in name_ws_dict.items():
        if v.ws == ws:
            print(k, " closed!")
            key_to_remove = k
    if key_to_remove is not None:
        del name_ws_dict[key_to_remove]
        name_list.remove(key_to_remove)


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


def get_box_secret():
    return 1, 1  # add code later...


def action_login(sender, msg, ws):
    user_list = []
    for name in name_list:
        t = name_ws_dict[name]
        user_dic = {"id": t.id, "x": t.p_x, "y": t.p_y, "d": t.pub_d, "n": t.pub_n, "bx": t.b_x, "by": t.b_y}
        # user_dic['id'] = name_ws_dict[name].id
        # user_dic['p_x'] = name_ws_dict[name].p_x
        # user_dic['p_y'] = name_ws_dict[name].p_y
        # user_dic['pub_d'] = name_ws_dict[name].pub_d
        # user_dic['pub_n'] = name_ws_dict[name].pub_n
        # user_dic['b_x'] = name_ws_dict[name].b_x
        # user_dic['b_y'] = name_ws_dict[name].b_y
        user_list.append(user_dic)

    tmp_jd = {'action': 'login2', 'msg': {}}
    tmp_jd["msg"]["users"] = user_list
    send_target(sender, json.dumps(tmp_jd))

    x, y = get_init_pos()
    bx, by = get_box_secret()
    name_list.append(sender)
    name_ws_dict[sender] = User(sender, ws, msg['d'], msg['n'], x, y, bx, by)
    jd = {'action': 'create', 'msg': {}}
    jd['msg']['id'] = sender
    jd['msg']['x'], jd['msg']['y'] = x, y
    jd['msg']['d'], jd['msg']['n'] = msg['d'], msg['n']
    jd['msg']['bx'], jd['msg']['by'] = bx, by
    send_broadcast(json.dumps(jd))


def action_move(sender, msg, ws):
    jd = {'action': 'move', 'msg': {}}
    jd['msg']['id'] = sender
    jd['msg']['x'], jd['msg']['y'] = msg['x'], msg['y']
    send_broadcast(json.dumps(jd))


def action_auth(sender, msg, ws):
    jd = {'action': 'auth', 'msg': msg}
    jd['msg']['source'] = sender
    send_target(json.dumps(jd), msg['target'])


def action_auth2(sender, msg, ws):
    jd = {'action': 'auth2', 'msg': msg}
    jd['msg']['source'] = sender
    send_target(json.dumps(jd), msg['target'])


def send_broadcast(msg):
    for (name, user) in name_ws_dict.items():
        user.ws.send(msg)


def send_target(msg, target):
    if target in name_ws_dict.keys():
        name_ws_dict[target].ws.send(msg)


@app.route('/')
def hello():
    return 'Hello World!'


if __name__ == "__main__":
    from gevent import pywsgi
    from geventwebsocket.handler import WebSocketHandler

    server = pywsgi.WSGIServer(('', 7000), app, handler_class=WebSocketHandler)
    server.serve_forever()
