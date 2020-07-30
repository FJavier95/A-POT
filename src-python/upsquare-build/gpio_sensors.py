import mraa
import time


class GPIO:
    def __init__(self):
        # self.pin_peso =
        # self.pin_rele =
        self.pin_RED = 37
        self.pin_GREEN = 31
        self.pin_BLUE = 29

    def activate_evalv(self):
        pin = mraa.Gpio(self.pin_rele)
        pin.dir(mraa.DIR_OUT)
        pin.write(1)
        return "Valvula ABIERTA"

    def deactivate_evalv(self):
        pin = mraa.Gpio(self.pin_rele)
        pin.dir(mraa.DIR_OUT)
        pin.write(1)
        return "Valvula CERRADA"

    def change_color(self, value):
        pin_R = mraa.Gpio(self.pin_RED)
        pin_B = mraa.Gpio(self.pin_BLUE)
        pin_G = mraa.Gpio(self.pin_GREEN)
        pin_R.dir(mraa.DIR_OUT)
        pin_B.dir(mraa.DIR_OUT)
        pin_G.dir(mraa.DIR_OUT)
        if value < 200:
            pin_G.write(1)
            # pin_B.write(1)
        else:
            # pin_B.write(0)
            pin_R.write(1)


if __name__=="__main__":
    gp = GPIO()
    gp.change_color(400)
    time.sleep(1)
    gp.change_color(800)
    time.sleep(1)
    gp.change_color(400)
    time.sleep(1)
    gp.change_color(700)
    time.sleep(1)
    gp.change_color(300)
