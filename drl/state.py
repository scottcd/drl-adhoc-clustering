import math
import torch

class State():
    def __init__(self, src_node=None, dest_node=None):
        self.src_node = src_node
        self.dest_node = dest_node    
        
    def to_tensor(self):
        return torch.tensor([[self.src_node.position[0],
                             self.src_node.position[1],
                             self.src_node.velocity[0],
                             self.src_node.velocity[1],
                             self.dest_node.position[0],
                             self.dest_node.position[1],
                             self.dest_node.velocity[0],
                             self.dest_node.velocity[1]]],
                             dtype=torch.float32)

    def __str__(self):
        string = f''
        string += f'-- Source Node --\n'
        string += f'{self.src_node}'
        string += f'-- Destination Node --\n'
        string += f'{self.dest_node}'
        return string