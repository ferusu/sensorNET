# -*- coding: utf-8 -*-
import traceback, decimal
import numpy as np

class Coordinates():
    def __init__(self, origin_longitude = 0.0, origin_latitude = 0.0):
        self.origin_longitude = origin_longitude
        self.origin_latitude = origin_latitude
        return

    def convert_trimble_gps_data(self, longitude, latitude):
        try:
            deg_w_lon = float(int(longitude / 100.0))
            deg_c_lon = (longitude - deg_w_lon * 100.0) / 60.0
            longitude_si = -(deg_w_lon + deg_c_lon)
            deg_w_lat = float(int(latitude / 100.0))
            deg_c_lat = (latitude - deg_w_lat * 100.0) / 60.0
            latitude_si = (deg_w_lat + deg_c_lat)
            return (longitude_si, latitude_si)
        except:
            print('Coordinates (convert_trimble_gps_data): ' + str(traceback.format_exc()))
            return (None, None)

    def convert_knots_meter_second(self, knots):
        try:
            return 0.51444444444 * float(knots)
        except:
            print('Coordinates (convert_knots_meter_second): ' + str(traceback.format_exc()))
            return None

    def convert_xy_to_longitude_latitude(self, point_x, point_y):
        try:  
            origin_longitude = self.origin_longitude
            origin_latitude = self.origin_latitude
            latitudeCircumference = 40075160.0 * np.cos(origin_latitude*np.pi/180.0)
            longitude  = (point_x*360.0 / latitudeCircumference) + origin_longitude
            latitude = (point_y*360.0 / 40008000.0) + origin_latitude
            return (longitude, latitude)
        except:  
            print('Coordinates (convert_xy_to_longitude_latitude): ' + str(traceback.format_exc()))
            return None

    def convert_longitude_latitude_to_xy(self, longitude, latitude):
        try:
            origin_longitude = self.origin_longitude
            origin_latitude = self.origin_latitude
            #print('origin_latitude=' , origin_latitude)
            latitudeCircumference = 40075160.0 * np.cos(origin_latitude*np.pi/180.0)
            point_x = (longitude - origin_longitude) * latitudeCircumference / 360.0
            point_y = (latitude - origin_latitude) * 40008000.0 / 360.0
            return (point_x, point_y)
        except:  
            print('Coordinates (convert_longitude_latitude_to_xy): ' + str(traceback.format_exc()))
            return None
      
    def convert_longitudes_latitudes_to_xy(self, longitudes, latitudes):
        try:  
            num_points = len(longitudes)
            points_x = np.zeros(num_points, dtype=np.dtype(decimal.Decimal)) 
            points_y = np.zeros(num_points, dtype=np.dtype(decimal.Decimal))
            for p in range(num_points):
                (points_x[p], points_y[p]) = self.convert_longitude_latitude_to_xy(longitudes[p], latitudes[p])
            return (points_x, points_y)
        except:  
            print('Coordinates (convert_longitudes_latitudes_to_xy): ' + str(traceback.format_exc()))
            return None
       
    def convert_xy_to_longitudes_latitudes(self, points_x, points_y):
        try:  
            num_points = len(points_x)
            longitudes = np.zeros(num_points, dtype=np.dtype(decimal.Decimal)) 
            latitudes = np.zeros(num_points, dtype=np.dtype(decimal.Decimal))
            for p in range(num_points):
                (longitudes[p], latitudes[p]) = self.convert_longitude_latitude_to_xy(points_x[p], points_y[p])
            return (longitudes, latitudes)
        except:  
            print('Coordinates (convert_xy_to_longitudes_latitudes): ' + str(traceback.format_exc()))
            return None
        
    def get_relative_position(self, x, y, x_origin, y_origin, heading_origin):      
        try:
            delta_x = x - x_origin
            delta_y = y - y_origin           
            relative_x = delta_x*np.cos(heading_origin) + delta_y*np.sin(heading_origin) 
            relative_y = -delta_x*np.sin(heading_origin) + delta_y*np.cos(heading_origin)
            return (relative_x, relative_y)
        except:
            print('Coordinates (get_relative_position): ' + str(traceback.format_exc()))
            return None
        
    def get_point_from_distance_and_angle(self, x, y, distance, angle):
        try:  
            to_x = x + distance*np.cos(angle)
            to_y = y + distance*np.sin(angle)         
            return (to_x,  to_y)      
        except:  
            print('Coordinates (get_point_from_distance_and_angle): ' + str(traceback.format_exc()))
            return None 
        
    def get_distance_and_angle_betwwen_two_points(self, from_x, from_y, to_x, to_y):
        try:  
            distance = self.get_distance(from_x, from_y, to_x, to_y)
            heading = self.get_angle(from_x, from_y, to_x, to_y)
            return (distance, heading) # (m, radians)      
        except:  
            print('Coordinates (get_distance_and_angle): ' + str(traceback.format_exc()))
            return None   

    def get_angle_betwwen_two_points(self, from_x, from_y, to_x, to_y):
        try:
            return np.arctan2((from_y - to_y), (from_x - to_x))
        except:  
            print('Coordinates (get_angle): ' + str(traceback.format_exc()))
            return None
        
    def get_distance_between_two_points(self, from_x, from_y, to_x, to_y):
        try:
            return np.sqrt((from_x - to_x)**2.0 + (from_y - to_y)**2.0)
        except:  
            print('Coordinates (get_distance): ' + str(traceback.format_exc()))
            return None