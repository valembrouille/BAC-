#include "enseignants.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef DEBUG
#include "tests.h"
#endif

typedef struct s_enseig{
    Enseignant e;
    struct s_enseig* suivant;
}*Enseig;

struct s_enseignants{
    Enseig sentinelle;
    int taille;
};

Enseignants enseignants(){
    Enseignants es= (Enseignants) malloc(sizeof(struct s_enseignants));
    es->sentinelle=(Enseig)malloc(sizeof(struct s_enseig));
    es->sentinelle->suivant=es->sentinelle;
    es->taille=0;
    return es;
}

#ifdef JSON
Enseignants enseignantsParser(json_t *json_enseignants) {
    json_t *json_arr_e = json_object_get(json_enseignants, "enseignants");

    assert(json_is_array(json_arr_e));

    Enseignants es = enseignants();

    size_t index;
    json_t *value;
    // ? https://jansson.readthedocs.io/en/latest/apiref.html#c.json_array_foreach
    json_array_foreach(json_arr_e, index, value) {
        ajouterEs(enseignantParser(value), es);
    }

    return es;
}
#endif


Enseig enseig(Enseignant e, Enseignants es){
    Enseig g=(Enseig) malloc(sizeof(struct s_enseig));
    g->e=e;
    g->suivant=es->sentinelle;
    return g;
}

Enseignants ajouterEs(Enseignant g, Enseignants es){
    Enseig e=enseig(g,es);
    if(!appartient(g,es)){
        e->suivant = es->sentinelle->suivant;
        es->sentinelle->suivant=e;
        es->taille++;
    }
    return es;
}

bool appartient(Enseignant g, Enseignants es){
    Enseig e=enseig(g,es);
    bool b= false;
    Enseig courant= es->sentinelle->suivant;
    for(int i=0; i<es->taille-1; i++){
        if((getNom(courant->e) != getNom(e->e)) && (getMatiere(courant->e) != getMatiere(e->e)) && !b){
            b= true;
        }
        courant=courant->suivant;
    }
    return b;
}

Enseignants supprimerEs(Enseignant g, Enseignants es){
    assert(!es->taille==0);
    Enseig e =enseig(g,es);
    Enseig precedent = es->sentinelle;
    Enseig courant = es->sentinelle->suivant;
    int i=0;
    while((getNom(courant->e) != getNom(g)) && (getMatiere(courant->e) != getMatiere(g)) && (i<es->taille-1)){
        precedent=courant;
        courant=courant->suivant;
        i++;
    }
    e->suivant=courant->suivant;
    precedent->suivant=e;
    free(courant);
    es->taille--;
    return es;
}

void afficherEnseignants (Enseignants es){
    printf("---------------------\n");
    printf("Liste des enseignants\n");
    printf("---------------------\n\n");
    Enseig courant = es->sentinelle->suivant;
    for(int i=0; i<es->taille;i++){
        printf("- ");
        afficheEnseignant(courant->e);
        courant=courant->suivant;
    }
}

Enseignant getEnseignantN(Enseignants es, char *n){
    bool b=false;
    Enseignant e;
    Enseig courant = es->sentinelle->suivant;
    for(int i=0; i<es->taille; i++){
        if(n==getNom(courant->e) && !b){
            b= true;
            e=courant->e;
        }
        courant=courant->suivant;
    }
    return e;
}

Enseignant getEnseignantM(Enseignants es, char *m){
    bool b=false;
    Enseignant e;
    Enseig courant = es->sentinelle->suivant;
    for(int i=0; i<es->taille; i++){
        if(m==getMatiere(courant->e) && !b){
            b= true;
            e=courant->e;
        }
        courant=courant->suivant;
    }
    return e;
}

#ifdef JSON
json_t* getJsonEnseignants(Enseignants es) {

    json_t *root = json_object();
    json_t *json_arr = json_array();

    // la liste des enseignants sera stocké dans un tableau
    json_object_set_new(root, "enseignants", json_arr);

    Enseig courant = es->sentinelle->suivant;

    for(int i = 0; i < es->taille; i++) {
        json_array_append(json_arr, getJsonEnseignant(courant->e));
        courant = courant->suivant;
    }

    return root;
}

char* toStringEnseignants(Enseignants es) {

    json_t *json_enseignants = getJsonEnseignants(es);
    char *str = json_dumps(json_enseignants, 0);

    #ifdef DEBUG
    puts(str);
    #endif

    // deallocate json object memory
    json_decref(json_enseignants);

    return str;
}
#endif

#ifdef TEST
#include <string.h>

int main() {

    // init

    char* e1_nom = "TRUILLET";
    char* e2_nom = "GAILDRAT";
    char* e1_matiere = "Structure de données";
    char* e2_matiere = "Programmation orientée objet";
    Enseignant e1 = enseignant(e1_nom, e1_matiere);
    Enseignant e2 = enseignant(e2_nom, e2_matiere);

    // testing

    Enseignants es = enseignants();

    info(ajouterEs(e1,es));
    info(ajouterEs(e2,es));

    info(supprimerEs(e1,es));
    test(getEnseignantN(es,"GAILDRAT") == e2);
    test(getEnseignantM(es,"Programmation orientée objet") == e2);

    info(afficherEnseignants(es));

    #ifdef JSON
    test(strcmp(toStringEnseignants(es), toStringEnseignants(enseignantsParser(getJsonEnseignants(es)))) == 0);
    #endif


    return 0;
}

#endif