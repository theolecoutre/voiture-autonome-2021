# **voiture-autonome-2021**
Projet de voiture autonome - Centrale Lille - 2021

## **Répartition du travail**

 **Louis Delsol :** suivi de lignes
 
 **Luiz Fabião Giserman :** communication, réception des coordonnées avec le système MarvelMind
 
 **Simon Kurney :** impression 3D du support de caméra, automatique
 
 **Théo Lecoutre :** IA pour la reconnaissance des panneaux et détection d'objets, code Python principal du système
 
 **Mário Monteiro :** automatique
 
 **Ilyas Ouradi :** communication, solution alternative pour l'IA (modèle RCNN)
 
 **Noé Pigret :** automatique
 
 **Sun Yishan :** communication
 

## **Reconnaissance d'objets**

La reconnaissance d'objets est une partie cruciale de notre projet, car l'autonomie de notre voiture passe impérativement par la perception de son environnement.

### **Matériel**

Le matériel utilisé pour la perception de l'environnement se compose de :
* **un Raspberry Pi 3 Model B+**
* **une caméra 5 MPx pour Raspberry Pi (vient de element14.com) :** ce sera les "yeux" de notre maquette
* **l'accélérateur USB Coral Edge TPU Coprocessor :** le Raspberry Pi 3 étant peu puissant, la reconnaissance d'objet par intelligence artificielle prend beaucoup de temps. Cet accélérateur permet de faire fonctionner le modèle d'IA plus rapidement.

### **Logiciel**
La partie logicielle se décompose en 2 parties :
* **Open CV** est une librairie Python open source , qui permet la lecture du flux vidéo de la webcam, ainsi que son traitement
* **Tensorflow** est une plateforme complète d'intelligence artifcielle : elle permettra de créer notre modèle, de l'entraîner, et de connaître ses prédictions pour notre environnement.

### **Déroulé du travail**

Pour mener à bien cette partie de notre projet, nous avons identifié 3 étapes clés :

1. Nous devons constituer un dataset des objets que notre véhicule doit pouvoir détecter
2. Nous devons créer/trouver un modèle de réseau de neurones et l'entraîner sur notre dataset
3. Lors du fonctionnement de notre maquette, le véhicule doit pouvoir reconnaître les objets qui l'entourent.

#### **Le dataset**

Le dataset constitue la partie la plus importante de la reconnaissance d'objet : même le réseau de neurones le plus entraîné et performant demeurera médiocre avec un mauvais dataset (déséquilibré, éloigné de la réalité...).

Dans un premier temps, nous avons identifié les objets que notre maquette doit être capable de reconnaître :

* panneau **sens interdit**
* panneau **stop**
* panneau **zone 30**
* panneau de **vitesse 50**
* panneau de **passage piéton**
* **piéton**

[comment]: <> (TODO : mettre les images des objets)


Une fois identifiés, il nous faut des images de ces objets pour consituter notre dataset. Pour cela, nous avons choisi d'imprimer les panneaux, pour ensuite les prendre en photo sous différents profils (environ 50 photos de chaque panneau). De même le piéton sera modélisé par un stylo.

Une fois que nous avons ces images, nous devons les traiter à la main, afin d'indiquer à notre modèle quels sont et pù sont les objets à détecter sur ces images.

Nous avons utilisé le logiciel labelImg qui permet de réaliser cela : à partir des bounding boxes que nous plaçons sur une image, LabelImg génère un fichier XML par image, au format Pascal VOC, indiquant les coordonnées de chaque bounding box, et l'objet identifié.

![Le logiciel labelImg en action](readme/labelImg.png "Le logiciel labelImg en action")
**Le logiciel labelImg en action**

#### **Le réseau de neurones**

Pour des raisons de complexité et de temps évidentes, nous n'avons pas créé notre propre réseau de neurones.

Nous avons eu recours au "transfer learning" : il s'agit d'utiliser un modèle déjà existant en entraîné sur un dataset, pour le ré-entraîner sur notre propre dataset.
Il existe deux solutions de transfer learning : 

* **ré-entraîner uniquement les dernières couches du modèle :** il s'agit d'uniquement ré-entraîner les dernières couches du modèle (celles qui donnent finalement la classification) sur notre dataset. Le résultat sera moins précis qu'un ré-entraînement total, mais permet un gain de temps non négligeable
* **ré-entraîner le modèle entier :** cela permet généralement une meilleure précision, mais nécessite beaucoup de temps et de ressources. De plus, il faut généralement un grand dataset (beaucoup plus de 50 images par objet, ce qui est déjà peu...) pour éviter l'overfitting.

Nous nous sommes donc naturellement tournés vers la seconde solution.

Nous avons choisi le réseau de neurones SSD MobileNet V1, qui est compatible avec le TPU Coral et qui présente un rapport précision/rapidité intéressant. Ce modèle est par défaut entraîné avec le dataset COCO, constitué de 90 objets de la vie courante.

Après avoir configuré la pipeline d'entraînement (configuration de l'entraînement : chemin d'accès aux images, graphes...) et gelé certaines couches du modèles (pour ré-entraîner uniquement les dernières), nous avons entraîné notre modèle avec 500 étapes.

Après cet entraînement, nous exportons le graphe de notre réseau au format TensorFlow Lite : c'est un format qui permet de stocker les variables sur des entiers (au lieu de flottants), permettant ainsi une exécution plus rapide, et une compatibilité avec le TPU Coral.

Une fois l'exportation réalisé", nouscompilons le model (au format .tflite) que nous obtenons grâce à l'outil edgetpu_compiler fourni sur le site de Coral, qui permet d'optimiser le modèle pour qu'il s'exécute de manière plus rapide sur le TPU.

Les résultats que nous obtenons à priori sont satisfaisants : 

![La courbe de précision en fontction des entraînements](readme/detection_precision.png "La courbe de précision en fontction des entraînements")
**La courbe de précision en fonction des entraînements**

![](readme/reco_110.png)
![](readme/reco_passage_pieton.png)
![](readme/reco_zone_30.png)
**Quelques comparaisons reconnaissance effective (à gauche)/reconnaissance théorique parfaite (à droite)**

#### **Interfaçage avec le reste du code**

Pour chaque image que le réseau de neurones traite, ils produit 3 variables de sortie qui nous intéressent :

1. Les **boundings boxes**, qui contiennent les coordonnées de chaque rectangle de détection potentielle
2. Les **classes**, un tableau contenant le numéro de classe pour chaque potentielle détection
3. Les **scores**, un tableau contenant le score de chaque potentielle détection (le score est compris entre 0 et 1). Nous considérons les prédictions comme fiables quand elles ont un score supérieur ou égal à 0,7.

Ainsi à partir de ces 3 variables, la maquette pourra connaître, à chaque image transmise par la webcam, la composition de son environnement.
