# -*- coding: utf-8 -*-
import sys, traceback, serial, datetime, time
from Coordinates import Coordinates
coordinates = Coordinates()

def serial_port_flush(serial_port):
    try:
        serial_port.flushInput()
        serial_port.flushOutput()
        return True
    except:
        print('***************************************')
        print('serial_port_read error : ', str(traceback.format_exc()))
        print('***************************************')
        return False

def serial_port_gps_differential_data(serial_port):
    global coordinates
    database_record = None
    try:

        if serial_port.inWaiting() > 0:
            serial_data_txt_a = serial_port.readline().decode('utf-8')
            serial_data = serial_data_txt_a.split(',')
            gps_latitude = None
            gps_longitude = None
            gps_time = None
            gps_number_of_satellites = None
            gps_quality = None
            gps_velocity = None
            gps_heading = None

            if serial_data[0] == '$GPGGA':
                (gps_longitude, gps_latitude) = coordinates.convert_trimble_gps_data(float(serial_data[4]), float(serial_data[2]))
                gps_time = float(serial_data[1])
                gps_number_of_satellites = int(serial_data[7])
                gps_quality = int(serial_data[6])
            else:
                gps_velocity = coordinates.convert_knots_meter_second(float(serial_data[7]))
                gps_heading = float(serial_data[8])
            time.sleep(0.0001)

            if serial_port.inWaiting() > 0:
                serial_data_txt_b = serial_port.readline().decode('utf-8')
                serial_data = serial_data_txt_b.split(',')
                if serial_data[0] == '$GPGGA':
                    (gps_longitude, gps_latitude) = coordinates.convert_trimble_gps_data(float(serial_data[4]), float(serial_data[2]))
                    gps_time = float(serial_data[1])
                    gps_number_of_satellites = int(serial_data[7])
                    gps_quality = int(serial_data[6])
                else:
                    gps_velocity = coordinates.convert_knots_meter_second(float(serial_data[7]))
                    gps_heading = float(serial_data[8])

            database_record = (str(datetime.datetime.now()), gps_latitude, gps_longitude, gps_time, gps_number_of_satellites, gps_quality, \
                               gps_heading, gps_velocity, str(serial_data_txt_a + '__' + serial_data_txt_b))

            return database_record
    except:
        print('***************************************')
        print('serial_port_read error : ', str(traceback.format_exc()))
        print('***************************************')
        return database_record

def serial_port_read_sensors_network_data(serial_port):
    database_record = None
    try:
        sensor_id = 0
        sensor_counter = 0
        if serial_port.inWaiting() > 0:
            serial_data_txt = serial_port.readline().decode('utf-8')
            serial_data = serial_data_txt.split(';')
            if serial_data[0] == '1':
                sensor_id = 1
                sensor_counter = int(serial_data[1])
            elif serial_data[0] == '2':
                sensor_id = 2
                sensor_counter = int(serial_data[1])
            elif serial_data[0] == '3':
                sensor_id = 3
                sensor_counter = int(serial_data[1])
            elif serial_data[0] == '4':
                sensor_id = 4
                sensor_counter = int(serial_data[1])
            else:
                return None

        if sensor_id > 0:
            counter = 0
            while True:
                if serial_port.inWaiting() > 0:
                    serial_data_txt = serial_port.readline().decode('utf-8')
                    serial_data_gga = serial_data_txt.split(',')
                    break
                counter += 1
                time.sleep(0.0001)
                if counter > 100:
                    return None

            counter = 0
            while True:
                if serial_port.inWaiting() > 0:
                    serial_data_txt = serial_port.readline().decode('utf-8')
                    serial_data_rmc = serial_data_txt.split(',')
                    break
                counter += 1
                time.sleep(0.0001)
                if counter > 100:
                    return None

            counter = 0
            while True:
                if serial_port.inWaiting() > 0:
                    serial_data_txt = serial_port.readline().decode('utf-8')
                    serial_data_imu = serial_data_txt.split(';')
                    break
                counter += 1
                time.sleep(0.0001)
                if counter > 100:
                    return None

            (gps_longitude, gps_latitude) = coordinates.convert_trimble_gps_data(float(serial_data_gga[4]),float(serial_data_gga[2]))
            gps_quality =  int(serial_data_gga[6])
            gps_number_of_satellites = int(serial_data_gga[7])
            gps_hdop = float(serial_data_gga[8])
            gps_heading = 0.0 #float(serial_data_rmc[8])
            gps_velocity = float(serial_data_rmc[7])
            gps_time = float(serial_data_gga[1])
            imu_acceleration_x = float(serial_data_imu[0])
            imu_acceleration_y = float(serial_data_imu[1])
            imu_acceleration_z = float(serial_data_imu[2])
            imu_gyro_x = float(serial_data_imu[3])
            imu_gyro_y = float(serial_data_imu[4])
            imu_gyro_z = float(serial_data_imu[5])

            database_record = (str(datetime.datetime.now()), int(sensor_id), sensor_counter, gps_latitude, gps_longitude,
                               str(gps_time), gps_number_of_satellites, gps_heading, gps_velocity,
                               imu_acceleration_x, imu_acceleration_y, imu_acceleration_z, imu_gyro_x, imu_gyro_y,
                               imu_gyro_z, gps_quality, gps_hdop)
            return database_record
    except:
        print('***************************************')
        print('serial_port_read error : ', str(traceback.format_exc()))
        print('***************************************')
        return database_record

def serial_port_open(port_name, baudrate):
    try:
        serial_port = serial.Serial()
        serial_port.port = port_name
        serial_port.baudrate = baudrate
        serial_port.open()
        if serial_port.isOpen():
            return serial_port
        else:
            return None
    except:
        print('***************************************')
        print('serial_port_open error : ', str(traceback.format_exc()))
        print('***************************************')
        return None
