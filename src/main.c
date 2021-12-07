#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "utilities.h"
#include "data.h"


// Implémentation de l'algorithme PAM avec un affichage simple dans le terminal
DATA* pam(DATA data_set, size_t nbr_cluster);
// Implémentation de l'algorithme PAM avec un affichage de chaque étapes dans le terminal
DATA* pam_verbose(DATA data_set, size_t nbr_cluster);
// Prend en charge les arguments de la commande ./pam
int command(char** argv, DATA** clusters, int* nbr_clusters);


int main(int argc, char *argv[]) {
  DATA* clusters;
  int nbr_cluster;

  // Gestion des arguments
  if (command(argv, &clusters, &nbr_cluster)) {
    puts("Requète invalide");
    usage();
    return 1;
  }

  // Affichage des Medoids trouvés
  printf("\n# Medoids\n");
  for (size_t k=0; k < (size_t)nbr_cluster; k++)
    print_element(clusters[k].medoid, clusters[k].size_objets);

  return 0;
}


/*
 * IMPLEMNTATION DE L'ALGORITHME PAM
 * L'algorithme prend en entrée un dataset et le nombre de clusters voulue
*/
DATA* pam (DATA dataset, size_t nbr_cluster) {
  if (nbr_cluster > dataset.nbr_objets) {
    fprintf(stderr, "Le nombre de clusters voulue est superieur au nombre d'elements ! Sortie du programme...\n");
    exit(EXIT_FAILURE);
  }

  size_t nbr_objets = dataset.nbr_objets;

  bool is_medoid[nbr_objets];                  // Si is_medoid[k] = true alors l'objet k du dataset est un medoid
  memset(is_medoid, false, nbr_objets);

  int distance[nbr_objets][nbr_objets];        // Stocke la matrice de distance du dataset
  distance_mx(dataset, nbr_objets, distance);

  int medoids_ranks[nbr_cluster];              // Stocke le rang des medoids du dataset

  // # Initialisation : Objets de départ pris au hasard
  srand(getpid());
  for(size_t i=0; i<nbr_cluster; i++)
    medoids_ranks[i] = pick_rand_number(nbr_objets, is_medoid);

  size_t cluster_rank; size_t medoid_rank;
  size_t cost_actual = INT_MAX; size_t cost_before = INT_MAX;

  // Calcule du coût initiale avec les medoids pris au hasard
  size_t cost = get_cost(medoids_ranks, nbr_cluster, nbr_objets, distance);

  int medoids_ranks_candidat[nbr_cluster];

  // # SWAP : On cherche maintenant la meilleur position
  // Les valeurs "candidat" sont les valeurs qui pourraient potentiellement remplacer les valeurs "actual".
  // Les valeurs "actual" sont les valeurs qui pourraient potentiellement remplacer les valeurs final
  // (meilleur coût et meilleur medoids)
  while(1) {
    if (cost_before <= cost) break;  // Si la coût ne diminue plus, arreter la boucle
    cost_before = cost;

    for (size_t y=0; y < nbr_cluster; y++) {
      for (size_t k=0; k < nbr_objets; k++) {
        if (is_medoid[k]) continue;

        for (size_t j=0; j < nbr_cluster; j++)
          medoids_ranks_candidat[j] = medoids_ranks[j];

        size_t cost_candidat;
        medoids_ranks_candidat[y] = k;
        cost_candidat = get_cost(medoids_ranks_candidat, nbr_cluster, nbr_objets, distance);

        if (cost_candidat < cost_actual) {
          cluster_rank = k;
          medoid_rank = y;
          cost_actual = cost_candidat;
        }
      }

      if (cost_actual < cost)
        cost = cost_actual;
    }

    is_medoid[medoids_ranks[medoid_rank]] = false;
    medoids_ranks[medoid_rank] = cluster_rank;
    is_medoid[medoids_ranks[medoid_rank]] = true;
  }

  printf("\n//-> Coût final : %ld\n", cost);

  return create_clusters(dataset, medoids_ranks, nbr_cluster, nbr_objets, distance);
}


