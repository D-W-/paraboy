#!/usr/bin/env python

from flask import Flask
from flask_sockets import Sockets

app = Flask(__name__)
sockets = Sockets(app)

name_ws_dict = {}


@sockets.route('/echo')
def echo_socket(ws):
    while not ws.closed:
        message = ws.receive()
        if message == None:
            key_to_remove = None
            for (k,v) in name_ws_dict.items():
                if v == ws:
                    print(k," closed!")
                    key_to_remove = k
            if key_to_remove != None:
                del name_ws_dict[key_to_remove]
            continue
        print("message:",message)
        if "name:" in message:
            name_ws_dict[message[5:]] = ws
            print("add client ", message[5:])
        if "broadcast:" in message:
            send_broadcast(message)
        ws.send(message)


def send_broadcast(msg):
    for (name, ws) in name_ws_dict.items():
        ws.send(msg)


@app.route('/')
def hello():
    return 'Hello World!'


if __name__ == "__main__":
    from gevent import pywsgi
    from geventwebsocket.handler import WebSocketHandler

    server = pywsgi.WSGIServer(('', 7000), app, handler_class=WebSocketHandler)
    server.serve_forever()
