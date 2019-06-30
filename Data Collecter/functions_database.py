# -*- coding: utf-8 -*-
import traceback, sqlite3
import numpy as np

def database_read_gps_data(database_cursor, table_name):
    try:
        sql_sentence = str('SELECT cpu_time, gps_latitude, gps_longitude, gps_time, gps_number_of_satellites, gps_quality,'
                           ' gps_heading, gps_velocity FROM ' + table_name + ' WHERE gps_latitude IS NOT NULL '
                           ' AND gps_velocity IS NOT NULL ORDER BY cpu_time')
        print('GPS read sql_sentence=' , sql_sentence)
        numpy_dtype = [('cpu_time', 'datetime64[s]'), ('gps_latitude', 'float64'), ('gps_longitude', 'float64'), ('gps_time', 'O'),
                       ('gps_number_of_satellites', 'int'), ('gps_quality', 'int'), ('gps_heading', 'float64'), ('gps_velocity', 'float64')]
        database_cursor.execute(sql_sentence)
        database_records = np.array(database_cursor.fetchall(), dtype=numpy_dtype)
        return database_records
    except:
        print('***************************************')
        print('database_read_gps_differential error : ', str(traceback.format_exc()))
        print('***************************************')
        return False


def database_read_sensors_data(database_cursor, table_name):
    try:
        sql_sentence = str('SELECT cpu_time, sensor_id, sensor_counter, gps_latitude, gps_longitude, gps_time, gps_number_of_satellites,' 
                           ' gps_heading, gps_velocity, imu_acceleration_x, imu_acceleration_y, imu_acceleration_z, imu_gyro_x, imu_gyro_y,'
                           ' imu_gyro_z, gps_quality, gps_hdop FROM ' + table_name + ' ORDER BY cpu_time ') # LIMIT 50
        print('Sensors read sql_sentence=', sql_sentence)
        numpy_dtype = [('cpu_time', 'datetime64[ms]'), ('sensor_id', 'int'), ('sensor_counter', 'int'),
                       ('gps_latitude', 'float64'), ('gps_longitude', 'float64'), ('gps_time', 'O'),
                       ('gps_number_of_satellites', 'int'), ('gps_heading', 'float64'), ('gps_velocity', 'float64'),
                       ('imu_acceleration_x', 'float64'), ('imu_acceleration_y', 'float64'), ('imu_acceleration_z', 'float64'),
                       ('imu_gyro_x', 'float64'), ('imu_gyro_y', 'float64'), ('imu_gyro_z', 'float64'),
                       ('gps_quality', 'float64'), ('gps_hdop', 'float64')]
        database_cursor.execute(sql_sentence)
        database_records = np.array(database_cursor.fetchall(), dtype=numpy_dtype)
        return database_records
    except:
        print('***************************************')
        print('database_read_gps_differential error : ', str(traceback.format_exc()))
        print('***************************************')
        return False

def database_insert_gps_differential(database_connection, database_cursor, table_name, database_records):
    try:
        database_cursor.executemany("""INSERT INTO {table_name} VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)""".format(table_name = table_name), database_records)
        database_connection.commit()
        return True
    except:
        print('***************************************')
        print('database_insert_sensors error : ', str(traceback.format_exc()))
        print('***************************************')
        return False

def database_insert_sensors(database_connection, database_cursor, table_name, database_records):
    try:
        database_cursor.executemany("""INSERT INTO {table_name} VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)""".format(table_name = table_name), database_records)
        database_connection.commit()
        return True
    except:
        print('***************************************')
        print('database_insert_sensors error : ', str(traceback.format_exc()))
        print('***************************************')
        return False


def database_close(database_connection):
    try:
        database_connection.close()
        return True
    except:
        print('***************************************')
        print('database_close error : ', str(traceback.format_exc()))
        print('***************************************')
        return False

def database_connect(database_name):
    try:
        database_connection = sqlite3.connect(database_name)
        database_cursor = database_connection.cursor()
        return (database_connection, database_cursor)
    except:
        print('***************************************')
        print('database_connect error : ', str(traceback.format_exc()))
        print('***************************************')
        return (None, None)

def database_create_table_differential(database_cursor, table_name):
    try:
        drop_table_sql = """DROP TABLE IF EXISTS {table_name}""".format(table_name = table_name)
        #print('drop_table_sql=', drop_table_sql)
        database_cursor.execute(drop_table_sql)
        create_table_sql = """CREATE  TABLE {table_name} (
                            cpu_time REAL, 
                            gps_latitude REAL, 
                            gps_longitude REAL,
                            gps_time REAL, 
                            gps_number_of_satellites INTEGER, 
                            gps_quality INTEGER, 
                            gps_heading REAL,
                            gps_velocity REAL,
                            record_txt BLOB
                            );""".format(table_name = table_name)
        #print('create_table_sql=', create_table_sql)
        database_cursor.execute(create_table_sql)
        return True
    except:
        print('***************************************')
        print('database_create_table error : ', str(traceback.format_exc()))
        print('***************************************')
        return False

def database_create_table_sensors(database_cursor, table_name):
    try:
        drop_table_sql = """DROP TABLE IF EXISTS {table_name}""".format(table_name = table_name)
        #print('drop_table_sql=', drop_table_sql)
        database_cursor.execute(drop_table_sql)
        create_table_sql = """CREATE  TABLE {table_name} (
                            cpu_time REAL, 
                            sensor_id integer,
                            sensor_counter integer,                              
                            gps_latitude REAL, 
                            gps_longitude REAL,
                            gps_time REAL, 
                            gps_number_of_satellites INTEGER, 
                            gps_heading REAL,
                            gps_velocity REAL,                            
                            imu_acceleration_x REAL, 
                            imu_acceleration_y REAL, 
                            imu_acceleration_z REAL,
                            imu_gyro_x REAL, 
                            imu_gyro_y REAL, 
                            imu_gyro_z REAL,
                            gps_quality integer,
                            gps_hdop REAL                            
                            );""".format(table_name = table_name)
        #print('create_table_sql=', create_table_sql)
        database_cursor.execute(create_table_sql)
        return True
    except:
        print('***************************************')
        print('database_create_table error : ', str(traceback.format_exc()))
        print('***************************************')
        return False