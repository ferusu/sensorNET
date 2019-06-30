# -*- coding: utf-8 -*-
import numpy as np
from functions_database import *
from Coordinates import Coordinates
import matplotlib.pyplot as plt


def read_test_data(parameters):
    try:

        # Create coordinates object
        print('Coordinates :: origin_latitude=', parameters['origin_latitude'], 'origin_longitude=', parameters['origin_longitude'])
        coordinates = Coordinates(parameters['origin_longitude'], parameters['origin_latitude'])

        # Connect to the sensors database
        (database_connection, database_cursor) = database_connect(parameters['database_name'])
        if not database_connection is None:
            # Sensors test data read
            sensors_database_records = database_read_sensors_data(database_cursor, parameters['sensors_table_name'])
            number_of_records = sensors_database_records.shape[0]
            print('Sensor database number_of_records = ', number_of_records)

            # Group data by sensor id
            sensors_latitude = dict()
            sensors_longitude = dict()
            sensors_x = dict()
            sensors_y = dict()
            sensors_number_of_satellites = dict()
            sensors_velocity = dict()
            sensors_hdop = dict()
            sensors_angular_velocity = dict()
            sensors_heading = dict()
            sensors_acceleration_x = dict()
            sensors_acceleration_y = dict()
            sensors_acceleration_z = dict()
            sensors_cpu_time = dict()
            sensors_mean_x = dict()
            sensors_mean_y = dict()
            sensors_mean_latitude = dict()
            sensors_mean_longitude = dict()
            sensors_number_of_records = dict()
            sensors_time = []
            sensors_velocity_estimation = []
            sensors_angular_velocity_estimation = []
            sensors_velocity_memory = None
            sensors_angular_velocity_memory = 0.0
            gps_velocity_memory = None
            start_cpu_time = None
            sensors_total_time = 0.0
            for record in sensors_database_records:
                sensor_id = record[1]
                if not sensor_id in sensors_longitude:
                    sensors_longitude[sensor_id] = list()
                    sensors_latitude[sensor_id] = list()
                    sensors_number_of_satellites[sensor_id] = list()
                    sensors_x[sensor_id] = list()
                    sensors_y[sensor_id] = list()
                    sensors_velocity[sensor_id] = list()
                    sensors_hdop[sensor_id] = list()
                    sensors_cpu_time[sensor_id] = list()
                    sensors_angular_velocity[sensor_id] = list()
                    sensors_acceleration_x[sensor_id] = list()
                    sensors_acceleration_y[sensor_id] = list()
                    sensors_acceleration_z[sensor_id] = list()
                    sensors_heading[sensor_id] = list()
                    sensors_mean_x[sensor_id] = 0.0
                    sensors_mean_y[sensor_id] = 0.0
                    sensors_mean_latitude[sensor_id] = 0.0
                    sensors_mean_longitude[sensor_id] = 0.0
                    sensors_number_of_records[sensor_id] = 0
                #print(record)
                sensors_hdop[sensor_id].append(record[16])
                sensors_number_of_satellites[sensor_id].append(record[6])
                sensors_latitude[sensor_id].append(record[3])
                sensors_longitude[sensor_id].append(record[4])
                (x, y) = coordinates.convert_longitude_latitude_to_xy(record[4], record[3])
                sensors_number_of_records[sensor_id] += 1
                sensors_x[sensor_id].append(x)
                sensors_y[sensor_id].append(y)
                sensors_heading[sensor_id].append(record[7])
                velocity = record[8] - 0.0 * parameters['sensors_velocities_offsets'][sensor_id - 1]  # km/h
                sensors_velocity[sensor_id].append(velocity)
                angular_velocity = record[14] - 0.0 * parameters['sensors_angular_velocity_z_ofssets'][sensor_id - 1]
                sensors_angular_velocity[sensor_id].append(angular_velocity)  # º/s
                sensors_acceleration_x[sensor_id].append(record[9] - 0.0 * parameters['sensors_acceleration_x_offsets'][sensor_id - 1])
                sensors_acceleration_y[sensor_id].append(record[10] - 0.0 * parameters['sensors_acceleration_y_offsets'][sensor_id - 1])
                sensors_acceleration_z[sensor_id].append(record[11] - 0.0 * parameters['sensors_acceleration_z_offsets'][sensor_id - 1])

                if start_cpu_time is None:
                    start_cpu_time = record[0]
                sensor_time = ((np.datetime64(record[0]) - np.datetime64(start_cpu_time)) / np.timedelta64(1, 'ms')) / 1.0e3  # / np.timedelta64(1, 's') #.total_seconds()
                sensors_cpu_time[sensor_id].append(sensor_time)
                if sensor_time > sensors_total_time:
                    sensors_total_time = sensor_time

                if sensors_velocity_memory is None:
                    velocity_estimation = velocity
                else:
                    velocity_gain = 0.05 #(1.0 - np.exp(-parameters['adaptive_filter_exponential_gain']*abs(velocity))) * parameters['sensors_velocity_filter_gains'][sensor_id - 1]
                    velocity_estimation = velocity_gain * velocity + (1.0 - velocity_gain) * sensors_velocity_memory
                sensors_velocity_memory = velocity_estimation
                angular_velocity_gain = parameters['sensors_angular_velocity_filter_gains'][sensor_id - 1]  #(1.0 - np.exp(-1.0 * abs(angular_velocity))) * parameters['sensors_velocity_filter_gains'][sensor_id - 1]

                angular_velocity_estimation = angular_velocity_gain * angular_velocity + (1.0 - angular_velocity_gain) * sensors_angular_velocity_memory
                sensors_angular_velocity_memory = angular_velocity_estimation
                sensors_time.append(sensor_time)
                sensors_velocity_estimation.append(velocity_estimation)
                sensors_angular_velocity_estimation.append(angular_velocity_estimation)

            # Compute mean position for each sensor
            for sensor_id in sensors_x.keys():
                sensors_mean_x[sensor_id] = np.mean(sensors_x[sensor_id])
                sensors_mean_y[sensor_id] = np.mean(sensors_y[sensor_id])
                sensors_mean_latitude[sensor_id] = np.mean(sensors_latitude[sensor_id])
                sensors_mean_longitude[sensor_id] = np.mean(sensors_longitude[sensor_id])

            # GPS test data read
            gps_latitude = []
            gps_longitude = []
            gps_x = []
            gps_y = []
            gps_velocity = []
            gps_cpu_time = []
            gps_heading = []
            if parameters['gps_data_enabled']:
                gps_database_records = database_read_gps_data(database_cursor, parameters['gps_table_name'])
                number_of_records = gps_database_records.shape[0]
                print('GPS database number_of_records = ', number_of_records)
                for gps_record in gps_database_records:
                    gps_time = ((np.datetime64(gps_record[0]) - np.datetime64(start_cpu_time)) / np.timedelta64(1, 'ms')) / 1.0e3
                    if gps_time < 0:
                        continue
                    if gps_time > sensors_total_time:
                        break
                    gps_latitude.append(gps_record[1])
                    gps_longitude.append(gps_record[2])
                    (x, y) = coordinates.convert_longitude_latitude_to_xy(gps_record[2], gps_record[1])
                    gps_x.append(x)
                    gps_y.append(y)
                    gps_heading.append(gps_record[6])
                    velocity = gps_record[7] * 3.60
                    if not gps_velocity_memory is None:
                        gps_velocity_gain = (1.0 - np.exp(-parameters['adaptive_filter_exponential_gain'] * abs(velocity))) * parameters['gps_velocity_filter_gain']
                        velocity = gps_velocity_gain * velocity + (1.0 - gps_velocity_gain) * gps_velocity_memory
                    gps_velocity_memory = velocity
                    gps_velocity.append(velocity) #m/s->km/h
                    gps_cpu_time.append(gps_time)
        database_close(database_connection)
        return (sensors_total_time, sensors_cpu_time, sensors_latitude, sensors_longitude, sensors_x, sensors_y, sensors_velocity,
                sensors_angular_velocity, sensors_acceleration_x, sensors_acceleration_y,  sensors_acceleration_z, sensors_mean_latitude, sensors_mean_longitude,
                sensors_mean_x, sensors_mean_y, sensors_time, sensors_velocity_estimation, sensors_angular_velocity_estimation,
                sensors_heading, sensors_number_of_satellites, sensors_hdop,
                gps_latitude, gps_longitude, gps_x, gps_y, gps_velocity, gps_cpu_time, gps_heading)


    except:
        print('***************************************')
        print('read_sensors_and_group_by_sensor error : ', str(traceback.format_exc()))
        print('***************************************')
        return False
