# -*- coding: utf-8 -*-
from functions_parameters import parameters_by_test
from functions_analysis import read_test_data
import matplotlib.pyplot as plt
import datetime
import numpy as np
from Coordinates import Coordinates
from functions_csv import csv_save_google_maps_data

######################################## PARAMETERS #############################################################
test_name = '2019_06_28_bueu_estatico_1'
#test_name = '2019_06_28_bueu_estatico_2'
#test_name = '2019_06_28_bueu_dinamico_1'
#test_name = '2019_6_11_ctag_estatico_3'
#test_name = '2019_6_11_ctag_vuelta_sentido_antihorario_2'
parameters = parameters_by_test(0, None, test_name)

#################################################################################################################

########################################## Read test data ##########################################
(sensors_total_Tiempo, sensors_cpu_time, sensors_latitude, sensors_longitude, sensors_x,
 sensors_y, sensors_velocity, sensors_angular_velocity, sensors_acceleration_x,
 sensors_acceleration_y, sensors_acceleration_z, sensors_mean_latitude, sensors_mean_longitude, sensors_mean_x, sensors_mean_y,
 sensors_time, sensors_velocity_estimation, sensors_angular_velocity_estimation,
 sensors_heading, sensors_number_of_satellites, sensors_hdop,
 gps_latitude, gps_longitude, gps_x, gps_y, gps_velocity, gps_cpu_time, gps_heading) = read_test_data(parameters)

########################################## Save data to create google maps ##########################################
for sensor_id in sensors_x.keys():
    csv_file_name = str('./CSV/' + parameters['sensors_table_name'] + '_sensor_' + str(sensor_id) +'.csv')
    csv_save_google_maps_data(sensors_latitude[sensor_id], sensors_longitude[sensor_id], csv_file_name, parameters)
csv_file_name = str('./CSV/' + parameters['gps_table_name'] + '.csv')
csv_save_google_maps_data(gps_latitude, gps_longitude, csv_file_name, parameters)

# ########################################## Plot data ##########################################

# Plot XY data
figure_xy = plt.figure(1)
figure_xy.clf()
for sensor_id in sensors_x.keys():
    plt.scatter(sensors_x[sensor_id], sensors_y[sensor_id], s=10, c=parameters['sensor_colors'][sensor_id], marker='.', label=parameters['sensor_names'][sensor_id])
    plt.scatter(sensors_mean_x[sensor_id], sensors_mean_y[sensor_id], s=100, c=parameters['sensor_colors'][sensor_id], marker='o')
if parameters['gps_data_enabled']:
    plt.scatter(gps_x, gps_y, s=10, c='black', marker='x', label='GPS diferencial')
