class SteeringControl:
    def __init__(self):
        self.KP = 0.5
        self.KD = 0.0
        self.KI = 0.0
        self.last_error = 0
        self.integral_error = 0

    def control(self, received):
        error = received
        self.integral_error += error
        derivative_error = error - self.last_error

        self.last_error = error
        return self.KP * error + self.KD * derivative_error + self.KI * self.integral_error

