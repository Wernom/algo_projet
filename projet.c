#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h> 
#include <stdint.h>

#define NAME_LENGTH_MIN 3
#define NAME_LENGTH_MAX 10
#define TELEPHONE_LENGTH 8
#define DIRECTORY_LENGTH 10
#define BUCKET_LENGTH DIRECTORY_LENGTH*2
#define BUFSIZE sizeof(char)

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

struct index_bucket {
  const struct directory_data *data;
  struct index_bucket *next;
};


typedef size_t (*index_hash_func_t)(const struct directory_data *data);


struct index {
  struct index_bucket **buckets;
  size_t count;
  size_t size;
  index_hash_func_t func;
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
void directory_search(const struct directory *self, const char *last_name);
int directory_data_compare(const char *first, const char *second);
void directory_sort(struct directory *self);
void directory_quick_sort(struct directory *self, size_t i, size_t j);
size_t directory_partition(struct directory *self, size_t i, size_t j);
void directory_search_opt(const struct directory *self, const char *last_name);
size_t fnv_hash(const char *key);
void directory_search_dichotomique(const struct directory *self, const char *last_name, size_t min, size_t max);
struct index_bucket *index_bucket_add(struct index_bucket *self, const struct directory_data *data);
void index_bucket_destroy(struct index_bucket *self);
size_t index_first_name_hash(const struct directory_data *data);
size_t index_telephone_hash(const struct directory_data *data);
void index_create(struct index *self, index_hash_func_t func);
void index_rehash(struct index *self);
void index_add(struct index *self, const struct directory_data *data);
void index_fill_with_directory(struct index *self, const struct directory *dir);

//****PARTIE 1****

//fonction qui affiche les informations d'une entrée
void directory_data_print(const struct directory_data *data){
  printf("-%s %s: %s\n", data->last_name, data->first_name, data->telephone);
}

/*
 *Fonction qui affiche toutes les informations d'un répertoire
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

/*
 *Fonction qui recherche un nom dans un tableau on trié et affiche les resultats.
 */
void directory_search(const struct directory *self, const char *last_name){
  bool b =true;// passe à false si les last_name ne correspondent pas pour éviter l'affichage.
  int k = 0;// si k = 0 à la fin de la fonction permet d'afficher que la fonction n'a rien trouvé.
  
  printf("\n**RECHERCHE**\n");

  for(size_t i = 0; i < self->size; ++i){
    b = !directory_data_compare(self->data[i]->last_name, last_name);
    
    if(b){
      ++k;
      directory_data_print(self->data[i]);
    }
  }

  if(!k){
    printf("last_name non trouvé\n");
  }
  
}

/*
 *Fonction swap.
 */
void directory_swap(struct directory *self, size_t i, size_t j){
  struct directory_data *tmp;

  tmp = self->data[i];
  self->data[i] = self->data[j];
  self->data[j] = tmp;
}

/*
 *Fonction qui compare 2 chaines de caracteres et renvoie -1 si la premiere se trouve avant dans l'alphabet, 1 si la premiere se trouve apres dans l'alphabet et zero si elles sont egales.
 */
int directory_data_compare(const char *first, const char *second){
  size_t j = 0;
  while(!(first[j] == '\0' && second[j] == '\0')){
    if(first[j] > second[j]){
      return 1;
    }
    if(first[j] < second[j]){
      return -1;
    }
        
    ++j;
  }

  return 0;
}

/*
 *Fonction de partition utilisé dans quick sort.
 */
size_t directory_partition(struct directory *self, size_t i, size_t j){
  const size_t pivot_index = (i + j) / 2;
  const struct directory_data *pivot = self->data[pivot_index];
  directory_swap(self, pivot_index, j);
  size_t l = i;
  for(size_t k = i; k < j; ++k){
    if(directory_data_compare(self->data[k]->last_name, pivot->last_name) == -1){
      directory_swap(self, k, l);
      l++;
    }
  }
  directory_swap(self, l, j);

  return l;
}
/*
 *Quick sort.
 */
void directory_quick_sort(struct directory *self, size_t i, size_t j){
  if(i < j){
    size_t p = directory_partition(self, i, j);
    directory_quick_sort(self, i, p);
    directory_quick_sort(self, p + 1, j);
  }
}

/*
 *Fonction qui trie notre repertoire par ordre alphabetique par le last_name à l'aide d'un quick sort.
 */
void directory_sort(struct directory *self){
  directory_quick_sort(self,0, self->size - 1);
}

/*
 *Algorithm qui recherche une chaine de caractere dans un tableau trié (recherche dichotomique)
 */
void directory_search_opt(const struct directory *self, const char *last_name){
  directory_search_dichotomique(self, last_name, 0, self->size);
}
void directory_search_dichotomique(const struct directory *self, const char *last_name, size_t min, size_t max){
  if(min == max){
    printf("Rien n'a été trouvé\n");
    return;
  }

  size_t mid = (min + max)/2;

  if(directory_data_compare(self->data[mid]->last_name, last_name) == 1){
    directory_search_dichotomique(self, last_name, min, mid);
    return;
  }

  if(directory_data_compare(self->data[mid]->last_name, last_name) == -1){
    directory_search_dichotomique(self, last_name, mid + 1, max);
    return;
  }
  
  int j = 0;
  
  while(!directory_data_compare(self->data[mid - j]->last_name, self->data[mid]->last_name)){
    directory_data_print(self->data[mid - j]);
    ++j;
  }
  j = 1;
  
  while(!directory_data_compare(self->data[mid + j]->last_name, self->data[mid]->last_name)){
    directory_data_print(self->data[mid + j]);
    ++j;
  }

}

//****PARTIE 3****

/*
 *Fonction qui ajoute un element dans une liste.
 */
struct index_bucket *index_bucket_add(struct index_bucket *self, const struct directory_data *data){
  struct index_bucket *new = malloc(sizeof(struct index_bucket));
  new->data = data;
  new->next = self;
  return new;
}

/*
 *Fonction qui détruit une liste.
 */
void index_bucket_destroy(struct index_bucket *self){
	if(!self){
		return;
	}
	struct index_bucket *curr = self->next;
  

	while(curr){
		self->next = curr->next;
		free(curr);
		curr = self->next;

	}
	free(self);
}


/*
 *Fonction de hashage FNV-1a 64 bit.
 */
size_t fnv_hash(const char *key){
  uint8_t byte_of_data = 0;
  const uint64_t FNV_offset_basis = 0xcbf29ce484222325;
  const uint64_t FNV_prime = 0x100000001b3;
  size_t i = 0;
  size_t hash = FNV_offset_basis;
  while(key[i] != '\0'){
    byte_of_data = key[i];
    hash = hash ^ byte_of_data;
    hash = hash * FNV_prime;
    ++i;
  }
  return hash;
}

/*
 *Fonction de hashage sur le prenom.
 */
size_t index_first_name_hash(const struct directory_data *data){
  return fnv_hash(data->first_name);
}

/*
 *Fonction de hashage sur le numero de telephone.
 */
size_t index_telephone_hash(const struct directory_data *data){
  return fnv_hash(data->telephone);
}

/*! \brief Fonction qui crée un index vide et initialise la fonction de hashage.
 *
 *  Detailed description of the function
 *
 * \param *self Désigne l'index que l'on veut créer
 * \param func Désigne la fonction de hashage
 * \return 
 */
void index_create(struct index *self, index_hash_func_t func)
{
	self->count = 0;
	self->size = BUCKET_LENGTH;
	self->buckets = calloc(self->size, sizeof(struct index_bucket));
	self->func = func;
}

/*! \brief Détruit un index
 *
 *  Détruit toutes les listes chainées mais pas les entrées du répertoire.
 *
 * \param *self L'index que l'on veut détruire
 * \return 
 */
void index_destroy(struct index *self)
{
	for (size_t i = 0; i < self->size; ++i) {
		index_bucket_destroy(self->buckets[i]);
	}
	free(self->buckets);
	
}

/*! \brief Effectue un reashe
 *
 *  Double la taille du tableau et recalcule tous les indices des éléments déja présents
 *
 * \param *self L'index que l'on veut reashe
 * \return 
 */
void index_rehash(struct index *self)
{
	size_t index = 0;
	self->size *= 2;
	struct index_bucket **other = calloc(self->size, sizeof(struct index_bucket));
	for (size_t i = 0; i < self->size/2; ++i) {
		while(self->buckets[i]){
			index = self->func(self->buckets[i]->data);
			other[index % self->size] = index_bucket_add(other[index % self->size], self->buckets[i]->data);
			self->buckets[i] = self->buckets[i]->next;
		}
	}
	free(self->buckets);
	self->buckets = other;

}


/*! \brief Ajoute une entrée dans l'index
 *
 *  Ajoute une entrée dans l'index et effectue un reashe si le nombre d'element de la table dvisé par le nombre de case du tableau est superieur ou égale à 0.5.
 *
 * \param *self L'index où on veut ajouter un élément
 * \param *data Element que l'on veut ajouter dans l'index
 * \return 
 */
void index_add(struct index *self, const struct directory_data *data)
{
	size_t index = 0;
	float count = (float)self->count;
	float size = (float)self->size;
	if ((count / size) >= 0.5000) {
		index_rehash(self);
	}
	index = self->func(data);
	self->buckets[index % self->size] = index_bucket_add(self->buckets[index % self->size], data);
	++self->count;
}

/*! \brief Affiche un repertoire
 *
 *  
 *
 * \param self Le repertoire que l'on veut afficher
 * \return 
 */
void index_print(struct index *self)
{
	for (size_t i = 0; i < self->size; ++i) {
		struct index_bucket *curr = self->buckets[i];
		printf("%zu >\n", i);
		while(curr){
			directory_data_print(curr->data);
			curr = curr->next;
		}
		printf("NULL\n");
	}
}

/*! \brief Remplit un index aves un répertoire
 *
 *  
 *
 * \param *self L'index que l'on veut remplire
 * \param *dir Repertoire que l'on veut indexer
 * \return 
 */
void index_fill_with_directory(struct index *self, const struct directory *dir)
{
	for (size_t i = 0; i < dir->size; ++i) {
		index_add(self, dir->data[i]);
	}
}

/*! \brief Cherche et affiche les entrées d'un index en fonction du prénom.
 *
 *  
 *
 * \param *self L'index où on veut effectuer la recherche
 * \param *first_name Le prénom que recherché
 * \return 
 */
void index_search_by_first_name(const struct index *self, const char *first_name)
{
	if (*self->func != &index_first_name_hash) {
		printf("ERREUR : mauvais index");
		return;
	}

	int b = 0;
	printf("Le prénom recherché est %s : \n Personne trouvée : \n", first_name);
	size_t index_first_name = fnv_hash(first_name);
	struct index_bucket *curr = self->buckets[index_first_name % self->size];

	while(curr){
		if(!directory_data_compare(curr->data->first_name, first_name)){
			directory_data_print(curr->data);
			++b;
		}
		curr = curr->next;
	}
	
	if (!b) {
		printf("Rien n'a été trouvé\n")	;
	}
}

/*! \brief Cherche et affiche les entrées d'un index en fonction du numero de telephone.
 *
 *  
 *
 * \param *self L'index où on veut effectuer la recherche
 * \param *telephone Le numero de telephone recherché
 * \return 
 */
void index_search_by_telephone(const struct index *self, const char *telephone)
{
	if (*self->func != &index_telephone_hash) {
		printf("ERREUR : mauvais index");
		return;
	}
	
	int b = 0;
	printf("Le numero de telephone recherché est %s : \n Personne trouvée : \n", telephone);
	
	size_t index_telephone = fnv_hash(telephone);
	struct index_bucket *curr = self->buckets[index_telephone % self->size];

	while(curr){
		if(!directory_data_compare(curr->data->telephone, telephone)){
			directory_data_print(curr->data);
			++b;
		}
		curr = curr->next;
	}

	if (!b) {
		printf("Rien n'a été trouvé\n")	;
	}
}

void test(struct index *self){
	printf("%zu", self->count / self->size); //self->size / self->count >= 0.5;
}

//****PART 4****
/*! \brief Nettoie le buffer
 *
 *  
 *
 * \return 
 */
void clean_buffer()
{
	char c;
	while((c = getchar()) != '\n' ){}
	
}
/*! \brief Supprime le caractere de fin de ligne final et le remplace par \0
 *
 *  
 *
 * \param *buf La chaine de caractere que l'on veut modifier
 * \param size La taille de la chaine de caractere
 * \return 
 */
void clean_newline(char *buf, size_t size)
{
	char *p = strchr(buf, '\n');
	if(p){
		printf("1");
		*p = '\0';
	}
	
}



//***MAIN***
int main(){
	struct directory dir;
	struct index index_first_name;
	struct index index_telephone;
	directory_create(&dir);
	index_create(&index_first_name, index_first_name_hash);
	index_create(&index_telephone, index_telephone_hash);
	
	directory_random(&dir, DIRECTORY_LENGTH);

	index_fill_with_directory(&index_first_name, &dir);
	index_fill_with_directory(&index_telephone, &dir);
	
	directory_print(&dir, dir.size);
	printf("****INDEX****\n");
	printf("\nFIRST_NAME\n");
	index_print(&index_first_name);
	/*struct 	index_bucket *p = index_first_name.buckets[0];
	for (size_t i = 0; i < index_first_name.size; ++i) {
		while(p){
			directory_data_print(p->data);
			p = p->next;
		}
	}*/

	printf("\nTELEPHONE\n");
	index_print(&index_telephone);

	/*for (size_t i = 0; i < index_telephone.size; ++i) {
		while(index_telephone.buckets[i]){
			directory_data_print(index_telephone.buckets[i]->data);
			index_telephone.buckets[i] = index_telephone.buckets[i]->next;
		}
	}*/
	
	

	index_search_by_first_name(&index_first_name, "DOJ");
	printf("\n\n");
	index_search_by_telephone(&index_telephone, "26187920");


	printf("\n\n");
	index_destroy(&index_first_name);
	index_destroy(&index_telephone);
	directory_destroy(&dir);

	/*char chaine[5];
	printf("taper un truc : \n>");
	fgets(chaine, sizeof(chaine), stdin);
	clean_newline(chaine, 5);
	printf("\"%s\"", chaine);
	clean_buffer();

	printf("taper un truc : \n>");
	fgets(chaine, sizeof(chaine), stdin);
	clean_newline(chaine, 5);
	printf("\"%s\"", chaine);
	clean_buffer();*/



	return 0;
}
