## Sensor list
##### State:   ['z', 'angle', 'gear', 'trackPos', 'speedY', 'distRaced', 'speedZ', 'damage', 'wheelSpinVel', 'focus', 'track', 'curLapTime', 'speedX', 'racePos', 'fuel', 'distFromStart', 'opponents', 'rpm', 'lastLapTime']
* angle [-pi,+pi] (rad) Angle between the car direction and the direction of the track axis.
* curLapTime [0, inf) (s) Time elapsed during current lap.
* damage [0, +inf) (point) Current damage of the car (the higher is the value the higher is the damage).
* distFromStart [0,+inf) (m) Distance of the car from the start line along the track line.
* distRaced [0,+inf) (m) Distance covered by the car from the beginning of the race
* focus [0,200] (m) Vector of 5 range finder sensors: each sensor returns the distance between the track edge and the 
car within a range of 200 meters. When noisy option is enabled (see Section 7). Sensors are affected by i.i.d. normal 
noises with a standard deviation equal to the 1% of sensors range. The sensors sample, with a resolution of one degree, 
a five degree space along a specific direction provided by the client (the direction is defined with the focus command 
and must be in the range [-90,+90] degrees w.r.t. the car axis). Focus sensors are not always available: they can be 
used only once per second of simulated time. When the car is outside of the track (i.e., pos is less than -1 or greater 
than 1), the focus direction is outside the allowed range ([-90,+90] degrees) or the sensors has been already used once 
in the last second, the returned values are not reliable.
* fuel [0,+inf) (l) Current fuel level.
* gear {-1,0,1,2,3,4,5,6} Current gear: -1 is reverse, 0 is neutral and the gear from 1 to 6.
* lastLapTime [0,+inf) (s) Time to complete the last lap
* opponents [0,200] (m) Vector of 36 opponent sensors: each sensor covers a span of 10 degrees within a range of 200
meters and returns the distance of the closest opponent in the covered area. When noisy option is enabled (see Section 
7), sensors are affected by i.i.d. normal noises with a standard deviation equal to the 2% of sensors range. The 36 
sensors cover all the space around the car, spanning clockwise from -180 degrees up to +180 degrees with respect to the 
car axis.
* racePos {1,2, ... ,N} Position in the race with respect to other cars.
* rpm [0,+inf) (rpm) Number of rotation per minute of the car engine.
* speedX (-inf,+inf) (km/h) Speed of the car along the longitudinal axis of the car.
* speedY (-inf,+inf) (km/h) Speed of the car along the transverse axis of the car.
* speedZ (-inf,+inf) (km/h) Speed of the car along the Z axis of the car.
* track [0,200] (m) Vector of 19 range finder sensors: each sensors returns the distance between the track edge and the
car within a range of 200 meters. When noisy option is enabled (see Section 7), sensors are affected by i.i.d. normal 
noises with a standard deviation equal to the 10% of sensors range. By default, the sensors sample the space in front of 
the car every 10 degrees, spanning clockwise from -90 degrees up to +90 degrees with respect to the car axis. However, 
the con- figuration of the range finder sensors (i.e., the angle w.r.t. to the car axis) can be set by the client once 
during initialization, i.e., before the beginning of each race. When the car is outside of the track (i.e., pos is less 
than -1 or greater than 1), the returned values are not reliable (typically -1 is returned).
* trackPos (-inf,+inf) Distance between the car and the track axis. The value is normalized w.r.t to the track width: it
is 0 when car is on the axis, -1 when the car is on the right edge of the track and +1 when it is on the left edge of 
the car. Values greater than 1 or smaller than -1 mean that the car is outside of the track.
* wheelSpinVel [0,+inf] (rad/s) Vector of 4 sensors representing the rotation speed of wheels.
* z [-inf,+inf] (m) Distance of the car mass center from the surface of the track along the Z axis.

##### Response:  ['gear', 'clutch', 'focus', 'accel', 'meta', 'brake', 'steer']
* accel [0,1] Virtual gas pedal (0 means no gas, 1 full gas).
* brake [0,1] Virtual brake pedal (0 means no brake, 1 full brake).
* clutch [0,1] Virtual clutch pedal (0 means no clutch, 1 full clutch).
* gear -1,0,1,2,3,4,5,6 Gear value.
* steering [-1,1] Steering value: -1 and +1 means respectively full right and left, that corresponds to an angle of 
0.366519 rad.
* focus [-90,90] Focus direction (see the focus sensors in Table 1) in degrees.
* meta 0,1 This is meta-control command: 0 do nothing, 1 ask competition server to restart the race.