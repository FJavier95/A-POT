import smbus2 as smbus
import time


class Luminosidad:
    def __init__(self):
        self.bus = smbus.SMBus(4)
        self.addr = 0x10
        self.als_conf_0 = 0x00
        self.als_WH = 0x01
        self.als_WL = 0x02
        self.pow_sav = 0x03
        self.als = 0x04
        self.white = 0x05
        self.interrupt = 0x06
        self.confValues = [0x00, 0x13] # 1/8 gain, 25ms IT (Integration Time)
        self.interrupt_high = [0x00, 0x00]  # Clear values
        self.interrupt_low = [0x00, 0x00]  # Clear values
        self.power_save_mode = [0x00, 0x00]  # Clear values

    def read_value(self):
        self.bus.write_i2c_block_data(self.addr, self.als_conf_0, self.confValues)
        self.bus.write_i2c_block_data(self.addr, self.als_WH, self.interrupt_high)
        self.bus.write_i2c_block_data(self.addr, self.als_WL, self.interrupt_low)
        self.bus.write_i2c_block_data(self.addr, self.pow_sav, self.power_save_mode)
        time.sleep(.04)  # 40ms
        word = self.bus.read_word_data(self.addr, self.als)
        gain = 1.8432  # Gain for 1/8 gain & 25ms IT
        val = word * gain
        val = round(val, 1)
        return val
