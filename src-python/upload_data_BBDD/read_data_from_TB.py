# coding=utf-8

import requests
from configparser import ConfigParser
import json
from datetime import datetime


def timestamp_to_date(timestamp):
    dt_object = datetime.fromtimestamp(float(timestamp) / 1000.0)
    return dt_object


def read_credentials(filename='../../config.ini', section='thingsboard'):
    parser = ConfigParser()
    parser.read(filename)

    thingsboard = {}
    if parser.has_section(section):
        items = parser.items(section)
        for item in items:
            thingsboard[item[0]] = item[1]
    else:
        raise Exception('{0} not found in the {1} file'.format(section, filename))

    return thingsboard


def read_data_from_thingsboard():
    thingsboard_credentials = read_credentials()
    base = "https://demo.thingsboard.io/api/plugins/telemetry/DEVICE/"
    access_token = thingsboard_credentials.get('access_token')
    endpoint = "/values/timeseries"
    url = base + access_token + endpoint

    headers = {
        'x-authorization': "{}".format(thingsboard_credentials.get("jwt_token")),
        'content-type': "application/json",
    }

    r = requests.get(url=url, headers=headers)
    print(r.status_code)
    parsed = json.loads(r.content.decode('utf-8'))
    print(parsed)
    print()
    return parsed


def return_data(json):
    data = []
    read_date = timestamp_to_date(json.get("agua_detectada")[0].get('ts'))
    peso = json.get("peso")[0].get('value')
    humedad_suelo_INT = json.get("humedad_suelo_INT")[0].get('value')
    humedad_suelo_EXT = json.get("humedad_suelo_EXT")[0].get('value')
    humedad_aire = json.get("humedad_aire")[0].get('value')
    co2 = json.get("co2")[0].get('value')
    luminosidad = json.get("luminosidad")[0].get('value')
    temperatura = json.get("temperatura")[0].get('value')
    agua_detectada = json.get("agua_detectada")[0].get('value')
    rele = json.get("rele")[0].get("value")
    data = [read_date, peso, humedad_suelo_INT, humedad_suelo_EXT, humedad_aire, co2, luminosidad, temperatura, agua_detectada, rele]
    return data


if __name__ == "__main__":
    json = read_data_from_thingsboard()
    print(json.get("agua_detectada")[0].get('ts'))

