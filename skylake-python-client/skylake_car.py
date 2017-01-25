import math
from control_steering import SteeringControl
from control_gear import GearControl

class SkylakeCar:
    def __init__(self, client):
        self.client = client
        self.steer_control = SteeringControl()
        self.gear_control = GearControl()
        self.steer_lock = 0.366519

    def drive(self):
        self.client.get_servers_input()
        self.logic_cycle()
        self.client.respond_to_server()

    def logic_cycle(self):
        '''This is only an example. It will get around the track but the
            correct thing to do is write your own `drive()` function.'''
        S = self.client.S.d
        R = self.client.R.d
        target_speed = 200

        # Damage Control
        target_speed -= S['damage'] * .05
        if target_speed < 25:
            target_speed = 25

        # Steer To Corner
        angle = S['angle'] - self.steer_control.control(S['trackPos'])
        R['steer'] = angle / self.steer_lock
        R['steer'] = clip(R['steer'], -1, 1)

        # Throttle Control
        if S['speedX'] < target_speed - (R['steer'] * 50):
            R['accel'] += .01
        else:
            R['accel'] -= .01
        if S['speedX'] < 10:
            R['accel'] += 1 / (S['speedX'] + .1)

        # Traction Control System
        if ((S['wheelSpinVel'][2] + S['wheelSpinVel'][3]) -
                (S['wheelSpinVel'][0] + S['wheelSpinVel'][1]) > 5):
            R['accel'] -= .2
        R['accel'] = clip(R['accel'], 0, 1)

        # Automatic Transmission
        R['gear'] = self.gear_control.control(int(S['gear']), S['rpm'])

def clip(v, lo, hi):
    if v < lo:
        return lo
    elif v > hi:
        return hi
    else:
        return v