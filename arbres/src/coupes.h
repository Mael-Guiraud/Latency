/* 
Ce fichier permet de séléctionner les coupes à activer ou à désactiver pour le FPT.
*/
#define NB_COUPES 7

#define AFFICHE_RES 1

///// COUPES A LA FIN D'UN ARC ///////////
//Coupe avec le calcul de borne inf à la fin de chaque arc.
#define BORNEINF_ON 1

//Si une route dans la seconde periode peut être placée dans la première quelque part.
#define SECONDE_DANS_PREMIERE 1

//Si On peut echanger i et j dans la seconde pour que i passe en premiere
#define ECHANGE_SECONDE 1

/////// COUPES AU FUR ET A MESURE DU CALCUL DES ROUTES //////


//Si la route i est collée dans la première periode, on ne fait pas la seconde periode
#define I_COLLE 1
//Si après avoir mis i en première periode, je pourrais faire rentrer une des routes suivantes avant i dans la première periode, je ne fais pas avec i en première periode
#define ROUTES_SUIVANTES_AVANT_I 1
//Si j'ai une route avec 0 de délai qui n'est pas de l'id le plus petit, je ne fait pas l'odre, car je le verrais à un autre moment.
#define PAS_PLUS_PETIT_ID 1

