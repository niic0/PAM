#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#include "utilities.h"
#include "data.h"


/*
 * Alloue et initialise les valeurs du dataset.
 *  - DATA* sample         : Sample a allouer
 *  - size_t nbr_objets    : Nombre d'element du dataset
 *  - size_t size_objets   : Nombre de valeurs definissant un element du sample
*/
void init_data(DATA* sample, size_t nbr_objets, size_t size_objets) {
  sample->nbr_objets = nbr_objets;
  sample->size_objets = size_objets;
  sample->objets = malloc(sizeof(OBJET)*nbr_objets);

  if(sample->objets == NULL) {
    fprintf(stderr, "Erreur d'allocation dans la fonction init_data()");
    exit(EXIT_FAILURE);
  }
}


/*
 * Creer un dataset contenant des valeurs d'un fichier csv.
 * - data data_set : Dataset contenant les valeurs à mettre dans la structure DATA
 */
DATA set_data_csv(char* file_name) {
  DATA dataset;
  FILE* stream = fopen(file_name, "r");

  char line[1024];
  char* tok;
  size_t size_objets = get_size_objets(file_name);
  size_t nbr_objets = get_nbr_objets(file_name);

  init_data(&dataset, nbr_objets, size_objets);

  for (int i=-1; fgets(line, 1024, stream); i++) {
    if (i == -1) continue;
    char* tmp = strdup(line);
    size_t k = 0;

    dataset.objets[i].point = malloc(sizeof(int)*size_objets);

    for (tok = strtok(line, ";"); tok && *tok; tok = strtok(NULL, ";\n")) {
      if (is_number(tok)) {
        dataset.objets[i].point[k] = atoi(tok);
        dataset.objets[i].rank = i;
        k++;
      }
    }

    free(tmp);
  }

  return dataset;
}


/*
 * Renvoie un dataset contenant des valeurs aleatoires comprises entre 0 et un max.
 * - size_t nbr_objets  : Nombre d'objets voulues dans le dataset
 * - size_t size_objets : Nombre de points définissant un objet
 * - size_t range       : Valeur maximal des valeurs aléatoires
*/
DATA set_data_rand(size_t nbr_objets, size_t size_objets, size_t range) {
  DATA dataset;
  init_data(&dataset, nbr_objets, size_objets);
  srand(time(NULL));

  for (size_t i=0; i < dataset.nbr_objets; i++) {
    dataset.objets[i].point = malloc(sizeof(size_t)*dataset.size_objets);
    for (size_t k=0 ; k<dataset.size_objets ; k++) {
      dataset.objets[i].rank = i;
      dataset.objets[i].point[k] = rand()%range;
    }
  }

  return dataset;
}


/*
 * Retourne le nombre de valeurs qui caractérisent un objet selon un fichier
 * csv contenant les valeurs.
 * - char* file_name : Nom du fichier csv
*/
size_t get_size_objets(char* file_name) {
  FILE* stream = fopen(file_name, "r");
  size_t size_objets = 0;
  char line[1024];
  char* tok;

  fgets(line, 1024, stream); // ligne 1 du fichier contenant le header
  fgets(line, 1024, stream); // ligne 2 du fichier

  for (tok = strtok(line, ";"); tok && *tok; tok = strtok(NULL, ";\n")) {
    if (is_number(tok))
      size_objets++;
  }

  return size_objets;
}


/*
 * Retourne le nombre de valeurs que comprend un dataset compris dans un fichier csv.
 * - char* file_name : Nom du fichier csv
*/
size_t get_nbr_objets(char* file_name) {
  FILE* stream = fopen(file_name, "r");
  size_t nbr_objets = 0;
  char line[1024];

  while (fgets(line, 1024, stream)) nbr_objets++;

  // Décrémentation de 1 car fgets a compté le header
  return nbr_objets-1;
}


