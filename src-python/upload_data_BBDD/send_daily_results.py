import datetime
import pandas as pd

from mysql.connector import MySQLConnection, Error
from python_mysql_dbconfig import read_db_config
from upload_file_to_storage import StorageRepository


def get_results_from_mysql():
    date_today = datetime.date.today()
    date_yesterday = date_today + datetime.timedelta(days=-1)
    print("Lanzando query entre los dias {0} y {1}".format(str(date_yesterday), str(date_today)))
    query = "SELECT * FROM esp32.esp32_thingsboard  WHERE read_date > %s AND read_date < %s"
    args = (date_yesterday, date_today)

    try:
        db_config = read_db_config()
        conn = MySQLConnection(**db_config)

        cursor = conn.cursor()
        cursor.execute(query, args)
        records = cursor.fetchall()
        print("Total number of rows is: ", cursor.rowcount)
        conn.commit()
    except Error as error:
        print("error")
        print(error)
        return "error in select statement"

    finally:
        cursor.close()
        conn.close()
    return records


def transform_to_dataframe(results):
    columns = ["id", "read_date", "peso", "humedad_suelo_INT", "humedad_suelo_EXT", "humedad_aire", "co2",
               "luminosidad", "temperatura", "agua_detectada", "rele"]
    df = pd.DataFrame(columns=columns)

    for row in results:
        df = df.append({"id": row[0], "read_date": row[1], "peso": row[2], "humedad_suelo_INT": row[3],
                        "humedad_suelo_EXT": row[4], "humedad_aire": row[5], "co2": row[6], "luminosidad": row[7],
                        "temperatura": row[8], "agua_detectada": row[9], "rele": row[10]}, ignore_index=True)
    df.to_csv("results_{}.csv".format(str(datetime.date.today()+datetime.timedelta(days=-1))), sep=';')


def main():
    storage = StorageRepository()
    results = get_results_from_mysql()
    transform_to_dataframe(results)
    storage.load()


if __name__ == '__main__':
    main()
