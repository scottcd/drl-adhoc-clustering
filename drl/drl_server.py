#!/usr/bin/env python
""" Deep Reinforced Learning TCP Server

A TCP server that handles client connections from OMNET++ and implements 
deep reinforced learning for agents that are connected. Once connected,
the server should received ___ which will be optimized for performance.
The server handles messages and maintains the connection until the 
client disconnects.
"""

__author__ = "Chandler Scott"
__contact__ = "scottcd1@etsu.edu"
__date__ = "04/02/2023"

import socket
import threading

HOST = '127.0.0.1'
PORT = 65432 

def listen_for_connections():
    """
    Listens and accepts connections from OMNET++ clients.
    Creates a thread to handle each client connection.
    """
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen()

    print(f'Listening for client connections on port {PORT}..')

    while True:
        conn, addr = server_socket.accept()
        client_thread = threading.Thread(target=handle_client, args=(conn, addr))
        client_thread.start()

def handle_client(conn, addr):
    """
    Handles client messages until the client closes the connection.
    """
    node_name = "undefined"
    greeting = True
    print(f'Connected by {addr}')
    # listen until the client closes the connection
    with conn:
        while True:
            # receive ___ from client
            data = conn.recv(1024)
            if not data:
                break
            if greeting:
                node_name = data.decode()
                print(f"Nice to meet you, {node_name}!")
                greeting = False
            
            #                               #
            #   Implement DRL logic here    #
            #                               #
            
            # send ___ to client
            print(f'Received: {data.decode()}. Replying with 6.')
            number = 6
            conn.sendall(str(number).encode())
            
    # connection closed by client
    print(f'Connection closed by {addr}')     
    

if __name__ == '__main__':
    listen_for_connections()