/*
 * Implémentation de l'algorithme pam identique à la précédente mais cette fois si avec
 * un affichage dans le terminal à chaque étape
*/
DATA* pam_verbose (DATA dataset, size_t nbr_cluster) {
  printf("# DATASET\n");
  print_values(dataset);

  if (nbr_cluster > dataset.nbr_objets) {
    fprintf(stderr, "Le nombre de clusters voulue est superieur au nombre d'elements ! Sortie du programme...\n");
    exit(EXIT_FAILURE);
  }

  size_t nbr_objets = dataset.nbr_objets;
  size_t size_objets = dataset.size_objets;

  bool is_medoid[nbr_objets];
  memset(is_medoid, false, nbr_objets);

  int distance[nbr_objets][nbr_objets];
  distance_mx(dataset, nbr_objets, distance);

  srand(getpid());
  int medoids_ranks[nbr_cluster];

  printf("# Medoids initiaux\n" );
  for(size_t i=0; i<nbr_cluster; i++) {
    medoids_ranks[i] = pick_rand_number(nbr_objets, is_medoid);
    print_element(dataset.objets[medoids_ranks[i]], size_objets);
  }

  size_t cluster_rank; size_t medoid_rank;
  size_t cost_actual = INT_MAX; size_t cost_before = INT_MAX;

  size_t cost = get_cost(medoids_ranks, nbr_cluster, nbr_objets, distance);

  int medoids_ranks_candidat[nbr_cluster];

  while(1) {
    if (cost_before <= cost) break;
    cost_before = cost;

    for (size_t y=0; y < nbr_cluster; y++) {
      for (size_t k=0; k < nbr_objets; k++) {
        if (is_medoid[k]) continue;
        for (size_t j=0; j<nbr_cluster; j++)
          medoids_ranks_candidat[j] = medoids_ranks[j];

        size_t cost_candidat;

        medoids_ranks_candidat[y] = k;
        cost_candidat = get_cost(medoids_ranks_candidat, nbr_cluster, nbr_objets, distance);

        if (cost_candidat < cost_actual) {
          cluster_rank = k;
          medoid_rank = y;
          cost_actual = cost_candidat;
        }
      }
      if (cost_actual < cost)
        cost = cost_actual;
    }

    printf("\n//-> SWAP\n");
    print_element(dataset.objets[medoids_ranks[medoid_rank]], size_objets);
    print_element(dataset.objets[cluster_rank], size_objets);
    printf("Coût %ld => %ld\n", cost_before, cost);
    printf("-//\n\n");

    is_medoid[medoids_ranks[medoid_rank]] = false;
    medoids_ranks[medoid_rank] = cluster_rank;
    is_medoid[medoids_ranks[medoid_rank]] = true;


    for (size_t j=0; j<nbr_cluster; j++)
      print_element(dataset.objets[medoids_ranks[j]], size_objets);
  }

  DATA* clusters = create_clusters(dataset, medoids_ranks, nbr_cluster, nbr_objets, distance);

  printf("\n# Clusters\n");
  for (size_t i=0; i < nbr_cluster; i++) {
    printf("-> cluster[%ld]\n", i+1);
    print_values(clusters[i]);
  }

  printf("\n//-> Coût final : %ld\n", cost);

  return clusters;
}


/*
 * Gestion des arguments de la commande ./pam
 * - char **argv : Tableau contenant les arguments en chaîne de caractères
 * - DATA** clusters : Stocke les clusters qui sont renvoyé via un pointeur
 * - nbr_cluster : Stocke le nombre de clusters entré en argument et la renvoie via un pointeur
*/
int command(char** argv, DATA** clusters, int* nbr_clusters) {
  if (argv[1] == NULL) return 1;

  // Mode sans affichage, n'affiche que le cout final et les clusters associé à ce coût
  if (!strcmp(argv[1], "-f")) {
    if(argv[2] == NULL || argv[3] == NULL) {
      puts("Requète invalide");
      return 1;
    }

    DATA dataset = set_data_csv(argv[2]);
    *nbr_clusters = atoi(argv[3]);

    if ((argv[4]!=NULL) && !strcmp(argv[4], "-v"))
      *clusters = pam_verbose(dataset, (size_t)*nbr_clusters);
    else {
      printf("\n[NOTE]\nLe contenu des clusters peut être obtenue avec la commande: ./pam -f data.csv 4 -v\n");
      *clusters = pam(dataset, (size_t)*nbr_clusters);
    }
  }

  // Mode avec affichage de chaque étape de l'algorithme
  else if (!strcmp(argv[1], "-r")) {
    if(argv[2] == NULL || argv[3] == NULL || argv[4] == NULL || argv[5] == NULL) {
      puts("Requète invalide");
      return 1;
    }
    DATA dataset = set_data_rand(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    *nbr_clusters = atoi(argv[5]);

    if ((argv[6]!=NULL) && !strcmp(argv[6], "-v"))
      *clusters = pam_verbose(dataset, (size_t)*nbr_clusters);
    else
      *clusters = pam(dataset, (size_t)*nbr_clusters);
  }

  return 0;
}
