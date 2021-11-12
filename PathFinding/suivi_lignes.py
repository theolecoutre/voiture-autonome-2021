from gridmap import OccupancyGridMap
import matplotlib.pyplot as plt
from a_star import a_star
from utils import plot_path
from traitement_ligne import *

def trouver_chemin(masque_img):
    # on charge notre carte à partir du masque
    gmap = OccupancyGridMap.from_png(masque_img, 1)

    # on définit le point de départ (position du véhicule): généralement dans le milieu-bas de la photo
    start_node = (559.0, 0.0)

    # on calcule notre point d'arrivée
    goal_node = getArrivalPoint(gmap, hauteur=gmap.dim_cells[0]-50);

    # on lance l'algorithme A*
    path, path_px = a_star(start_node, goal_node, gmap, movement='8N')

    gmap.plot()

    if path:
        # plot resulting path in pixels over the map
        plot_path(path_px)
    else:
        print('Impossible d\'atteindre le point d\'arrivée!')

        # plot des points d'arrivée et de départ
        start_node_px = gmap.get_index_from_coordinates(start_node[0], start_node[1])
        goal_node_px = gmap.get_index_from_coordinates(goal_node[0], goal_node[1])

        plt.plot(start_node_px[0], start_node_px[1], 'ro')
        plt.plot(goal_node_px[0], goal_node_px[1], 'go')

    plt.show()

# test
trouver_chemin('exemple_masques/c2.png');