/*
 * Calcul la matrice de distance d'un dataset
 * - DATA data sample : Dataset contenant les données
 * - int nbr_objets   : Nombre d'objets de la matrice (ne sert que pour la variable dist_tab)
 * - int dist_tab     : Stocke la matrice carré de la taille du nombre d'objet du dataset
*/
void distance_mx(DATA dataset, int nbr_objets, int dist_tab[nbr_objets][nbr_objets]) {
  for (size_t i = 0; i < dataset.nbr_objets; i++) {
    for (size_t j = 0; j < dataset.nbr_objets; j++) {
      int buf = 0;

      // Calcul de la valeur dist_tab[i][j]
      for (size_t k=0; k < dataset.size_objets; k++)
        buf = abs(dataset.objets[j].point[k] - dataset.objets[i].point[k]) + buf;

      dist_tab[i][j] = buf;
    }
  }
}


/*
 * Ajoute un objet à un dataset. Retourne 1 si l'allocation n'a pas marché, 0 sinon.
 * DATA sample      : Sample pour lequel il faut ajouter l'objet (Sample car on peut parler d'un cluster)
 * OBJET obj_to_add : objet à ajouter au sample
*/
int add_objet(DATA* sample, OBJET obj_to_add) {
  sample->nbr_objets++;
  // Si le sample est vide on alloue la mémoire pour un objet
  if (sample->nbr_objets-1 == 0)
    sample->objets = malloc(sizeof(OBJET));
  // Si le data set n'est pas vide on réalloue de la mémoire pour l'objet
  else
    sample->objets = realloc(sample->objets, sample->nbr_objets * sizeof(OBJET));

  if (sample->objets == NULL) {
    fprintf(stderr, "Erreur à l'allocation de la memoire (fct add_objet)\n");
    return 1;
  }

  // Ajoue en dernier position (l'espace venant d'être alloué)
  sample->objets[sample->nbr_objets-1] = obj_to_add;

  return 0;
}

/*
 * Cree k clusters selon les rangs des medoids dans le dataset
 * - DATA dataset       : Dataset pour lequel on veut créer les clusters
 * - int* medoid_rank   : Tableau d'entier contenant le rang des medoids du dataset
 * - size_t nbr_cluster : Nombre de clusters voulue
 * - size_t nbr_objet   : Nombre d'objet voulue (ne sert que'à définir la taille de la matrice de
 *                        distance dans les aarguments de la fonction)
 * - int dist           : Matrice de distance du dataset
*/
DATA *create_clusters(DATA dataset, int* medoid_rank, size_t nbr_cluster, size_t nbr_objets, int dist[nbr_objets][nbr_objets]) {
  // # Initialisation des clusters (tableau de DATA)
  DATA *cluster = malloc(sizeof(DATA)*nbr_cluster);
  for(size_t y=0; y < nbr_cluster; y++) {
    init_data(&cluster[y], 0, dataset.size_objets);
    cluster[y].medoid = dataset.objets[medoid_rank[y]];
  }

  size_t cluster_rank;

  // # Creation des clusters
  for (size_t i=0; i < dataset.nbr_objets ; i++) {
    int min = INT_MAX;
    for (size_t k=0; k < nbr_cluster ; k++) {
      if (dist[i][medoid_rank[k]] < min) {
        cluster_rank = k;
        min = dist[i][medoid_rank[k]];
      }
    }

    // Ajout de l'objet dans le cluster adequat
    if(add_objet(&cluster[cluster_rank], dataset.objets[i])) exit(EXIT_FAILURE);
    dataset.objets[i].cluster_id = cluster_rank;
  }

  return cluster;
}


/*
 * Renvoie le coût total de la combinaison selon le rang des medoids.
 * Exemple: si medoid_rank = [23, 12] alors dataset.objets[23] dataset.objets[12] sont des medoids
 * - int* medoids_ranks  : Tableau contenant les rang des medoids
 * - size_t nbr_clusters : Nombre de clusters
 * - size_t nbr_objets   : Nombres d'objets (ne sert que pour définir la taille de la matrice de distance
 *                         dans les arguments de la fonction)
 * - int dist            : Matrice de distance
*/
int get_cost(int* medoid_rank, size_t nbr_clusters, size_t nbr_objets, int dist[nbr_objets][nbr_objets]) {
  int sum=0;

  for (size_t i=0; i<nbr_objets; i++) {
    int min = INT_MAX;
    for (size_t j=0; j<nbr_clusters; j++) {
      if (dist[i][medoid_rank[j]] < min) {
        min = dist[i][medoid_rank[j]];
      }
    }
    sum += min;
  }

  return sum;
}
