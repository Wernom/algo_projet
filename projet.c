#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define NAME_LENGTH_MIN 3
#define NAME_LENGTH_MAX 10
#define TELEPHONE_LENGTH 8

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

void directory_create(struct directory *self);
//****PARTIE 1****

void directory_data_print(const struct directory_data *data){
  printf("%s %s : %s", data->last_name, data->first_name, data-> telephone);
}

void directory_create(struct directory *self){
  self->size       = 0;
  self->capacity   = 10000;
  self->data = calloc(self->capacity, sizeof(struct directory_data));
}
//****PARTIE 2****
//****PARTIE 3****
//****PARTIE 4****

int main(){
 struct directory d;
 directory_create(&d);
 printf("%p", d.data);
}
