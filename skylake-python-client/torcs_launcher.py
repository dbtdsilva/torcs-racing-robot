import os
import time

class TorcsLauncher:
    def __init__(self):
        self.number_tries = 0
        self.instance_launched = False

    def launch(self):
        if self.instance_launched:
            return

        self.kill_existing_instances()
        os.system(u'torcs -nofuel -nodamage -nolaptime 2>/dev/null &')
        time.sleep(0.1)
        os.system(u'xte "key Return"')
        os.system(u'xte "key Return"')
        os.system(u'xte "key Return"')
        os.system(u'xte "key Return"')

        print 'Torcs launched successfully!'


    def kill_existing_instances(self):
        os.system(u'pkill torcs')