plt.grid()
plt.xlabel('X [m]', fontsize=parameters['label_font_size'])
plt.ylabel('Y [m]', fontsize=parameters['label_font_size'])
plt.title(str('Mapa XY. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
plt.tight_layout()
if parameters['save_figures']:
    plt.savefig(str('./IMAGES/' + parameters['test_name'] + '_x_y.png'))

# Plot xy deviation data vs time
figure_xy_deviation_vs_time = plt.figure(2)
figure_xy_deviation_vs_time.clf()
plot_counter = 1
for sensor_id in sensors_x.keys():
    plt.subplot(2, 1, 1)
    plt.plot(sensors_cpu_time[sensor_id], sensors_x[sensor_id], color=parameters['sensor_colors'][sensor_id],
             label=parameters['sensor_names'][sensor_id], linewidth=2.5)

    if plot_counter == parameters['number_of_sensors']:
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('Desviación [m]', fontsize=parameters['label_font_size'])
        plt.title(str('Coordenada X. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
        plt.grid()

    plt.subplot(2, 1, 2)
    plt.plot(sensors_cpu_time[sensor_id], sensors_y[sensor_id], color=parameters['sensor_colors'][sensor_id],
             label=parameters['sensor_names'][sensor_id], linewidth=2.5)
    if plot_counter == parameters['number_of_sensors']:
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('Desviación [m]', fontsize=parameters['label_font_size'])
        plt.title(str('Coordenada Y. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
        plt.grid()

    plot_counter += 1

plt.tight_layout()
if parameters['save_figures']:
    plt.savefig(str('./IMAGES/' + parameters['test_name'] + '_xy_deviation_vs_time.png'))

# Plot deviation histograms
figure_deviation_histograms = plt.figure(3)
figure_deviation_histograms.clf()
plot_counter = 1
for sensor_id in sensors_x.keys():
    plt.subplot(parameters['number_of_sensors'], 2, plot_counter)
    plt.hist([x-sensors_mean_x[sensor_id] for x in sensors_x[sensor_id]], parameters['histogram_number_of_bins'], color=parameters['sensor_colors'][sensor_id], label=parameters['sensor_names'][sensor_id])
    plt.grid()
    plt.xlabel('X [m]', fontsize=parameters['label_font_size'])
    plt.ylabel('Frequencia', fontsize=parameters['label_font_size'])
    plt.title(str('Desviación media ' + parameters['sensor_names'][sensor_id]))

    plt.subplot(parameters['number_of_sensors'], 2, plot_counter + 1)
    plt.hist([y-sensors_mean_x[sensor_id] for y in sensors_y[sensor_id]], parameters['histogram_number_of_bins'], color=parameters['sensor_colors'][sensor_id], label=parameters['sensor_names'][sensor_id])
    plt.grid()
    plt.xlabel('Y [m]', fontsize=parameters['label_font_size'])
    plt.ylabel('Frequencia', fontsize=parameters['label_font_size'])
    plt.title(str('Desviación media ' + parameters['sensor_names'][sensor_id]))

    plot_counter += 2

plt.tight_layout()
if parameters['save_figures']:
    plt.savefig(str('./IMAGES/' + parameters['test_name'] + '_deviation_histogram.png'))

# Plot number of satellites data
figure_number_of_satellites = plt.figure(4)
figure_number_of_satellites.clf()
plot_counter = 1
for sensor_id in sensors_x.keys():
    if parameters['hdop_enabled']:
        plt.subplot(2, 1, 1)
        plt.plot(sensors_cpu_time[sensor_id], sensors_number_of_satellites[sensor_id], color=parameters['sensor_colors'][sensor_id],
             label=parameters['sensor_names'][sensor_id], linewidth=2.5)
    if plot_counter == parameters['number_of_sensors']:
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('Satélites', fontsize=parameters['label_font_size'])
        plt.title(str('Número de satelites. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
        plt.grid()

    if parameters['hdop_enabled']:
        plt.subplot(2, 1, 2)
        plt.plot(sensors_cpu_time[sensor_id], sensors_hdop[sensor_id], color=parameters['sensor_colors'][sensor_id],
                 label=parameters['sensor_names'][sensor_id], linewidth=2.5)
    if plot_counter == parameters['number_of_sensors']:
        plt.grid()
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('H.D.O.P.', fontsize=parameters['label_font_size'])
        plt.title(str('H.D.O.P. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
    plot_counter += 1

plt.tight_layout()
if parameters['save_figures']:
    plt.savefig(str('./IMAGES/' + parameters['test_name'] + '_number_of_satellites_hdop.png'))


# Plot velocity data
figure_velocity = plt.figure(6)
figure_velocity.clf()
plot_counter = 1
for sensor_id in sensors_x.keys():
    plt.subplot(2, 1, 1)
    plt.plot(sensors_cpu_time[sensor_id], sensors_velocity[sensor_id], color=parameters['sensor_colors'][sensor_id],
             label=parameters['sensor_names'][sensor_id], linewidth=2.5)
    if plot_counter == parameters['number_of_sensors']:
        if parameters['gps_data_enabled']:
            plt.plot(gps_cpu_time, gps_velocity, color='yellow', label='Diferencial', linewidth=2.5)
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('Velocidad [km/h]', fontsize=parameters['label_font_size'])
        plt.title(str('Velocidad longitudinal. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
        plt.grid()

    plt.subplot(2, 1, 2)
    plt.plot(sensors_cpu_time[sensor_id], sensors_angular_velocity[sensor_id], color=parameters['sensor_colors'][sensor_id],
             label=parameters['sensor_names'][sensor_id], linewidth=2.5)
    if plot_counter == parameters['number_of_sensors']:
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('Velocidad [º/s]', fontsize=parameters['label_font_size'])
        plt.title(str('Velocidad angular. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
        plt.grid()

    plot_counter += 1



plt.tight_layout()
if parameters['save_figures']:
    plt.savefig(str('./IMAGES/' + parameters['test_name'] + '_velocities_vs_time.png'))

# Plot acceleration data
figure_acceleration = plt.figure(7)
figure_acceleration.clf()
plot_counter = 1
for sensor_id in sensors_x.keys():
    plt.subplot(3, 1, 1)
    plt.plot(sensors_cpu_time[sensor_id], sensors_acceleration_x[sensor_id],
             color=parameters['sensor_colors'][sensor_id],
             label=parameters['sensor_names'][sensor_id], linewidth=2.5)
    if plot_counter == parameters['number_of_sensors']:
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('Aceleración [m/s²]', fontsize=parameters['label_font_size'])
        plt.title(str('Aceleración eje X. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
        plt.grid()

    plt.subplot(3, 1, 2)
    plt.plot(sensors_cpu_time[sensor_id], sensors_acceleration_y[sensor_id],
             color=parameters['sensor_colors'][sensor_id],
             label=parameters['sensor_names'][sensor_id], linewidth=2.5)
    if plot_counter == parameters['number_of_sensors']:
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('Aceleración [m/s²]', fontsize=parameters['label_font_size'])
        plt.title(str('Aceleración eje Y. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})
        plt.grid()

    plt.subplot(3, 1, 3)
    plt.plot(sensors_cpu_time[sensor_id], sensors_acceleration_z[sensor_id],
             color=parameters['sensor_colors'][sensor_id],
             label=parameters['sensor_names'][sensor_id], linewidth=2.5)
    if plot_counter == parameters['number_of_sensors']:
        plt.grid()
        plt.xlabel('Tiempo [s]', fontsize=parameters['label_font_size'])
        plt.ylabel('Aceleración [m/s²]', fontsize=parameters['label_font_size'])
        plt.title(str('Aceleración eje Z. Test : ' + parameters['test_name']), fontsize=parameters['label_font_size'])
        plt.legend(loc='lower right', prop={'size': parameters['label_font_size']})

    plot_counter += 1
plt.tight_layout()
if parameters['save_figures']:
    plt.savefig(str('./IMAGES/' + parameters['test_name'] + '_accelerations_vs_time.png'))