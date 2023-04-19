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
import random
import torch
from state import State
from node import Node
from environment import Environment
from dqn import DQN
from replay_memory import ReplayMemory
import math
import torch.optim as optim
import torch.nn as nn
from collections import namedtuple



HOST = '127.0.0.1'
PORT = 65432 
BATCH_SIZE = 128
GAMMA = 0.99
EPS_START = 0.9
EPS_END = 0.05
EPS_DECAY = 1000
TAU = 0.005
LR = 1e-4

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
    num = 0

    while True:
        num += 1
        conn, addr = server_socket.accept()
        client_thread = threading.Thread(target=handle_client, args=(conn, addr, num))
        client_thread.start()

def handle_client(conn, addr, connection_number):
    """
    Handles client messages until the client closes the connection.
    """
    print(f'Connected by {addr}')
    waiting_for_reply = False
    node_name = 'undefined'
    steps_done = 0
    state = State()
    env = Environment()
    policy_net = DQN(8, 2)
    target_net = DQN(8, 2)
    target_net.load_state_dict(policy_net.state_dict())
    optimizer = optim.AdamW(policy_net.parameters(), lr=LR, amsgrad=True)
    memory = ReplayMemory(10000)

    # listen until the client closes the connection
    with conn:
        while True:
            # receive data from client
            print('waiting for data')
            data = conn.recv(1024).decode()
            if not data:
                break
            print(f'got "{data}" here')
            values = data.split()
            
            if values[0] == "PING":
                steps_done += 1
                if waiting_for_reply is True:
                    print("Dropped a ping.")
                    handle_state_change(env, state, memory, action, policy_net, target_net, optimizer, dropped=True)
                    waiting_for_reply = False

                # perceive the environment to get current state
                state = env.percept(values)
            
                print('\n-----------------------------')
                print(f'    {node_name} - State #{steps_done}')
                print('-----------------------------')
                print(state)
                
                action = select_action(env, state, policy_net, steps_done)
                waiting_for_reply = env.start_step(conn, action, waiting_for_reply)

                # if we tell the node to not ping, 
                # we will not rcv from socket.
                # So, we need to handle the rest
                # of the training loop here.
                if action.item() == 0:
                    handle_state_change(env, state, memory, action, policy_net, target_net, optimizer, not_sent=True)


            elif values[0] == "REPLY":
                print(data)
                handle_state_change(env, state, memory, action, policy_net, target_net, optimizer)
                waiting_for_reply = False
            else:
                node_name = values[0]
            
    # connection closed by client
    print(f'Connection closed by {addr}')  
    # output learned policies to file
    print(policy_net(state.to_tensor()).max(0))
    torch.save(policy_net.state_dict(), f'policies/{connection_number}_policy_net.pth')
    # policy_net.load_state_dict(torch.load('policy_net.pth'))

    
    
def select_action(env, state, policy_net, steps_done):
    sample = random.random()
    eps_threshold = EPS_END + (EPS_START - EPS_END) * \
        math.exp(-1. * steps_done / EPS_DECAY)
    steps_done += 1
    if sample > eps_threshold:
        with torch.no_grad():
            # t.max(1) will return the largest column value of each row.
            # second column on max result is index of where max element was
            # found, so we pick action with the larger expected reward.
            return policy_net(state.to_tensor()).max(1)[1].view(1, 1)
    else:
        return torch.tensor([env.action_space[random.randint(0, len(env.action_space)-1)]], 
                            dtype=torch.long).view(1,1)

def handle_state_change(env, state, memory, action, policy_net, target_net, optimizer, dropped=False, not_sent=False):
    """"""
    observation, reward = env.finish_step(dropped, not_sent)
    reward = torch.tensor([reward])
    next_state = observation.to_tensor().clone().detach().unsqueeze(0).float()

    # store the transition into memory
    memory.push(state.to_tensor(), action, next_state, reward)

    # move to the next state
    state = next_state
    
    # optimize the model
    optimize_model(memory, policy_net, target_net, optimizer)
    
    # Soft update the target network's weights
    target_net_state_dict = target_net.state_dict()
    policy_net_state_dict = policy_net.state_dict()
    for key in policy_net_state_dict:
        target_net_state_dict[key] = policy_net_state_dict[key]*TAU + target_net_state_dict[key]*(1-TAU)
    target_net.load_state_dict(target_net_state_dict)
  
def optimize_model(memory, policy_net, target_net, optimizer):
    """Optimize the model""" 
    Transition = namedtuple('Transition',
                        ('state', 'action', 'next_state', 'reward'))
    
    if len(memory) < BATCH_SIZE:
        print(f'{len(memory)} {BATCH_SIZE}')
        print("Batch not big enough to optimize model. Returning.")
        return
    print("Optimizing model.")
    transitions = memory.sample(BATCH_SIZE)
    # Transpose the batch (see https://stackoverflow.com/a/19343/3343043 for
    # detailed explanation). This converts batch-array of Transitions
    # to Transition of batch-arrays.
    batch = Transition(*zip(*transitions))

    # Compute a mask of non-final states and concatenate the batch elements
    # (a final state would've been the one after which simulation ended)
    non_final_mask = torch.tensor(tuple(map(lambda s: s is not None,
                                          batch.next_state)), dtype=torch.bool)
    non_final_next_states = torch.cat([s for s in batch.next_state
                                                if s is not None])
    
    state_batch = torch.cat(batch.state)    
    reward_batch = torch.cat(batch.reward)
    action_batch = torch.cat(batch.action)

    # Compute Q(s_t, a) - the model computes Q(s_t), then we select the
    # columns of actions taken. These are the actions which would've been taken
    # for each batch state according to policy_net
    state_action_values = policy_net(state_batch).gather(1, action_batch)

    # Compute V(s_{t+1}) for all next states.
    # Expected values of actions for non_final_next_states are computed based
    # on the "older" target_net; selecting their best reward with max(1)[0].
    # This is merged based on the mask, such that we'll have either the expected
    # state value or 0 in case the state was final.
    next_state_values = torch.zeros(BATCH_SIZE, 2)
    with torch.no_grad():
        #next_state_values[non_final_mask] = target_net(non_final_next_states).max(1)[0]
        next_state_values[non_final_mask, :] = target_net(non_final_next_states).max(1)[0].to()



    # Compute the expected Q values
    expected_state_action_values = (next_state_values * GAMMA) + reward_batch.unsqueeze(1)

    # Compute Huber loss
    criterion = nn.SmoothL1Loss()
    loss = criterion(state_action_values, expected_state_action_values)

    # Optimize the model
    optimizer.zero_grad()
    loss.backward()
    # In-place gradient clipping
    torch.nn.utils.clip_grad_value_(policy_net.parameters(), 100)
    optimizer.step() 

if __name__ == '__main__':
    listen_for_connections()
