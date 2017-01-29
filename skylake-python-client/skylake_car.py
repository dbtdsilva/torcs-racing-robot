import sys
import math
from skylake_consts import SkylakeCarConsts
from control_steering import SteeringControl
from control_gear import GearControl

class SkylakeCar:
    def __init__(self, client):
        self.client = client
        self.steer_control = SteeringControl()
        self.gear_control = GearControl()
        self.angle = 0
        self.position = (0, 0)
        self.dist_raced = 0

    def drive(self):
        self.client.get_servers_input()
        self.logic_cycle()
        self.client.respond_to_server()

    def convert_wheel_speed_to_distance(self, speed):
        distance_second = (speed * SkylakeCarConsts.TIRE_PERIMETER_M) / (2.0 * math.pi)
        distance_simulation_step = SkylakeCarConsts.TORCS_SIMULATION_STEP * distance_second
        return distance_simulation_step

    def logic_cycle(self):
        '''This is only an example. It will get around the track but the
            correct thing to do is write your own `drive()` function.'''
        S = self.client.S.d
        R = self.client.R.d
        target_speed = 150

        # Damage Control
        target_speed -= S['damage'] * .05
        if target_speed < 25:
            target_speed = 25

        # Steer To Corner
        angle = S['angle'] - self.steer_control.control(S['trackPos'])
        raced_between_ticks = S['distRaced'] - self.dist_raced

        # 2pi rad ->
        #print raced_between_ticks


        self.dist_raced = S['distRaced']



        R['steer'] = angle / SkylakeCarConsts.STEER_LOCK
        R['steer'] = clip(R['steer'], -1, 1)

        #dcenter = (S['wheelSpinVel'][1] + S['wheelSpinVel'][0]) / 2.0
        #(S['wheelSpinVel'][0] - S['wheelSpinVel'][1]) / 1.94

        distance_travelled = S['speedX'] * 0.02


        distance_left_wheel = self.convert_wheel_speed_to_distance(S['wheelSpinVel'][1])
        distance_right_wheel = self.convert_wheel_speed_to_distance(S['wheelSpinVel'][0])
        distance = (distance_left_wheel + distance_right_wheel) / 2.0



        self.position = (self.position[0] + distance * math.cos(self.angle),
                         self.position[1] + distance * math.sin(self.angle))
        self.angle += (distance_right_wheel - distance_left_wheel) / SkylakeCarConsts.WHEELS_DISTANCE_FRONT_M

        print "%6.4f %6.4f %7.3f %9.3f %9.3f" % (raced_between_ticks, distance,
                                                 (self.angle * 180.0) / math.pi,
                                                 self.position[0], self.position[1])
        # Throttle Control
        if S['speedX'] < 120:  # abs(S['trackPos']) < 0.1 or
            R['accel'] += .5
        else:
            R['accel'] -= .5

        # Traction Control System
        if ((S['wheelSpinVel'][2] + S['wheelSpinVel'][3]) - (S['wheelSpinVel'][0] + S['wheelSpinVel'][1]) > 5):
            R['accel'] -= .2
        R['accel'] = clip(R['accel'], 0, 1)

        # Automatic Transmission
        R['gear'] = self.gear_control.control(int(S['gear']), S['rpm'])

        # for x in S['track']:
        #    sys.stdout.write("%7.1f," % x)
        # sys.stdout.write("\n")


def clip(v, lo, hi):
    if v < lo:
        return lo
    elif v > hi:
        return hi
    else:
        return v
