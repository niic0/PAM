#ifndef DATA_H_
#define DATA_H_

#include <stddef.h>
#include <stdbool.h>

// Structure stockant un objet du dataset
typedef struct OBJET {
  size_t rank;                // Rang du point dans la liste d'objet
  int *point;                 // "coordonnees" de l'element
  int cluster_id;             // numero du cluster dans lequel est l'objet, -1 si pas de cluster associé
  char* id;                   // identifiant de l'objet, on parle dans le cas present du nom
} OBJET;

// Structure stockant les données d'un dataset
typedef struct DATA {
  OBJET medoid;               // Point representatif du cluster (si le dataset n'est pas le dataset d'entré)
  size_t nbr_objets;          // nombre d'element
  size_t size_objets;         // taille des elements qui definissent les element
  OBJET *objets;
} DATA;


// Alloue la memoire du dataset. Initialiste sa taille et le nombre de caracteristiques qui
// definissent un element.
void init_data(DATA* sample, size_t nbr_objets, size_t size_objets);

// Creer un dataset aleatoire selon un nombre d'objets demandé, un nombre de valeurs définissant un objet
// demandé et une valeurs maximal pour chaque valeurs définnissantes
DATA set_data_rand(size_t nbr_objets, size_t size_objets, size_t range);

// Transforme un fichier csv (avec header) en une structure DATA
DATA set_data_csv(char* file_name);

// Renvoie le nombre de valeurs qui definnissent un objet d'un dataset compris dans un fichier csv
size_t get_size_objets(char* file_name);

// Renvoie le nombre de valeurs d'un dataset compris dans un fichier csv
size_t get_nbr_objets(char* file_name);

// Crée la matrice de distance d'un dataset. La valeur est renvoyé par le pointeur dist_tab
void distance_mx(DATA dataset, int nbr_objets, int dist_tab[nbr_objets][nbr_objets]);

// Ajoute un objet a un sample (sample et pas dataset car on peut parler d'un cluster = partie d'un dataset)
int add_objet(DATA* sample, OBJET obj_to_add);

// Renvoie un tableau de structure DATA contenant chacune les clusters avec les objets qu'ils contiennent
DATA *create_clusters(DATA dataset, int* medoid_rank, size_t nbr_cluster, size_t nbr_objets, int dist[nbr_objets][nbr_objets]);

// Renvoie le coût total d'une combinaison de medoids
int get_cost(int* medoids_ranks, size_t nbr_clusters, size_t nbr_objets, int dist[nbr_objets][nbr_objets]);


#endif // DATA_H_
