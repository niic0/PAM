#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utilities.h"

/*
 * Renvoie 1 si la chaîne de caractères est un nombre, 0 sinon
 * - char* string : Chaîne de caractères contenant le potentien nombre
*/
int is_number(char* string) {
  for (size_t i=0; i<strlen(string)-1; i++)
    if (!isdigit(string[i])) return 0;
  return 1;
}

/*
 * Prend un nombre aleatoire entre 0 et un nombre maximum en évitant les valeurs "true" de check.
 * - size_t max  : Valeur aléatoire maximum
 * - bool* check : si check[k] == true, alors ne pas prendre la valeur
*/
int pick_rand_number(size_t max, bool* check) {
  int ret = rand()%max;
  // Tant que la valeur choisie a déjà étée prise on en prend un autre
  while (check[ret]) ret = rand()%max;

  return ret;
}

/*
 * Affiche toutes les valeurs d'un dataset
 * - DATA data_set : data_set à afficher
*/
void print_values(DATA dataset) {
  for (size_t i=0; i<dataset.nbr_objets; i++) {
    printf("element[%lu] [ ", dataset.objets[i].rank);
    for (size_t k=0; k<dataset.size_objets; k++)
      printf("%d ", dataset.objets[i].point[k]);
    printf("]\n");
  }
  printf("\n");
}

/*
 * Affiche un objet d'un dataset
 * - elmnt data   : donnee a affichee
 * - size_elmnts  : nombre d'element qui caracterisent l'element
*/
void print_element(OBJET obj, size_t size_elmnts) {
  printf("element[%lu] [ ", obj.rank);
  for (size_t i=0; i<size_elmnts; i++)
    printf("%d ", obj.point[i]);
  printf("]\n");
}

/*
 * Affiche l'utilisation du programme
*/
void usage(){
  printf("\n[USAGE]\n"
         "2 manières d'utiliser le programme :"
         " - Le dataset est dans un fichier : ./pam -f <file.csv> <Nombre de clusters>\n"
         " - Création du dataset avec des valeurs aléatoires :\n"
         "     ./pam -r <Nombres objets> <Nombre de points définissant un objet> <Valeur aléatoire maximale d'un point>\n"
         "\n"
         "Pour avoir le mode avec le détail de chaque opération, inscrire -v (verbose) à la fin de la commande.\n"
         "[EXEMPLE] ./pam -f data.csv 4 -v\n"
         "          ./pam -r 100 4 10 10 -v");
}
