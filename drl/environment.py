import numpy as np
import torch
from state import State
from node import Node

class Environment():
    def __init__(self):
        self.action_space = torch.tensor([0,1])
        self.state = None
    
    def percept(self, values):
        """receive state from client"""
        # parse values into nodes
        src_node = Node(self.parse_value(values[1]), self.parse_value(values[2]), 
                    self.parse_value(values[5]), self.parse_value(values[6]))
        dest_node = Node(self.parse_value(values[9]), self.parse_value(values[10]), 
                    self.parse_value(values[13]), self.parse_value(values[14]))
        self.state = State(src_node, dest_node)
        return self.state

    def parse_value(self, value):
        """parse value from PING request"""
        return ''.join(filter(
            lambda x: x.isdigit() or 
            x == '.', value)
        )

    def start_step(self, conn, action, waiting_for_reply):
        """  """
        print(f'Sending score to client: {action.item()}.')
        conn.sendall(str(action.item()).encode())
        if action.item() == 1:
            waiting_for_reply = True
        else:
            print("Score too low - not pinging.")
        return waiting_for_reply
        
    def finish_step(self, dropped=False, not_sent=False):
        if dropped is True:
            reward = -1
        if not_sent is True:
            reward = 0
        else:
            reward = 1 
        return self.state, reward
    
    def __str__(self):
        string = f'{self.state}'
        return string