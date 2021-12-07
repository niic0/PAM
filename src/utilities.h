#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "data.h" // pour le type data et elmnt

// Indique si l'objet (indique par son rang idx) est le medoid d'un cluster
int is_medoid(DATA* dist, size_t idx, size_t nbr_cluster);

// Renvoie la chaine de caracteres correspondant au ang num et a la ligne line
char* getfield_csv(char* line, size_t num);

// Retourne un nombre aléatoire entre 0 et le maximum indiqué en évitant des valeurs
int pick_rand_number(size_t max, bool* is_medoid);

// Renvoie 1 si la valeur est un nombre, 0 sinon
int is_number(char* string);

// Affiche les objets d'un dataset, leurs rang et leurs valeurs.
void print_values(DATA dataset);

// affiche un objet d'un dataset
void print_element(OBJET data, size_t size_objet);

// Affiche l'utilisation du programme
void usage();


#endif // UTILITIES_H_
