
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct {
    int nb_lignes;
    int nb_colonnes;
    int** matrice_adjacence;
} Grille;

void lecture_fichier(Grille* G) ;
void remplissage_mat_surv(Grille *G, int **matrice_cible, int **matrice_surveillant);
bool sont_couvertes (int **matrice_cible , Grille *G);
int calcul_nbre_cible_par_lignes(int ligne, int colonne ,int** matrice_cible, int** matrice_surveillant , int nbre_colonne , Grille *G);
int calcul_nbre_cible_par_colonnes(int colonne,int ligne, int** matrice_cible, int** matrice_surveillant , int nbre_ligne , Grille *G);
void max_surv(int **matrice_surveillant, int nbre_lignes, int nbre_colonnes, int *max_i, int *max_j);
void couvrir_les_cibles(Grille *G,int **matrcie_surveillant,int **matrice_cible);
void couvrir_par_colonnes(int colonne, int ligne, int** matrice_cible, int** matrice_surveillant, int nbre_ligne, Grille* G) ;
void couvrir_par_lignes(int ligne, int colonne, int** matrice_cible, int** matrice_surveillant , int nbre_colonne , Grille *G);
int nbre_surv(Grille *G,int **matrice_cible, int **matrice_surveillant);


void lecture_fichier(Grille* G) {
    const char* graph_fic_nom = "td.txt";
    FILE* fp;
    fp = fopen(graph_fic_nom, "r");
    if (fp != NULL) {
        int nb_lignes, nb_colonnes;
        char caractertistique1[30];
        char caractertistique2[30];
        fscanf(fp, "%s %d", caractertistique1, &nb_lignes);
        if (strcmp(caractertistique1, "LIGNES") == 0) {
            G->nb_lignes = nb_lignes;
            //printf("nb lignes %d \n",G->nb_lignes);
        }
        fscanf(fp, "%s %d ",caractertistique2, &nb_colonnes);
        if (strcmp(caractertistique2, "COLONNES") == 0) {
            G->nb_colonnes = nb_colonnes;
            //printf("nb col %d \n",G->nb_colonnes);
        }

        G->matrice_adjacence = (int**)malloc(G->nb_lignes * sizeof(int*));
        for (int i = 0; i < G->nb_lignes; i++)
            G->matrice_adjacence[i] = (int*)malloc(G->nb_colonnes * sizeof(int));

        // Initialisation de la matrice
        for (int i = 0; i < G->nb_lignes; i++)
            for (int j = 0; j < G->nb_colonnes; j++)
                G->matrice_adjacence[i][j] = 0;

        while (!feof(fp)) {
            int x, y;
            char caractertistique[30];
            fscanf(fp, "%s %d %d", caractertistique, &x, &y);
            if (strcmp(caractertistique, "CIBLE") == 0) {
                G->matrice_adjacence[x][y] = 1;
            }
            else {
                G->matrice_adjacence[x][y] = -1;
            }
        }
        //printf("je suis sla \n");
        fclose(fp);
    }
    else {
        printf("Le fichier n'a pas été trouvé.");
    }
}

int main() {
    Grille G;
    lecture_fichier(&G);
    int res=0;

    //matrice cible
    int **matrice_cible =(int **)malloc(G.nb_lignes*sizeof(int*));
    for(int i=0;i<G.nb_lignes;i++){
        matrice_cible[i]=(int *)malloc(G.nb_colonnes*sizeof(int));
    }

    for(int i=0;i<G.nb_lignes;i++){
        for(int j=0;j<G.nb_colonnes;j++){
            matrice_cible[i][j]=0;         //toutes les cibles sont non couvertes
        }
    }

    //matrice surveillant 
    int **matrice_surveillant =(int **)malloc(G.nb_lignes*sizeof(int*));
    for(int i=0;i<G.nb_lignes;i++){
        matrice_surveillant[i]=(int *)malloc(G.nb_colonnes*sizeof(int));
    }

    for(int i=0;i<G.nb_lignes;i++){
        for(int j=0;j<G.nb_colonnes;j++){
            if(G.matrice_adjacence[i][j]==-1){
                matrice_surveillant[i][j]=-1;    // -1 cad le surveillant ne peut pas prendre cette position
            }
            else {
                matrice_surveillant[i][j]=1;  //   1 il peut la prendre
            }
        }
    }
    //printf("cc \n");
    
    res = nbre_surv(&G,matrice_cible, matrice_surveillant);
    printf("%d",res);
    free(matrice_cible);
    free(matrice_surveillant);
    

    return 0;
}



void remplissage_mat_surv(Grille* G, int** matrice_cible, int** matrice_surveillant) {
    for (int i = 0; i < G->nb_lignes; i++) {
        for (int j = 0; j < G->nb_colonnes; j++) {
            if (matrice_surveillant[i][j] == 1) { //on peut prendre ce surveillant
                matrice_surveillant[i][j] = 0;
                matrice_surveillant[i][j] += calcul_nbre_cible_par_lignes(i, j , matrice_cible, matrice_surveillant, G->nb_colonnes, G);
                matrice_surveillant[i][j] += calcul_nbre_cible_par_colonnes(j, i ,matrice_cible, matrice_surveillant, G->nb_lignes, G);
                //if (G->matrice_adjacence[i][j] == 1) {
                 //   matrice_surveillant[i][j] -= 1; // on enlève 1 si la cible existe déjà dans le point de départ (elle est deux fois calculée)
                //}
            }
        }
    }
}


