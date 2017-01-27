class GearControl:
    def __init__(self):
        #self.rpm_up = [5000, 6000, 6000, 6500, 7000, 0]
        self.rpm_up = [7000, 7000, 7000, 7000, 7000, 0]
        self.rpm_down = [0, 2500, 3000, 3000, 3500, 3500]

    def control(self, gear, rpm):
        if gear < 1:
            gear = 1
        if gear < 6 and rpm >= self.rpm_up[gear - 1]:
            return gear + 1

        if gear > 1 and rpm <= self.rpm_down[gear - 1]:
            return gear - 1
        return gear