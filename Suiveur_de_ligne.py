#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Nov  3 17:40:30 2021

@author: louisdelsol
"""

import cv2
import numpy as np
import matplotlib.pyplot as plt
import logging
import math

frame = cv2.imread('/Users/theo/Documents/Temporaire/images.png') # On ouvre une image enregistré sur l'ordinateur


def detect_edges(frame): # Créé une nouvelle image avec les bordure des objets bleu

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV) # On transforme le format BRG en HSV pour éviter les différentes teintes de bleu due à la luminosité
    #plt.imshow(hsv) # On affiche la nouvelle image
    
    lower_blue = np.array([60, 40, 40]) # Bleu clair
    upper_blue = np.array([150, 255, 255]) # Bleu foncé
    mask = cv2.inRange(hsv, lower_blue, upper_blue) # On ne garde que les couleurs entre le bleu clair et le bleu foncé (à noter que open CV utilise une gamme de couleur entre 0 et 180)
    #plt.imshow(mask) # On affiche la nouvelle image
    
    edges = cv2.Canny(mask, 200, 400) # On ne garde que les contours
    #plt.imshow(edges) # On affiche la nouvelle image
    
    return edges

#edges = detect_edges(frame)


def region_of_interest(edges): # Créé une nouvelle image avec seulement les bordure du bas
    height, width = edges.shape # Dimensions de l'image
    mask = np.zeros_like(edges) # On crée un masque vide

    # On ne garde que le bas de l'image
    polygon = np.array([[
        (0, height * 1 / 2),
        (width, height * 1 / 2),
        (width, height),
        (0, height),
    ]], np.int32)

    cv2.fillPoly(mask, polygon, 255)
    cropped_edges = cv2.bitwise_and(edges, mask)
    #plt.imshow(cropped_edges) # On affiche la nouvelle image
    
    return cropped_edges

#cropped_edges = region_of_interest(edges)


def detect_line_segments(cropped_edges): # Transforme les bordures en segments
    # tuning min_threshold, minLineLength, maxLineGap sont déterminées empiriquement
    rho = 1  # Précision sur la distance en nombre de pixels
    angle = np.pi / 180  # Précision sur l'angle en radian (= 1 degré)
    min_threshold = 10  # nombre de votes minimal
    line_segments = cv2.HoughLinesP(cropped_edges, rho, angle, min_threshold, 
                                    np.array([]), minLineLength=8, maxLineGap=4)
    #print(line_segments) # On obtient les coordonnées de début et de fin de chaque segment

    return line_segments


#line_segments = detect_line_segments(cropped_edges)


def make_points(frame, line): # Renvoie les extrémités de la ligne
    height, width, _ = frame.shape
    slope, intercept = line
    y1 = height  # bas de l'image
    y2 = int(y1 * 1 / 2)  # créé des points du milieu au bas de l'image

    # lie les coordonnées dans l'image
    x1 = max(-width, min(2 * width, int((y1 - intercept) / slope)))
    x2 = max(-width, min(2 * width, int((y2 - intercept) / slope)))
    return [[x1, y1, x2, y2]]


def average_slope_intercept(frame, line_segments): # Créé 2 lignes à partir des segments
    """
    This function combines line segments into one or two lane lines
    If all line slopes are < 0: then we only have detected left lane
    If all line slopes are > 0: then we only have detected right lane
    """
    lane_lines = []
    if line_segments is None: # Si il n'y a aucun segment
        logging.info('No line_segment segments detected')
        return lane_lines

    height, width, _ = frame.shape # On prend les dimenssion de l'image
    left_fit = []
    right_fit = []

    boundary = 1/3
    left_region_boundary = width * (1 - boundary)  # Les segments de ligne de la voie de gauche doivent être sur les 2/3 gauches de l'écran
    right_region_boundary = width * boundary # Les segments de ligne de la voie de droite doivent être sur les 2/3 droits de l'écran

    for line_segment in line_segments:
        for x1, y1, x2, y2 in line_segment:
            if x1 == x2:
                #logging.info('skipping vertical line segment (slope=inf): %s' % line_segment) # On ne veut pas les lignes verticales
                continue
            fit = np.polyfit((x1, x2), (y1, y2), 1) # Génére un polynome de degré 1 qui passe par les point de coordonnées (x1, y1) et (x2, y2)
            slope = fit[0]
            intercept = fit[1]
            if slope < 0:
                if x1 < left_region_boundary and x2 < left_region_boundary: # Si la pente est négative et qu'on se trouve dans la partie gauche
                    left_fit.append((slope, intercept))
            else:
                if x1 > right_region_boundary and x2 > right_region_boundary:
                    right_fit.append((slope, intercept))

    left_fit_average = np.average(left_fit, axis=0) # Moyenne des pentes de la partie gauche
    if len(left_fit) > 0:
        lane_lines.append(make_points(frame, left_fit_average)) # On génére la ligne de la partie gauche

    right_fit_average = np.average(right_fit, axis=0) # Moyenne des pentes de la partie droite
    if len(right_fit) > 0:
        lane_lines.append(make_points(frame, right_fit_average)) # On génére la ligne de la partie droite

    #logging.debug('lane lines: %s' % lane_lines)  # [[[316, 720, 484, 432]], [[1009, 720, 718, 432]]]

    return lane_lines


#lane_lines = average_slope_intercept(frame, line_segments)

def detect_lane(frame): # Fonction qui résume les précédentes
    
    edges = detect_edges(frame)
    cropped_edges = region_of_interest(edges)
    line_segments = detect_line_segments(cropped_edges)
    lane_lines = average_slope_intercept(frame, line_segments)
    
    return lane_lines

#lane_lines = detect_lane(frame)


def display_lines(frame, lines, line_color=(0, 255, 0), line_width=2):
    line_image = np.zeros_like(frame) # On créé une image de la taille de l'image de base
    if lines is not None:
        for line in lines:
            for x1, y1, x2, y2 in line:
                cv2.line(line_image, (x1, y1), (x2, y2), line_color, line_width) # On représente les 2 lignes
    line_image = cv2.addWeighted(frame, 0.8, line_image, 1, 1) # On supperpose les 2 images
    return line_image

#lane_lines_image = display_lines(frame, lane_lines)
#cv2.imshow("lane lines", lane_lines_image)
#plt.imshow(lane_lines_image) # On affiche la nouvelle image


def compute_steering_angle(frame, lane_lines):
    """ Find the steering angle based on lane line coordinate
        We assume that camera is calibrated to point to dead center
    """
    if len(lane_lines) == 0: # Si on ne voit pas de ligne on ne fait rien
        logging.info('No lane lines detected, do nothing')
        return -90

    height, width, _ = frame.shape
    if len(lane_lines) == 1: # Si on voit une ligne, on suit sa direction
        logging.debug('Only detected one lane line, just follow it. %s' % lane_lines[0])
        x1, _, x2, _ = lane_lines[0][0]
        x_offset = x2 - x1
    else:
        _, _, left_x2, _ = lane_lines[0][0]
        _, _, right_x2, _ = lane_lines[1][0]
        camera_mid_offset_percent = 0.0 # 0.0 signifie que la caméra pointe vers le centre, -0.03 : la caméra est centrée sur la gauche, +0.03 signifie que la caméra pointe vers la droite
        mid = int(width / 2 * (1 + camera_mid_offset_percent))
        x_offset = (left_x2 + right_x2) / 2 - mid # Décalage du x du bout de la ligne à suivre par rapport au milieu

    # Trouver l'angle de direction, qui est l'angle entre la direction de la navigation et l'extrémité de la ligne centrale.
    y_offset = int(height / 2)

    angle_to_mid_radian = math.atan(x_offset / y_offset)  # angle (en radian) par rapport à la ligne verticale centrale
    angle_to_mid_deg = int(angle_to_mid_radian * 180.0 / math.pi)  # angle (en degrés) par rapport à la ligne verticale centrale
    steering_angle = angle_to_mid_deg + 90  # C'est l'angle de braquage nécessaire à la roue avant de la voiture

    logging.debug('new steering angle: %s' % steering_angle)
    return steering_angle

#steering_angle = compute_steering_angle(frame, lane_lines)

def display_heading_line(frame, steering_angle, line_color=(0, 0, 255), line_width=5 ):
    heading_image = np.zeros_like(frame)
    height, width, _ = frame.shape

    # déterminer la ligne de cap à partir de l'angle de braquage
    # la ligne de direction (x1, y1) est toujours au centre du bas de l'écran
    # (x2, y2) nécessite un peu de trigonométrie

    # Note : l'angle de direction de :
    # 0-89 degré : tourner à gauche
    # 90 degrés : aller tout droit
    # 91-180 degré : tourner à droite 
    steering_angle_radian = steering_angle / 180.0 * math.pi
    x1 = int(width / 2)
    y1 = height
    x2 = int(x1 - height / 2 / math.tan(steering_angle_radian)) # = x1 + x_offset
    y2 = int(height / 2)

    cv2.line(heading_image, (x1, y1), (x2, y2), line_color, line_width)
    heading_image = cv2.addWeighted(frame, 0.8, heading_image, 1, 1)

    return heading_image

#heading_image = display_heading_line(frame, steering_angle)
#plt.imshow(heading_image) # On affiche la nouvelle image


def stabilize_steering_angle(curr_steering_angle, new_steering_angle, num_of_lane_lines, max_angle_deviation_two_lines=5, max_angle_deviation_one_lane=1):
    """
    Using last steering angle to stabilize the steering angle
    if new angle is too different from current angle, 
    only turn by max_angle_deviation degrees
    """
    if num_of_lane_lines == 2 : # si les deux voies sont détectées, alors on peut dévier davantage
        max_angle_deviation = max_angle_deviation_two_lines
    else : # Si une seule voie est détectée, ne déviez pas trop.
        max_angle_deviation = max_angle_deviation_one_lane
    
    angle_deviation = new_steering_angle - curr_steering_angle
    if abs(angle_deviation) > max_angle_deviation: # Si l'angle de déviation est trop important on l'augmente juste du maximum
        stabilized_steering_angle = int(curr_steering_angle
            + max_angle_deviation * angle_deviation / abs(angle_deviation))
    else:
        stabilized_steering_angle = new_steering_angle
    return stabilized_steering_angle


def steering_angle(frame):
    lane_lines = detect_lane(frame)
    steering_angle = compute_steering_angle(frame, lane_lines)
    
    return  steering_angle

# Fonction de test

def test_photo(file):
    
    frame = cv2.imread(file)
    lane_lines = detect_lane(frame)
    steering_angle = compute_steering_angle(frame, lane_lines)
    heading_image = display_heading_line(frame, steering_angle)
    plt.imshow(heading_image) # On affiche la nouvelle image
    
    return steering_angle

#print(test_photo('/Users/theo/Documents/Temporaire/images.png'))
