int nbre_surv(Grille *G,int **matrice_cible, int **matrice_surveillant){
    int nbre_surv=0;
    
    while(sont_couvertes(matrice_cible,G)==false){
        //printf("je suis la \n");
        remplissage_mat_surv(G, matrice_cible, matrice_surveillant);
        int maxi,maxj;
        
        //printf("%d %d \n",maxi,maxj);
        max_surv(matrice_surveillant, G->nb_lignes, G->nb_colonnes, &maxi, &maxj);
        //printf("%d   %d   \n",maxi,maxj);
        matrice_surveillant[maxi][maxj]=-1;   //on prend ce surveillant;
        for(int i=0;i<G->nb_lignes;i++){
        for(int j=0;j<G->nb_colonnes;j++){
            //printf("%d   ",matrice_surveillant[i][j] );
        }
        //printf("\n");
    }
    //printf("dsokks \n");
        for(int i=0;i<G->nb_lignes;i++){
            for(int j=0;j<G->nb_colonnes;j++){
                if(matrice_surveillant[i][j]>=0){
                    matrice_surveillant[i][j]=1;
                }
            }
        }
        nbre_surv++;
        couvrir_par_lignes(maxi,maxj, matrice_cible, matrice_surveillant , G->nb_lignes , G);
        couvrir_par_colonnes(maxj, maxi, matrice_cible, matrice_surveillant , G->nb_lignes , G);
        
    }
    return nbre_surv;

}






bool sont_couvertes(int **matrice_cible, Grille *G) {
    for (int i = 0; i < G->nb_lignes; i++) {
        for (int j = 0; j < G->nb_colonnes; j++) {
            if (matrice_cible[i][j] == 0 && G->matrice_adjacence[i][j]==1) {
                return false;
            }
        }
    }
    return true;
}



int calcul_nbre_cible_par_lignes(int ligne, int colonne ,int** matrice_cible, int** matrice_surveillant , int nbre_colonne , Grille *G){            
    int res = 0;
    int i =colonne+1;

    while (i < nbre_colonne && G->matrice_adjacence[ligne][i] != -1) {  //pas de d'obstacle
        if (G->matrice_adjacence[ligne][i] == 1 && matrice_cible[ligne][i] == 0) {    // une cible non couverte
            res++;
        }
        i++;
    }
    i =colonne-1;

    while (i>=0 && G->matrice_adjacence[ligne][i] != -1) {  //pas de d'obstacle
        if (G->matrice_adjacence[ligne][i] == 1 && matrice_cible[ligne][i] == 0) {    // une cible non couverte
            res++;
        }
        i--;
    }
    return res;

}


int calcul_nbre_cible_par_colonnes(int colonne,int ligne, int** matrice_cible, int** matrice_surveillant , int nbre_ligne , Grille *G){            
     int res = 0;
    // if (G->matrice_adjacence[ligne][colonne] == 1 && matrice_cible[ligne][colonne] == 0) {    // une cible non couverte
    //         res++;
    //     }
    int i =ligne;
    while (i < nbre_ligne && G->matrice_adjacence[i][colonne] != -1) {  //pas de d'obstacle
        if (G->matrice_adjacence[i][colonne] == 1 && matrice_cible[i][colonne] == 0) {    // une cible non couverte
            res++;
        }
        i++;
    }
    i =ligne-1;

    while (i >= 0 && G->matrice_adjacence[i][colonne] != -1) {  //pas de d'obstacle
        if (G->matrice_adjacence[i][colonne] == 1 && matrice_cible[i][colonne] == 0) {    // une cible non couverte
            res++;
        }
        i--;
    }
    return res;
}


void max_surv(int **matrice_surveillant, int nbre_lignes, int nbre_colonnes, int *max_i, int *max_j) {
    int max_valeur = matrice_surveillant[0][0];

    for (int i = 0; i < nbre_lignes; i++) {
        for (int j = 0; j < nbre_colonnes; j++) {
            if (matrice_surveillant[i][j] > max_valeur) {
                max_valeur = matrice_surveillant[i][j];
                *max_i = i;
                *max_j = j;
            }
        }
    }
}





void couvrir_par_lignes(int ligne, int colonne, int** matrice_cible, int** matrice_surveillant , int nbre_colonne , Grille *G){
    int i =colonne+1;
    while (i < G->nb_colonnes && G->matrice_adjacence[ligne][i] != -1) {  //pas de d'obstacle
        if (G->matrice_adjacence[ligne][i] == 1) {    // une cible trouvée 
            matrice_cible[ligne][i]=1;                //la cible devient couverte
        }
        i++;
    }
    i =colonne-1;
    while (i >= 0 && G->matrice_adjacence[ligne][i] != -1) {  //pas de d'obstacle
        if (G->matrice_adjacence[ligne][i] == 1) {    // une cible trouvée 
            matrice_cible[ligne][i]=1;                //la cible devient couverte
        }
        i--;
    }

}


void couvrir_par_colonnes(int colonne, int ligne, int** matrice_cible, int** matrice_surveillant, int nbre_ligne, Grille* G) {
    // if(G->matrice_adjacence[ligne][colonne] == 1){
    //     matrice_cible[ligne][colonne] = 1;
    // }
    int i = ligne;
    //printf("%d \n",i);
    //printf("%d \n",i);

    while (i < G->nb_lignes && G->matrice_adjacence[i][colonne] != -1) {  // pas d'obstacle
        if (G->matrice_adjacence[i][colonne] == 1) {    // une cible non couverte
            matrice_cible[i][colonne] = 1;                // la cible devient couverte
        }
        i++;
    }
    
    i = ligne-1;
    //printf("%d \n",i);

    while (i >=0 && G->matrice_adjacence[i][colonne] != -1) {  // pas d'obstacle
        if (G->matrice_adjacence[i][colonne] == 1) {    // une cible non couverte
            matrice_cible[i][colonne] = 1;                // la cible devient couverte
        }
        i--;
    }
}
