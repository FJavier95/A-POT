import smbus2 as smbus
import time


class TemperaturaHumedad:
    def __init__(self):
        self.bus = smbus.SMBus(4)
        self.addr = 0x44

    def activate_sensor(self):
        self.bus.write_i2c_block_data(self.addr, 0x2c, [0x06])
        time.sleep(0.5)
        data = self.bus.read_i2c_block_data(self.addr, 0x00, 6)
        return data

    def get_temperature(self):
        data = self.activate_sensor()
        temp_raw = (data[0] * 256) + data[1]
        temp = -45 + (175 * temp_raw / (2 ** 16 - 1))
        return temp

    def get_humidity(self):
        data = self.activate_sensor()
        hum_raw = (data[3] * 256) + data[4]
        hum = 100 * (hum_raw / (2 ** 16 - 1))
        return hum
