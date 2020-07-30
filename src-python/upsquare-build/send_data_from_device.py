import requests
import time

from ADC import ADCSensor
from i2c_luminosidad import Luminosidad
from temperatura import TemperaturaHumedad
from gpio_sensors import GPIO


class SendData:
    def __init__(self):
        self.access_token_luminosity = "3ROwGzCQ8v9SzgnmTUWs"
        self.access_token_temperature = "HTixCj5AdNcXS6FKu1SR"
        self.access_air_token_humidity = "YbUYkjq04yiO2rrIebgt"
        self.access_token_humidity_int = "fm1x1t4jwglf2CaFpYPn"
        self.access_token_humidity_ext = "PEGWEsgFwahKBo4ul3B2"
        self.access_token_water_detector = "oJTVCGogPbGTOCclDkch"
        self.access_token_weight_detector = ""
        self.adc = ADCSensor()
        self.luminosidad = Luminosidad()
        self.temperatura = TemperaturaHumedad()
        self.gpio = GPIO()

    def send_data(self, data, access_token):
        url = 'https://demo.thingsboard.io/api/v1/' + access_token + '/telemetry'
        payload = "{\n    \"valor actual:\": \"%s\"\n}" % data
        headers = {
            'Content-Type': "application/json",
            'User-Agent': "PostmanRuntime/7.20.1",
            'Accept': "*/*",
            'Cache-Control': "no-cache",
            'Postman-Token': "1ff5f5d9-439e-4ed2-897b-934e618a1696,5b369e7e-a05e-4b58-bf33-72d1db3c3477",
            'Host': "demo.thingsboard.io",
            'Accept-Encoding': "gzip, deflate",
            'Content-Length': "28",
            'Connection': "keep-alive",
            'cache-control': "no-cache"
        }

        r = requests.post(url=url, data=payload, headers=headers)
        if r.status_code == 200:
            print("information sent correctly")
        elif r.status_code == 401:
            print(r.text)
            print("Unauthorized request")
        else:
            print(r.status_code)
            print("Another error has occurred")

    def setup(self):
        self.send_data(self.temperatura.get_temperature(), self.access_token_temperature)
        self.send_data(self.temperatura.get_humidity(), self.access_air_token_humidity)
        self.send_data(self.adc.read_detectar_humedad_int(), self.access_token_humidity_int)
        self.send_data(self.adc.read_detectar_humedad_ext(), self.access_token_humidity_ext)
        valor_agua = self.adc.read_detectar_agua()
        self.send_data(valor_agua, self.access_token_water_detector)
        self.gpio.change_color(valor_agua)
        self.send_data(self.luminosidad.read_value(), self.access_token_luminosity)
        # self.send_data(self.adc.read_peso_motor(), self.access_token_weight_detector)
        # time.sleep(1)  #Se leen datos cada 5 segundos


if __name__ == '__main__':
    send = SendData()
    while True:
        send.setup()
