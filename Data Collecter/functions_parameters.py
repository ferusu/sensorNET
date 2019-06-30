# -*- coding: utf-8 -*-
import traceback, datetime

######################################## PARAMETERS #############################################################


def parameters_by_test(test_id, test_date = None, test_name = None):
    try:
        parameters = dict()
        parameters['save_figures'] = False
        parameters['sensors_rectangle_longitude'] = 1.80
        parameters['sensors_rectangle_width'] = 0.95

        parameters['number_of_sensors'] = 3
        parameters['sensor_colors'] = ['blue', 'red', 'green'] #, 'magenta']
        parameters['sensor_names'] = ['Sensor 1', 'Sensor 2', 'Sensor 3'] #, 'Sensor 4']
        parameters['label_font_size'] = 10
        parameters['histogram_number_of_bins'] = 20
        parameters['database_name'] = './tests.db' #'./sensores_distribuidos.db' #'

        parameters['sensors_serial_port_name'] = '/dev/ttyUSB0' #'COM5' #'
        parameters['sensors_serial_port_baudrate'] = 115200
        parameters['sensors_database_insert_size'] = 20
        parameters['sensors_total_time'] = 1800  # s
        parameters['sensors_number_of_fields'] = 15

        parameters['gps_serial_port_name'] = '/dev/ttyUSB1'
        parameters['gps_serial_port_baudrate'] = 38400
        parameters['gps_database_insert_size'] = 100
        parameters['gps_total_time'] = 330  # s
        parameters['gps_number_of_fields'] = 8

        parameters['ctag_path_latitude_correction'] = 0.00001
        parameters['ctag_path_longitude_correction'] = 0.00001
        parameters['origin_longitude'] = -8.614343
        parameters['origin_latitude'] = 42.102779
        parameters['google_maps_number_of_registers'] = 2000
        parameters['gps_data_enabled'] = True
        parameters['gps_table_name'] = 'gps_' + test_name
        parameters['sensors_table_name'] = 'sensores_' + test_name
        parameters['hdop_enabled'] = True

        # Calibration parameters
        parameters['sensors_velocities_offsets'] = [0.0, 0.0, 0.0, 0.0]
        parameters['sensors_acceleration_x_offsets'] = [0.0, 0.0, 0.0, 0.0]
        parameters['sensors_acceleration_y_offsets'] = [0.0, 0.0, 0.0, 0.0]
        parameters['sensors_acceleration_z_offsets'] = [0.0, 0.0, 0.0, 0.0]
        parameters['sensors_angular_velocity_z_ofssets'] = [0.0, 0.0, 0.0, 0.0]

        # Filtering parameters
        parameters['sensors_velocity_filter_gains'] = [0.0, 0.0, 0.0, 0.0]
        parameters['sensors_angular_velocity_filter_gains'] = [0.05, 0.05, 0.05, 0.05]
        parameters['gps_velocity_filter_gain'] = 0.250
        parameters['adaptive_filter_exponential_gain'] = 0.50

        return parameters
    except:
        print('***************************************')
        print('parameters_get_by_test : ', str(traceback.format_exc()))
        print('***************************************')
        return False



