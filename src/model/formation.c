#include "formation.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef DEBUG
#include "tests.h"
#endif

typedef struct s_edt {
    Creneau creneau;
    struct s_edt* suivant;
} *Edt;

struct s_formation {
    char * nom;
    Edt edt;
    int nbr;
};

Formation formation(char* n){
    Formation f=(Formation)malloc(sizeof(struct s_formation));
    f->edt=malloc(sizeof(sizeof(struct s_edt)));
    f->edt->suivant=f->edt;
    f->nom=n;
    f->nbr=0;
    return f;
}

#ifdef JSON
Formation formationParser(json_t *json_formation) {
    json_t *nom = json_object_get(json_formation, "nom");
    json_t *json_arr_c = json_object_get(json_formation, "creneaux");

    assert(json_is_string(nom) && json_is_array(json_arr_c));

    Formation f = formation((char*) json_string_value(nom));

    size_t index;
    json_t *value;
    // ? https://jansson.readthedocs.io/en/latest/apiref.html#c.json_array_foreach
    json_array_foreach(json_arr_c, index, value) {
        ajouterC(f, creneauParser(value));
    }

    return f;
}
#endif

Edt edt(Creneau c){
    Edt e=(Edt)malloc(sizeof(struct s_edt));
    e->creneau=c;
    return e;
}

bool estComplet(Formation f){
    return f->nbr == 4;
}

Formation ajouterC(Formation f, Creneau c){
    assert(!estComplet(f));
    Edt e=edt(c);
    e->suivant=f->edt->suivant;
    f->edt->suivant=e;
    f->nbr++;
    return f;
}

Formation supprimerH (Formation f, Horaire h){
    assert(f->nbr!=0);
    Edt precedent=f->edt;
    Edt courant=f->edt->suivant;
    int i=0;
    bool isFound = false;
    while(i<f->nbr && !isFound){
        isFound = getDebut(getH(courant->creneau)) == getDebut(h) && getFin(getH(courant->creneau)) == getFin(h);
        precedent=courant;
        courant=courant->suivant;
        i++;
    }
    if(isFound) {
        precedent->suivant = courant->suivant;
        free(courant);
    }
    return f;
}

void afficheFormation(Formation f){
    Edt courant=f->edt;
    printf("-----------\n");
    printf("EDT %s\n", f->nom);
    printf("-----------\n\n");
    for(int i=0; i< f->nbr; i++){
        printf("de ");
        afficheHoraire(getH(courant->creneau));
        if(f->edt==NULL){
            printf("VIDE");
        }else{
            printf("%s",getS(courant->creneau));
            afficheEnseignant(getE(courant->creneau));
        }
        printf("\n\n");
        courant=courant->suivant;
    }
}

#ifdef JSON
// TODO à tester
json_t* getJsonFormation(Formation f) {

void afficheFormation(Formation f){
    printf("-----------\n");
    printf("Formation : %s\n", f->nom);
    printf("-----------\n\n");
    Edt courant = f->edt->suivant;
    for(int i=0;i<f->nbr;i++){
        afficheCreneau(courant->creneau);
        courant = courant->suivant;
    }
}

#ifdef JSON
json_t* getJsonFormation(Formation f) {

    json_t *root = json_object();
    json_t *json_arr = json_array();

    json_object_set_new(root, "nom", json_string(f->nom));
    json_object_set_new(root, "creneaux", json_arr);

    Edt courant = f->edt->suivant;
    for(int i = 0; i < f->nbr; i++) {
        json_array_append(json_arr, getJsonCreneau(courant->creneau));
        courant = courant->suivant;
    }

    return root;
}

char* toStringFormation(Formation f) {

    json_t *json_formation = getJsonFormation(f);
    char *str = json_dumps(json_formation, 0);

    #ifdef DEBUG
    puts(str);
    #endif

    // deallocation json object memory
    json_decref(json_formation);

    return str;
}
#endif

#ifdef TEST
#include <string.h>

int main() {

    // init

    Horaire h1 = horaire(13, 15);
    Enseignant e1 = enseignant("TRUILLET", "Structure de données");
    char* f1_nom = "CUPGE";
    char* s1 = "118";

    Horaire h2 = horaire(16, 18);
    Enseignant e2 = enseignant("GAILDRAT", "Programmation orientée objet");
    char* f2_nom = "L2 Informatique";
    char* s2 = "103";

    Creneau c1 = creneau(e1, h1, f1_nom, s1);
    Creneau c1_bis = creneau(e2, h1, f1_nom, s2);
    Creneau c2 = creneau(e2, h2, f2_nom, s2);

    // testing

    Formation f = formation(f1_nom);

    // info(ajouterC(f, c2)); // devrait produire une erreur

    info(ajouterC(f, c1));

    info(afficheFormation(f));

    // info(ajouterC(f, c1_bis)); // devrait produire une erreur

    // info(ajouterC(f, c1));
    // info(ajouterC(f, c1));
    // info(ajouterC(f, c1));

    test(estComplet(f) == false);

    // info(supprimerH(f, h2)); // devrait produire une erreur
    info(supprimerH(f, h1));

    info(ajouterC(f, c1_bis));

    #ifdef JSON
    test(strcmp(toStringFormation(f), toStringFormation(formationParser(getJsonFormation(f)))) == 0);
    #endif

    return 0;
}

#endif