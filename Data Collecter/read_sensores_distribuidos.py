# -*- coding: utf-8 -*-
import sys, traceback, time, datetime
from functions_database import *
from functions_serial_port import *
from functions_parameters import parameters_by_test

######################################## PARAMETERS #############################################################
now = datetime.datetime.now()
test_date = str(now.year) + '_' + str(now.month) + '_' + str(now.day)
test_name = '2019_06_28_bueu_estatico_1'
test_name = '2019_06_28_bueu_dinamico_1'
#test_name = '2019_06_28_bueu_estatico_2'

parameters = parameters_by_test(0, test_date, test_name)


#################################################################################################################

if __name__ == '__main__':
    try:
        # Read name of log to be saved
        # sensors_table_name = sys.argv[1]

        # Connect to the database
        (database_connection, database_cursor) = database_connect(parameters['database_name'])
        if not database_connection is None:
            print('Connected to database : ', parameters['database_name'])
            database_create_table_sensors(database_cursor, parameters['sensors_table_name'])
            print('Created log table : ', parameters['sensors_table_name'])

            # Connect to serial port
            serial_port = serial_port_open(parameters['sensors_serial_port_name'], parameters['sensors_serial_port_baudrate'])
            if serial_port is None:
                database_close(database_connection)
                print('Unable to open serial port : ', parameters['sensors_serial_port_name'], ' with baudrate : ', parameters['sensors_serial_port_baudrate'])
            else:
                print('Opened serial port : ', parameters['sensors_serial_port_name'], ' with baudrate : ', parameters['sensors_serial_port_baudrate'])
                serial_port_flush(serial_port)
                database_records = []
                database_insert_counter = 0
                global_insert_counter = 0
                start_time = time.time()
                run_time = 0.0
                while run_time < parameters['sensors_total_time']:
                    database_record = serial_port_read_sensors_network_data(serial_port)
                    if not database_record is None:
                        database_records.append(database_record)
                        database_insert_counter += 1
                    if database_insert_counter > parameters['sensors_database_insert_size'] - 1:
                        database_insert_sensors(database_connection, database_cursor, parameters['sensors_table_name'], database_records)
                        print('Runnig time =' , run_time, '. Number of records saved = ', (global_insert_counter +1) * parameters['sensors_database_insert_size'])
                        database_records = []
                        global_insert_counter += 1
                        database_insert_counter = 0
                    time.sleep(0.0001)
                    run_time = time.time() - start_time
        else:
            print('Unable to connected to database : ', parameters['database_name'])

        database_close(database_connection)

    except KeyboardInterrupt:
        print('***************************************')
        print('KEYBOARD INTERRUPT !!! ', str(traceback.format_exc()))
        database_close(database_connection)
        serial_port_flush(serial_port)
        print('***************************************')
        sys.exit(0)
    except:
        print('***************************************')
        print('MAIN ERROR : ', str(traceback.format_exc()))
        database_close(database_connection)
        serial_port_flush(serial_port)
        print('***************************************')
        sys.exit(0)