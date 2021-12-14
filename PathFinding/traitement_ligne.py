import cv2

# permet de trouver le point d'arrivée sur l'image masque
def getPointMilieu(gmap, ligne_largeur_max = 150, pas = 5, hauteur = 400, recursive = False):
    # si la hauteur est négative, cela signifie qu'on a pas pu trouver le point voulu
    assert(hauteur > 0)

    x1 = -1
    x2 = -1
    x = 0
    while(x < gmap.dim_cells[1]):
        # on vérifie si on a intersection de la ligne avec le haut de l'écran
        if gmap.is_occupied_idx((x, hauteur)):
            if x1 == -1:
                x1 = x
                # on fait un saut suffisament large pour chercher l'autre point
                x += ligne_largeur_max
            elif x2 == -1:
                # on a trouvé les 2 points d'intersection, on retourne leur moitié
                x2 = x
                return ((x1 + x2) / 2, hauteur)
        else:
            x += pas

    # on a pas trouvé le points, on diminue la hauteur
    if not recursive:
        raise Exception('Point non trouvé!')

    return getPointMilieu(gmap, ligne_largeur_max, pas, hauteur - 50, True)


# permet de rendre les traits plus épais pour éviter de passer à côté
def dilater_bordure(img_data, iterations = 10) :
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    return cv2.dilate(img_data, kernel, iterations=iterations)