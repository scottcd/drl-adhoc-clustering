class Node():
    def __init__(self, xPosCoord, yPosCoord, xVelCoord, yVelCoord):
        self.position = (float(xPosCoord), float(yPosCoord))
        self.velocity = (float(xVelCoord), float(yVelCoord))

    def __str__(self):
        string = ''
        string += f'Position: ({self.position[0]}, {self.position[1]})\n'
        string += f'Velocity: ({self.velocity[0]}, {self.velocity[1]})\n'
        return string