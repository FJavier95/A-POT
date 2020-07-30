from mysql.connector import MySQLConnection, Error
from python_mysql_dbconfig import read_db_config
import read_data_from_TB as read_data


def insert_values(read_date, peso, humedad_suelo_INT, humedad_suelo_EXT, humedad_aire, co2, luminosidad, temperatura,
                  agua_detectada, rele):
    query = "INSERT INTO esp32.esp32_thingsboard(read_date, peso, humedad_suelo_INT, humedad_suelo_EXT, humedad_aire, co2, luminosidad, temperatura, agua_detectada, rele) " \
            "VALUES(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)"
    args = (
        read_date, peso, humedad_suelo_INT, humedad_suelo_EXT, humedad_aire, co2, luminosidad, temperatura,
        agua_detectada, rele)

    try:
        db_config = read_db_config()
        conn = MySQLConnection(**db_config)

        cursor = conn.cursor()
        cursor.execute(query, args)

        if cursor.lastrowid:
            print('last insert id', cursor.lastrowid)
        else:
            print('last insert id not found')

        conn.commit()
    except Error as error:
        print("error")
        print(error)

    finally:
        cursor.close()
        conn.close()


def main():
    json = read_data.read_data_from_thingsboard()
    data = read_data.return_data(json)
    insert_values(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9])


if __name__ == '__main__':
    main()
