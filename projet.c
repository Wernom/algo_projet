#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define NAME_LENGTH_MIN 3
#define NAME_LENGTH_MAX 10
#define TELEPHONE_LENGTH 8
#define DIRECTORY_LENGTH 1001

//**Structures**

struct directory_data{
  char last_name[NAME_LENGTH_MAX + 1];
  char first_name[NAME_LENGTH_MAX + 1];
  char telephone[TELEPHONE_LENGTH + 1];
};

struct directory {
  struct directory_data **data;
  size_t size;
  size_t capacity;
};

//***Prototypes**

void directory_data_print(const struct directory_data *data);
void directory_print(struct directory *self, size_t n);
void directory_data_name_random(char *data);
void directory_data_telephone_random(char *data);
void directory_data_random( struct directory_data *data);
void directory_create(struct directory *self);
void directory_add(struct directory *self, struct directory_data *data);
void directory_random(struct directory *self, size_t n);

//****PARTIE 1****

//fonction qui affiche les informations d'une entrée
void directory_data_print(const struct directory_data *data){
  printf("-%s %s: %s\n", data->last_name, data->first_name, data->telephone);
}

/*
 *Fonction qui affiche toute les informations d'un répertoire
 */
void directory_print(struct directory *self, size_t n){
  for(int i = 0; i < n; ++i){
    directory_data_print(self->data[i]);
  }
}


//fonction qui génère un nom aleatoirement.
void directory_data_name_random( char *data){
  int size = rand()%(NAME_LENGTH_MAX + 1 - NAME_LENGTH_MIN) + NAME_LENGTH_MIN;//on genere un nombre entre [3,10].
  char consonne [] = {'B','C','D','F','G','H','J','L','M','N','P','R','S','T','V'};//taille 15
  char voyelle [] = {'A','E','I','O','U'};//taille 5
  int select = 0;
  
  for(int i = 0; i < size; i +=2 ){
    select = rand()%15;//permet de selectionner une consonne dans le tableau de consonne
    data[i] = *(consonne + select);
    select = rand()%5;//permet de selectionner une voyelle dans le tableau de voyelle
    data[i+1] = *(voyelle + select);
  }

  data[size] = '\0';
}

/*
 *Fonction qui génère un numero de telephone aléatoire.
 */
void directory_data_telephone_random(char *data){
  for(int i = 0; i < TELEPHONE_LENGTH; ++i){
    data[i] = rand()%10 + '0';
  }
  data[TELEPHONE_LENGTH] = '\0';
}

//fonction qui remplit une entrée avec des données aléatoires.
void directory_data_random(struct directory_data *data){
  directory_data_name_random(data->last_name);
  directory_data_name_random(data->first_name);
  directory_data_telephone_random(data->telephone);
}

/*
 *Fonction qui initialise un reperoire.
 */
void directory_create(struct directory *self){
  self->size = 0;
  self->capacity = 1000;
  self->data = calloc(self->capacity, sizeof(struct directory_data));
}

/*
 *fonction qui détruit un repertoire
 */
void directory_destroy(struct directory *self){
  for(int i = 0; i < self->size; ++i){
    free(*(self->data + i));
  }
  free(self->data);
}

/*
 *Fonction qui ajoute une entrée.
 */
void directory_add(struct directory *self, struct directory_data *data){
  if(self->size == self->capacity){
    self->capacity *= 2;
    struct directory_data **other = calloc(self->capacity, sizeof(struct directory_data));
    memcpy(other, self->data, self->size * sizeof(struct directory_data));
    free(self->data);
    self->data = other;
  }
  self->data[self->size] = data;
  ++self->size;
}

/*
 *Fonction qui génère un répertoire avec n entrée au hasard.
 */
void directory_random(struct directory *self, size_t n){
  for(int i = 0; i < n; ++i){
    struct directory_data *new = malloc(sizeof(struct directory_data));
    directory_data_random(new);
    directory_add(self, new);
  }
}

//****PARTIE 2****
//****PARTIE 3****
//****PARTIE 4****

int main(){
  struct directory dir;
  directory_create(&dir);
  directory_random(&dir, DIRECTORY_LENGTH);
  directory_print(&dir, DIRECTORY_LENGTH);
  
  directory_destroy(&dir);
  return 0;
}
