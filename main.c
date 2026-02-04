#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#define MAX_ID 20
#define MAX_ENTIER 11
#define TAILLE_TABLE_SYM 100
#define MAX_CHAINE 256

// =====================================================
// TYPES POUR L'ANALYSE SEMANTIQUE
// =====================================================
typedef enum {
    TPROG,      // Programme
    TCONST,     // Constante
    TVAR,       // Variable
    TSTRUCT,    // Champ de structure
    TTYPE       // Type défini
} TSYM;

typedef enum {
    TYPE_ENTIER,
    TYPE_REEL,
    TYPE_CARACTERE,
    TYPE_CHAINE,
    TYPE_BOOLEEN,
    TYPE_TABLEAU,
    TYPE_STRUCT,
    TYPE_UNDEFINED
} TYPE_DONNEES;

typedef struct {
    char NOM[20];
    TSYM TIDF;
    TYPE_DONNEES TYPE;
    TYPE_DONNEES TYPE_ELEM;   // Pour les tableaux : type des elements
    int REF_TYPE;             // Pour les variables STRUCT : index du type (TTYPE)
    int VAL;            // Pour les constantes
    int ADRESSE;        // Pour les variables
    int TAILLE;         // Pour les tableaux/structures
    int OFFSET;         // Pour les champs de structure
    struct {
        char NOM[20];
        TYPE_DONNEES TYPE;
    } CHAMPS[20];       // Pour les structures
    int NB_CHAMPS;
} T_TAB_IDF;

// =====================================================
// ERREURS LEXICALES
// =====================================================

typedef enum {
    ERR_CAR_INC,      // caractère inconnu
    ERR_FICH_VID,     // fichier vide
    ERR_ID_LONG,      // identificateur trop long
    ERR_NUM_LONG,     // constante numérique trop longue
    ERR_CHAINE_INC    // chaîne non fermée
} Erreurs_Lexicaux;

// Table des messages d'erreur lexicale
typedef struct {
    Erreurs_Lexicaux CODE_ERR;
    char mes[40];
} Message_Erreur_Lexical;

// =====================================================
// ERREURS SYNTAXIQUES
// =====================================================

typedef enum {
    SYN_ERR_ALGORITHME,
    SYN_ERR_ID,
    SYN_ERR_DEBUT,
    SYN_ERR_FIN,
    SYN_ERR_POINT_VIRGULE,
    SYN_ERR_PAR_FERM,
    SYN_ERR_CROCHET_FERM,
    SYN_ERR_CROCHET_OUV,
    SYN_ERR_DEUX_POINTS,
    SYN_ERR_VIRGULE,
    SYN_ERR_AFFECTATION,
    SYN_ERR_EGAL,
    SYN_ERR_CONSTANTE,
    SYN_ERR_VARIABLES,
    SYN_ERR_TYPE,
    SYN_ERR_TABLEAU,
    SYN_ERR_STRUCTURE,
    SYN_ERR_FIN_STRUCTURE,
    SYN_ERR_SI,
    SYN_ERR_ALORS,
    SYN_ERR_SELON,
    SYN_ERR_FIN_SELON,
    SYN_ERR_POUR,
    SYN_ERR_FIN_POUR,
    SYN_ERR_TANT_QUE,
    SYN_ERR_FIN_TANT_QUE,
    SYN_ERR_REPETER,
    SYN_ERR_JUSQUA_QUE,
    SYN_ERR_ECRIRE,
    SYN_ERR_LIRE,
    SYN_ERR_RELOP,
    SYN_ERR_SORTIR,
    SYN_ERR_AUTRE_CAS,
    SYN_ERR_VALEUR,
    SYN_ERR_SIGNE,
    SYN_ERR_FACT,
    SYN_ERR_NUM,
    SYN_ERR_NON_TERM,
    SYN_ERR_DE_TYPE
} Erreur_Syntaxique_Code;

// Table des messages d'erreur syntaxique
typedef struct {
    Erreur_Syntaxique_Code CODE_ERR;
    char mes[40];
} Message_Erreur_Syntaxique;

// =====================================================
// ERREURS SEMANTIQUES
// =====================================================

typedef enum {
    SEM_ERR_DOUBLE_DECL,      // Double déclaration
    SEM_ERR_ID_NON_DECL,      // Identificateur non déclaré
    SEM_ERR_CONST_MODIF,      // Tentative de modification d'une constante
    SEM_ERR_TYPE_MISMATCH,    // Incompatibilité de type
    SEM_ERR_PROG_ID_USE,      // Utilisation du nom du programme
    SEM_ERR_STRUCT_CHAMP,     // Champ de structure inexistant
    SEM_ERR_INDEX_TAB,        // Indice de tableau invalide
    SEM_ERR_ID_TROP_LONG      // Identificateur trop long
} Erreur_Semantique_Code;

typedef struct {
    Erreur_Semantique_Code CODE_ERR;
    char mes[50];
} Message_Erreur_Semantique;

// =====================================================
// TOKENS DU PSEUDO-CODE
// =====================================================

typedef enum {
    // Mots-clés algorithmiques
    ALGORITHME_TOKEN = 1,
    DEBUT_TOKEN,
    FIN_TOKEN,
    LIRE_TOKEN,
    ECRIRE_TOKEN,
    VARIABLES_TOKEN,
    CONSTANTE_TOKEN,
    STRUCTURE_TOKEN,
    FIN_STRUCTURE_TOKEN,

    // Instructions de contrôle
    SI_TOKEN,
    ALORS_TOKEN,
    SINON_TOKEN,
    FIN_SI_TOKEN,

    // SELON/CAS
    SELON_TOKEN,
    CAS_TOKEN,
    DEUX_POINTS_TOKEN,
    SORTIR_TOKEN,
    AUTRE_CAS_TOKEN,
    FIN_SELON_TOKEN,

    // Boucles
    POUR_TOKEN,
    JUSQUA_TOKEN,
    PAS_TOKEN,
    FAIRE_TOKEN,
    FIN_POUR_TOKEN,
    TANT_QUE_TOKEN,
    FIN_TANT_QUE_TOKEN,
    REPETER_TOKEN,
    JUSQUA_QUE_TOKEN,

    // Types de données
    ENTIER_TOKEN,
    REEL_TOKEN,
    CARACTERE_TOKEN,
    CHAINE_TOKEN,
    BOOLEEN_TOKEN,
    TABLEAU_TOKEN,
    DE_TYPE_TOKEN,

    // Opérateurs
    AFFECTATION_TOKEN,
    PLUS_TOKEN,
    MOINS_TOKEN,
    MULT_TOKEN,
    DIV_TOKEN,
    MOD_TOKEN,

    // Opérateurs relationnels
    EGAL_TOKEN,
    DIFFERENT_TOKEN,
    INFERIEUR_TOKEN,
    INFERIEUR_EGAL_TOKEN,
    SUPERIEUR_TOKEN,
    SUPERIEUR_EGAL_TOKEN,

    // Opérateurs logiques
    ET_TOKEN,
    OU_TOKEN,
    NON_TOKEN,

    // Séparateurs
    PARENTHESE_OUVRANTE_TOKEN,
    PARENTHESE_FERMANTE_TOKEN,
    CROCHET_OUVRANT_TOKEN,
    CROCHET_FERMANT_TOKEN,
    VIRGULE_TOKEN,
    POINT_VIRGULE_TOKEN,
    POINT_TOKEN,

    // Identifiants et constantes
    ID_TOKEN,
    NUM_ENTIER_TOKEN,
    NUM_REEL_TOKEN,
    CHAINE_LITTERALE_TOKEN,
    BOOLEEN_LITTERALE_TOKEN,

    // Fin de fichier
    FIN_FICHIER_TOKEN,

    // Erreur
    ERREUR_TOKEN
} CODE_LEX;

// =====================================================
// STRUCTURES DE DONNEES
// =====================================================

// Structure pour un symbole (token)
typedef struct {
    CODE_LEX CODE;
    char NOM[MAX_ID + 1];
    double val;
} TSym_Cour;

// Table des mots-clés du pseudo-code
typedef struct {
    char nom[20];
    CODE_LEX code;
} MotCle;

// =====================================================
// VARIABLES GLOBALES
// =====================================================

FILE *Fichier;
char Car_Cour;
TSym_Cour Sym_Cour;

// Table des symboles sémantique
T_TAB_IDF TAB_SYM[TAILLE_TABLE_SYM];
int NBR_SYM = 0;
int OFFSET = 0;
int AVANT_BEGIN = 1;  // 1 = avant BEGIN, 0 = après BEGIN
int INDEX_DER_SYM_ACC = -1;
char NOM_PROGRAMME[20] = "";
int NB_ERREURS_SEMANTIQUES = 0;

// Variables pour la vérification des types
TYPE_DONNEES TYPE_EXPR_COURANT = TYPE_UNDEFINED;
TYPE_DONNEES TYPE_CIBLE_AFFEC = TYPE_UNDEFINED; // type de la variable cible dans une affectation

// Pour relier une variable STRUCT au bon type (structure_1, structure_2, ...)
int DERNIER_TYPE_STRUCT_INDEX = -1;

// Pour générer correctement l'affectation des chaînes : on capture le "lvalue" (cible)
int CAPTURE_LVALUE = 0;
char LVALUE_BUF[256];

// Variables pour la génération de code
FILE *Fichier_C = NULL;
int NIVEAU_INDENTATION = 0;
int MODE_GENERATION = 0;  // 0 = analyse, 1 = génération

// Variables temporaires pour ECRIRE
char FORMAT_PRINTF[512];
char ARGS_PRINTF[512];
int FORMAT_INDEX = 0;
int ARGS_INDEX = 0;

// =====================================================
// TABLES DES MESSAGES D'ERREUR
// =====================================================

Message_Erreur_Lexical MES_ERR[] = {
    {ERR_CAR_INC, "caractère inconnu"},
    {ERR_FICH_VID, "fichier vide"},
    {ERR_ID_LONG, "identificateur trop long"},
    {ERR_NUM_LONG, "constante numérique trop longue"},
    {ERR_CHAINE_INC, "chaîne non fermée"}
};

Message_Erreur_Syntaxique SYN_MES_ERR[] = {
    {SYN_ERR_ALGORITHME, "'algorithme' attendu"},
    {SYN_ERR_ID, "identificateur attendu"},
    {SYN_ERR_DEBUT, "'debut' attendu"},
    {SYN_ERR_FIN, "'fin' attendu"},
    {SYN_ERR_POINT_VIRGULE, "';' attendu"},
    {SYN_ERR_PAR_FERM, "')' attendu"},
    {SYN_ERR_CROCHET_FERM, "']' attendu"},
    {SYN_ERR_CROCHET_OUV, "'[' attendu"},
    {SYN_ERR_DEUX_POINTS, "':' attendu"},
    {SYN_ERR_VIRGULE, "',' attendu"},
    {SYN_ERR_AFFECTATION, "':=' attendu"},
    {SYN_ERR_EGAL, "'=' attendu"},
    {SYN_ERR_CONSTANTE, "'constante' attendu"},
    {SYN_ERR_VARIABLES, "'variables' attendu"},
    {SYN_ERR_TYPE, "type attendu"},
    {SYN_ERR_TABLEAU, "'tableau' attendu"},
    {SYN_ERR_STRUCTURE, "'structure' attendu"},
    {SYN_ERR_FIN_STRUCTURE, "'fin_structure' attendu"},
    {SYN_ERR_SI, "'si' attendu"},
    {SYN_ERR_ALORS, "'alors' attendu"},
    {SYN_ERR_SELON, "'selon' attendu"},
    {SYN_ERR_FIN_SELON, "'fin_selon' attendu"},
    {SYN_ERR_POUR, "'pour' attendu"},
    {SYN_ERR_FIN_POUR, "'fin_pour' attendu"},
    {SYN_ERR_TANT_QUE, "'tant_que' attendu"},
    {SYN_ERR_FIN_TANT_QUE, "'fin_tant_que' attendu"},
    {SYN_ERR_REPETER, "'repeter' attendu"},
    {SYN_ERR_JUSQUA_QUE, "'jusqua_que' attendu"},
    {SYN_ERR_ECRIRE, "'ecrire' attendu"},
    {SYN_ERR_LIRE, "'lire' attendu"},
    {SYN_ERR_RELOP, "opérateur relationnel attendu"},
    {SYN_ERR_SORTIR, "'sortir' attendu"},
    {SYN_ERR_AUTRE_CAS, "'autres' attendu"},
    {SYN_ERR_VALEUR, "valeur attendue"},
    {SYN_ERR_SIGNE, "'+' ou '-' attendu"},
    {SYN_ERR_FACT, "facteur attendu"},
    {SYN_ERR_NUM, "nombre entier ou réel attendu"},
    {SYN_ERR_NON_TERM, "instruction non terminée"},
    {SYN_ERR_DE_TYPE, "'de' attendu"}
};

Message_Erreur_Semantique SEM_MES_ERR[] = {
    {SEM_ERR_DOUBLE_DECL, "Double déclaration de l'identificateur"},
    {SEM_ERR_ID_NON_DECL, "Identificateur non déclaré"},
    {SEM_ERR_CONST_MODIF, "Tentative de modifier une constante"},
    {SEM_ERR_TYPE_MISMATCH, "Incompatibilite de type"},
    {SEM_ERR_PROG_ID_USE, "Utilisation du nom du programme interdit"},
    {SEM_ERR_STRUCT_CHAMP, "Champ de structure inexistant"},
    {SEM_ERR_INDEX_TAB, "Indice de tableau invalide"},
    {SEM_ERR_ID_TROP_LONG, "Identificateur trop long"}
};

// =====================================================
// TABLE DES MOTS-CLES
// =====================================================

MotCle tableMotsCles[] = {
    {"algorithme", ALGORITHME_TOKEN},
    {"debut", DEBUT_TOKEN},
    {"fin", FIN_TOKEN},
    {"lire", LIRE_TOKEN},
    {"ecrire", ECRIRE_TOKEN},
    {"variables", VARIABLES_TOKEN},
    {"constante", CONSTANTE_TOKEN},
    {"structure", STRUCTURE_TOKEN},
    {"fin_structure", FIN_STRUCTURE_TOKEN},
    {"si", SI_TOKEN},
    {"alors", ALORS_TOKEN},
    {"sinon", SINON_TOKEN},
    {"fin_si", FIN_SI_TOKEN},
    {"selon", SELON_TOKEN},
    {"cas", CAS_TOKEN},
    {"sortir", SORTIR_TOKEN},
    {"autres", AUTRE_CAS_TOKEN},
    {"fin_selon", FIN_SELON_TOKEN},
    {"pour", POUR_TOKEN},
    {"jusqua", JUSQUA_TOKEN},
    {"pas", PAS_TOKEN},
    {"faire", FAIRE_TOKEN},
    {"fin_pour", FIN_POUR_TOKEN},
    {"tant_que", TANT_QUE_TOKEN},
    {"fin_tant_que", FIN_TANT_QUE_TOKEN},
    {"repeter", REPETER_TOKEN},
    {"jusqua_que", JUSQUA_QUE_TOKEN},
    {"entier", ENTIER_TOKEN},
    {"reel", REEL_TOKEN},
    {"caractere", CARACTERE_TOKEN},
    {"chaine", CHAINE_TOKEN},
    {"booleen", BOOLEEN_TOKEN},
    {"tableau", TABLEAU_TOKEN},
    {"de", DE_TYPE_TOKEN},
    {"et", ET_TOKEN},
    {"ou", OU_TOKEN},
    {"non", NON_TOKEN},
    {"vrai", BOOLEEN_LITTERALE_TOKEN},
    {"faux", BOOLEEN_LITTERALE_TOKEN},
    {"mod", MOD_TOKEN},
    {"", FIN_FICHIER_TOKEN}
};

// =====================================================
// FONCTIONS DE GENERATION DE CODE C
// =====================================================

void Emettre(char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(Fichier_C, format, args);
    va_end(args);
}

void EmettreIndente(char *format, ...) {
    // Ajouter l'indentation
    for (int i = 0; i < NIVEAU_INDENTATION; i++) {
        fprintf(Fichier_C, "    ");
    }

    va_list args;
    va_start(args, format);
    vfprintf(Fichier_C, format, args);
    va_end(args);
}

void EmettreLigne(char *format, ...) {
    // Ajouter l'indentation
    for (int i = 0; i < NIVEAU_INDENTATION; i++) {
        fprintf(Fichier_C, "    ");
    }

    va_list args;
    va_start(args, format);
    vfprintf(Fichier_C, format, args);
    va_end(args);

    fprintf(Fichier_C, "\n");
}

// Convertir le type pseudo-code en type C
char* Type_Vers_C(TYPE_DONNEES type) {
    switch (type) {
        case TYPE_ENTIER: return "int";
        case TYPE_REEL: return "double";
        case TYPE_CARACTERE: return "char";
        case TYPE_CHAINE: return "char";
        case TYPE_BOOLEEN: return "bool";
        case TYPE_TABLEAU: return "array";
        case TYPE_STRUCT: return "struct";
        default: return "int";
    }
}

// Générer le code C pour une variable
void Generer_Declaration_Variable(char *nom, TYPE_DONNEES type, TYPE_DONNEES type_elem, int taille) {
    if (type == TYPE_CHAINE) {
        EmettreLigne("char %s[%d];", nom, MAX_CHAINE);
    } else if (type == TYPE_TABLEAU) {
        EmettreLigne("%s %s[%d];", Type_Vers_C(TYPE_ENTIER), nom, taille);
    } else {
        EmettreLigne("%s %s;", Type_Vers_C(type), nom);
    }
}

// Générer le code C pour une constante
void Generer_Declaration_Constante(char *nom, int valeur) {
    EmettreLigne("#define %s %d", nom, valeur);
}

// Générer une définition de structure C à partir d'un symbole TTYPE/TYPE_STRUCT
void Generer_Definition_Structure(int type_index) {
    if (type_index < 0 || type_index >= NBR_SYM) return;
    if (TAB_SYM[type_index].TIDF != TTYPE || TAB_SYM[type_index].TYPE != TYPE_STRUCT) return;

    EmettreLigne("typedef struct {");
    NIVEAU_INDENTATION++;
    for (int j = 0; j < TAB_SYM[type_index].NB_CHAMPS; j++) {
        char *nom_champ = TAB_SYM[type_index].CHAMPS[j].NOM;
        TYPE_DONNEES t = TAB_SYM[type_index].CHAMPS[j].TYPE;
        if (t == TYPE_CHAINE) {
            EmettreLigne("char %s[%d];", nom_champ, MAX_CHAINE);
        } else {
            EmettreLigne("%s %s;", Type_Vers_C(t), nom_champ);
        }
    }
    NIVEAU_INDENTATION--;
    EmettreLigne("} %s;", TAB_SYM[type_index].NOM);
    EmettreLigne("");
}

// Générer les déclarations globales C
void Generer_Declarations_Globales() {
    EmettreLigne("#include <stdio.h>");
    EmettreLigne("#include <stdlib.h>");
    EmettreLigne("#include <string.h>");
    EmettreLigne("#include <stdbool.h>");
    EmettreLigne("");

    // Définir les structures (typedef)
    for (int i = 0; i < NBR_SYM; i++) {
        if (TAB_SYM[i].TIDF == TTYPE && TAB_SYM[i].TYPE == TYPE_STRUCT) {
            Generer_Definition_Structure(i);
        }
    }

    // Déclarer les constantes
    for (int i = 0; i < NBR_SYM; i++) {
        if (TAB_SYM[i].TIDF == TCONST) {
            Generer_Declaration_Constante(TAB_SYM[i].NOM, TAB_SYM[i].VAL);
        }
    }
    EmettreLigne("");

    // Déclarer les variables
    for (int i = 0; i < NBR_SYM; i++) {
        if (TAB_SYM[i].TIDF == TVAR) {
            if (TAB_SYM[i].TYPE == TYPE_STRUCT && TAB_SYM[i].REF_TYPE >= 0 && TAB_SYM[i].REF_TYPE < NBR_SYM) {
                // Variable de structure: utiliser le typedef généré
                EmettreLigne("%s %s;", TAB_SYM[TAB_SYM[i].REF_TYPE].NOM, TAB_SYM[i].NOM);
            } else {
                Generer_Declaration_Variable(TAB_SYM[i].NOM, TAB_SYM[i].TYPE, TAB_SYM[i].TYPE_ELEM, TAB_SYM[i].TAILLE);
            }
        }
    }
    EmettreLigne("");
}

// Initialiser les buffers pour printf
void Init_Printf_Buffers() {
    FORMAT_INDEX = 0;
    ARGS_INDEX = 0;
    FORMAT_PRINTF[0] = '\0';
    ARGS_PRINTF[0] = '\0';
}

// Ajouter un élément au format printf
void Ajouter_Format_Printf(char *format) {
    strcat(FORMAT_PRINTF + FORMAT_INDEX, format);
    FORMAT_INDEX += strlen(format);
}

// Ajouter un argument à la liste d'arguments printf
void Ajouter_Argument_Printf(char *arg) {
    if (ARGS_INDEX > 0) {
        strcat(ARGS_PRINTF + ARGS_INDEX, ", ");
        ARGS_INDEX += 2;
    }
    strcat(ARGS_PRINTF + ARGS_INDEX, arg);
    ARGS_INDEX += strlen(arg);
}
// =====================================================
// FONCTIONS UTILITAIRES
// =====================================================

#ifdef _WIN32
int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = tolower(*s1);
        char c2 = tolower(*s2);
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return tolower(*s1) - tolower(*s2);
}
#endif

// =====================================================
// FONCTIONS POUR LA VERIFICATION DES TYPES
// =====================================================

// Fonction pour vérifier la compatibilité des types
int Types_Compatibles(TYPE_DONNEES type_dest, TYPE_DONNEES type_src, int pour_affectation) {
    // Mêmes types → toujours compatibles
    if (type_dest == type_src) return 1;

    // Pour les affectations : règles strictes
    if (pour_affectation) {
        // Entier peut recevoir un booléen (0/1)
        if (type_dest == TYPE_ENTIER && type_src == TYPE_BOOLEEN) return 1;

        // Réel peut recevoir un entier (conversion implicite)
        if (type_dest == TYPE_REEL && type_src == TYPE_ENTIER) return 1;

        // Booléen peut recevoir un entier (0/1)
        if (type_dest == TYPE_BOOLEEN && type_src == TYPE_ENTIER) return 1;

        // Autres cas : incompatibles
        return 0;
    }

    // Pour les opérations : règles plus permissives
    if (type_dest == TYPE_ENTIER && type_src == TYPE_REEL) return 1;  // entier opéré avec réel
    if (type_dest == TYPE_REEL && type_src == TYPE_ENTIER) return 1;  // réel opéré avec entier
    if (type_dest == TYPE_ENTIER && type_src == TYPE_BOOLEEN) return 1; // entier avec booléen
    if (type_dest == TYPE_BOOLEEN && type_src == TYPE_ENTIER) return 1; // booléen avec entier

    // Types UNDEFINED acceptés partout (pour les déclarations)
    if (type_dest == TYPE_UNDEFINED || type_src == TYPE_UNDEFINED) return 1;

    return 0;
}

// Fonction pour obtenir le nom d'un type en chaîne
char* Nom_Type(TYPE_DONNEES type) {
    switch (type) {
        case TYPE_ENTIER: return "entier";
        case TYPE_REEL: return "reel";
        case TYPE_CARACTERE: return "caractere";
        case TYPE_CHAINE: return "chaine";
        case TYPE_BOOLEEN: return "booleen";
        case TYPE_TABLEAU: return "tableau";
        case TYPE_STRUCT: return "structure";
        default: return "inconnu";
    }
}

// Fonction pour obtenir le type d'un token
TYPE_DONNEES Type_De_Token(CODE_LEX token) {
    switch (token) {
        case NUM_ENTIER_TOKEN: return TYPE_ENTIER;
        case NUM_REEL_TOKEN: return TYPE_REEL;
        case CHAINE_LITTERALE_TOKEN: return TYPE_CHAINE;
        case BOOLEEN_LITTERALE_TOKEN: return TYPE_BOOLEEN;
        default: return TYPE_UNDEFINED;
    }
}

// =====================================================
// FONCTIONS POUR LA TABLE DES SYMBOLES
// =====================================================

// Rechercher un symbole dans la table
int Rechercher_Sym(char *nom) {
    for (int i = 0; i < NBR_SYM; i++) {
        if (strcasecmp(TAB_SYM[i].NOM, nom) == 0) {
            return i;
        }
    }
    return -1;
}

// Fonction pour afficher une erreur sémantique
void Erreur_Semantique(Erreur_Semantique_Code code_err, char *details) {
    int trouve = 0;
    for (int i = 0; i < sizeof(SEM_MES_ERR) / sizeof(SEM_MES_ERR[0]); i++) {
        if (SEM_MES_ERR[i].CODE_ERR == code_err) {
            printf("Erreur semantique : %s", SEM_MES_ERR[i].mes);
            fflush(stdout);
            if (details != NULL && strlen(details) > 0) {
                printf(" (%s)", details);
                fflush(stdout);
            }
            printf("\n");
            fflush(stdout);
            trouve = 1;
            break;
        }
    }

    if (!trouve) {
        printf("Erreur semantique inconnue");
        fflush(stdout);
        if (details != NULL && strlen(details) > 0) {
            printf(" (%s)", details);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }

    NB_ERREURS_SEMANTIQUES++;
}

void Erreur_Semantique_Format(Erreur_Semantique_Code code_err, const char *format, ...) {
    char details[200] = "";

    if (format != NULL) {
        va_list args;
        va_start(args, format);
        vsprintf(details, format, args);
        va_end(args);
    }

    Erreur_Semantique(code_err, details);
}

int Ajouter_Sym(char *nom, TSYM tidf, TYPE_DONNEES type, int val, int adresse) {
    if (NBR_SYM >= TAILLE_TABLE_SYM) {
        Erreur_Semantique_Format(SEM_ERR_INDEX_TAB, "table des symboles pleine");
        return -1;
    }

    if (tidf != TTYPE) {
        if (Rechercher_Sym(nom) != -1) {
            Erreur_Semantique_Format(SEM_ERR_DOUBLE_DECL, "%s", nom);
            return -1;
        }
    }

    if (strlen(nom) >= MAX_ID) {
        Erreur_Semantique_Format(SEM_ERR_ID_TROP_LONG, "%s (max %d caractères)", nom, MAX_ID);
    }

    strcpy(TAB_SYM[NBR_SYM].NOM, nom);
    TAB_SYM[NBR_SYM].TIDF = tidf;
    TAB_SYM[NBR_SYM].TYPE = type;
    TAB_SYM[NBR_SYM].TYPE_ELEM = TYPE_UNDEFINED;
    TAB_SYM[NBR_SYM].REF_TYPE = -1;
    TAB_SYM[NBR_SYM].VAL = val;
    TAB_SYM[NBR_SYM].ADRESSE = adresse;
    TAB_SYM[NBR_SYM].TAILLE = 0;
    TAB_SYM[NBR_SYM].NB_CHAMPS = 0;

    if (tidf == TVAR) {
        if (type == TYPE_STRUCT || type == TYPE_TABLEAU) {
            TAB_SYM[NBR_SYM].ADRESSE = OFFSET;
        } else {
            OFFSET++;
            TAB_SYM[NBR_SYM].ADRESSE = OFFSET;
        }
    }

    return NBR_SYM++;
}

int Verifier_ID_Declare(char *nom, int avant_begin) {
    int index = Rechercher_Sym(nom);
    if (index == -1) {
        if (avant_begin) {
            return -1;
        } else {
            Erreur_Semantique_Format(SEM_ERR_ID_NON_DECL, "%s", nom);
            return -1;
        }
    }
    INDEX_DER_SYM_ACC = index;
    return index;
}

void Verifier_Non_Constante(char *nom) {
    int index = Rechercher_Sym(nom);
    if (index != -1 && TAB_SYM[index].TIDF == TCONST) {
        Erreur_Semantique_Format(SEM_ERR_CONST_MODIF, "%s", nom);
    }
}

void Verifier_Non_Prog_ID(char *nom, char *nom_prog) {
    if (strcasecmp(nom, nom_prog) == 0) {
        Erreur_Semantique_Format(SEM_ERR_PROG_ID_USE, "%s", nom);
    }
}

// =====================================================
// ANALYSEUR LEXICAL
// =====================================================

void Erreur_Lexicale(Erreurs_Lexicaux ERR) {
    int ind_err = ERR;
    if (ind_err >= 0 && ind_err < 5) {
        printf("Erreur lexicale : %s\n", MES_ERR[ind_err].mes);
    } else {
        printf("Erreur lexicale inconnue\n");
    }
    exit(1);
}

void Lire_Car() {
    Car_Cour = fgetc(Fichier);
}

void Passer_Separateurs() {
    while (1) {
        if (Car_Cour == ' ' || Car_Cour == '\t' || Car_Cour == '\n' || Car_Cour == '\r') {
            Lire_Car();
        } else if (Car_Cour == '#') {
            while (Car_Cour != '\n' && Car_Cour != EOF) {
                Lire_Car();
            }
            if (Car_Cour == '\n') Lire_Car();
        } else {
            break;
        }
    }
}

CODE_LEX Rechercher_Mot_Cle(char *mot) {
    int i = 0;
    while (tableMotsCles[i].code != FIN_FICHIER_TOKEN) {
        if (strcasecmp(mot, tableMotsCles[i].nom) == 0) {
            return tableMotsCles[i].code;
        }
        i++;
    }
    return ID_TOKEN;
}

void Lire_Mot() {
    char mot[MAX_ID + 1];
    int i = 0;

    if (!isalpha(Car_Cour) && Car_Cour != '_') {
        Sym_Cour.CODE = ERREUR_TOKEN;
        return;
    }

    while ((isalnum(Car_Cour) || Car_Cour == '_') && i < MAX_ID) {
        mot[i++] = tolower(Car_Cour);
        Lire_Car();
    }

    if (i >= MAX_ID) {
        Erreur_Lexicale(ERR_ID_LONG);
    }

    mot[i] = '\0';
    strcpy(Sym_Cour.NOM, mot);
    Sym_Cour.CODE = Rechercher_Mot_Cle(mot);
}

void Lire_Nombre() {
    char num[50];
    int i = 0;
    int point = 0;

    if (!isdigit(Car_Cour)) {
        Sym_Cour.CODE = ERREUR_TOKEN;
        return;
    }

    while ((isdigit(Car_Cour) || Car_Cour == '.') && i < 49) {
        if (Car_Cour == '.') {
            if (point) break;
            point = 1;
        }
        num[i++] = Car_Cour;
        Lire_Car();
    }

    if (i >= MAX_ENTIER && !point) {
        Erreur_Lexicale(ERR_NUM_LONG);
    }

    num[i] = '\0';
    strcpy(Sym_Cour.NOM, num);

    if (point) {
        Sym_Cour.CODE = NUM_REEL_TOKEN;
        Sym_Cour.val = atof(num);
    } else {
        Sym_Cour.CODE = NUM_ENTIER_TOKEN;
        Sym_Cour.val = atoi(num);
    }
}

void Lire_Chaine() {
    char chaine[256];
    int i = 0;

    Lire_Car();

    while (Car_Cour != '"' && Car_Cour != EOF && i < 255) {
        chaine[i++] = Car_Cour;
        Lire_Car();
    }

    if (Car_Cour != '"') {
        Erreur_Lexicale(ERR_CHAINE_INC);
    }

    chaine[i] = '\0';
    strcpy(Sym_Cour.NOM, chaine);
    Sym_Cour.CODE = CHAINE_LITTERALE_TOKEN;
    Lire_Car();
}

void Lire_Special() {
    Sym_Cour.NOM[0] = Car_Cour;
    Sym_Cour.NOM[1] = '\0';

    switch (Car_Cour) {
        case '+': Sym_Cour.CODE = PLUS_TOKEN; Lire_Car(); return;
        case '-': Sym_Cour.CODE = MOINS_TOKEN; Lire_Car(); return;
        case '*': Sym_Cour.CODE = MULT_TOKEN; Lire_Car(); return;
        case '/': Sym_Cour.CODE = DIV_TOKEN; Lire_Car(); return;
        case '%': Sym_Cour.CODE = MOD_TOKEN; Lire_Car(); return;
        case '=': Sym_Cour.CODE = EGAL_TOKEN; Lire_Car(); return;
        case '<':
            Lire_Car();
            if (Car_Cour == '=') {
                strcpy(Sym_Cour.NOM, "<=");
                Sym_Cour.CODE = INFERIEUR_EGAL_TOKEN;
                Lire_Car();
            } else if (Car_Cour == '>') {
                strcpy(Sym_Cour.NOM, "<>");
                Sym_Cour.CODE = DIFFERENT_TOKEN;
                Lire_Car();
            } else {
                strcpy(Sym_Cour.NOM, "<");
                Sym_Cour.CODE = INFERIEUR_TOKEN;
            }
            return;
        case '>':
            Lire_Car();
            if (Car_Cour == '=') {
                strcpy(Sym_Cour.NOM, ">=");
                Sym_Cour.CODE = SUPERIEUR_EGAL_TOKEN;
                Lire_Car();
            } else {
                strcpy(Sym_Cour.NOM, ">");
                Sym_Cour.CODE = SUPERIEUR_TOKEN;
            }
            return;
        case ':':
            Lire_Car();
            if (Car_Cour == '=') {
                strcpy(Sym_Cour.NOM, ":=");
                Sym_Cour.CODE = AFFECTATION_TOKEN;
                Lire_Car();
            } else {
                strcpy(Sym_Cour.NOM, ":");
                Sym_Cour.CODE = DEUX_POINTS_TOKEN;
            }
            return;
        case '(': Sym_Cour.CODE = PARENTHESE_OUVRANTE_TOKEN; Lire_Car(); return;
        case ')': Sym_Cour.CODE = PARENTHESE_FERMANTE_TOKEN; Lire_Car(); return;
        case '[': Sym_Cour.CODE = CROCHET_OUVRANT_TOKEN; Lire_Car(); return;
        case ']': Sym_Cour.CODE = CROCHET_FERMANT_TOKEN; Lire_Car(); return;
        case ',': Sym_Cour.CODE = VIRGULE_TOKEN; Lire_Car(); return;
        case ';': Sym_Cour.CODE = POINT_VIRGULE_TOKEN; Lire_Car(); return;
        case '.': Sym_Cour.CODE = POINT_TOKEN; Lire_Car(); return;
        default:
            Sym_Cour.CODE = ERREUR_TOKEN;
            Erreur_Lexicale(ERR_CAR_INC);
            return;
    }
}

void Sym_Suiv() {
    Passer_Separateurs();

    if (Car_Cour == EOF) {
        Sym_Cour.CODE = FIN_FICHIER_TOKEN;
        strcpy(Sym_Cour.NOM, "EOF");
        return;
    }

    if (isalpha(Car_Cour) || Car_Cour == '_') {
        Lire_Mot();
    } else if (isdigit(Car_Cour)) {
        Lire_Nombre();
    } else if (Car_Cour == '"') {
        Lire_Chaine();
    } else {
        Lire_Special();
    }
}

// =====================================================
// FONCTIONS D'ERREUR
// =====================================================

void Erreur_Syntaxique(Erreur_Syntaxique_Code err) {
    int ind_err = err;
    if (ind_err >= 0 && ind_err < (sizeof(SYN_MES_ERR) / sizeof(SYN_MES_ERR[0]))) {
        printf("Erreur syntaxique : %s\n", SYN_MES_ERR[ind_err].mes);
        printf("Token courant : %s\n", Sym_Cour.NOM);
    } else {
        printf("Erreur syntaxique inconnue\n");
    }
    exit(1);
}

void Test_Symbole(CODE_LEX attendu, Erreur_Syntaxique_Code err) {
    if (Sym_Cour.CODE == attendu) {
        Sym_Suiv();
    } else {
        Erreur_Syntaxique(err);
    }
}

// =====================================================
// FONCTION POUR ANALYSER LES TYPES
// =====================================================

TYPE_DONNEES Analyse_TYPE(int *taille_retournee, TYPE_DONNEES *type_elem_retourne);

// =====================================================
// PROTOTYPES DES FONCTIONS SYNTAXIQUES
// =====================================================

void PROGRAM();
void BLOCK();
void VARIABLES();
void LISTE_VARS();
void CONSTANTES();
void TYPE();
void TYPE_BASE();
void CHAMPS(int struct_index);
void INSTS_PRINCIPAL();
void INSTS_CORPS();
void INST();
void VARIABLE();
void AFFEC();
void SI();
void SELON();
void LISTE_CAS();
void CAS_AUTRES();
void INSTS_CAS();
void POUR();
void TANT_QUE();
void REPETER();
void ECRIRE();
void LIRE();
void COND();
void COND_TERM();
void COND_FACT();
void RELOP();
void EXPR();
void TERM();
void FACT();
void NUM_FACT();
void SIGNE();
void VALEUR();

// Prototypes pour les fonctions avec vérification de type
TYPE_DONNEES EXPR_TYPE();
TYPE_DONNEES TERM_TYPE();
TYPE_DONNEES FACT_TYPE();
TYPE_DONNEES NUM_FACT_TYPE();

// Prototypes pour les fonctions de génération de code
void GENERER_EXPR();
void GENERER_TERM();
void GENERER_FACT();
void GENERER_COND();
void GENERER_COND_TERM();
void GENERER_COND_FACT();
void GENERER_RELOP(CODE_LEX op);
void GENERER_VARIABLE(char *nom);
void GENERER_EXPR_ECRIRE();

// =====================================================
// FONCTIONS DE GENERATION DE CODE
// =====================================================

void GENERER_EXPR() {
    GENERER_TERM();
    while (Sym_Cour.CODE == PLUS_TOKEN || Sym_Cour.CODE == MOINS_TOKEN || Sym_Cour.CODE == OU_TOKEN) {
        CODE_LEX op = Sym_Cour.CODE;
        Sym_Suiv();

        if (MODE_GENERATION) {
            if (op == OU_TOKEN) {
                Emettre(" || ");
            } else if (op == PLUS_TOKEN) {
                Emettre(" + ");
            } else if (op == MOINS_TOKEN) {
                Emettre(" - ");
            }
        }

        GENERER_TERM();
    }
}

void GENERER_TERM() {
    GENERER_FACT();
    while (Sym_Cour.CODE == MULT_TOKEN || Sym_Cour.CODE == DIV_TOKEN ||
           Sym_Cour.CODE == ET_TOKEN || Sym_Cour.CODE == MOD_TOKEN) {
        CODE_LEX op = Sym_Cour.CODE;
        Sym_Suiv();

        if (MODE_GENERATION) {
            if (op == ET_TOKEN) {
                Emettre(" && ");
            } else if (op == MULT_TOKEN) {
                Emettre(" * ");
            } else if (op == DIV_TOKEN) {
                Emettre(" / ");
            } else if (op == MOD_TOKEN) {
                Emettre(" %% ");
            }
        }

        GENERER_FACT();
    }
}

void GENERER_FACT() {
    if (Sym_Cour.CODE == NON_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) Emettre("!");
        GENERER_FACT();
    } else if (Sym_Cour.CODE == PARENTHESE_OUVRANTE_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) Emettre("(");
        GENERER_EXPR();
        Test_Symbole(PARENTHESE_FERMANTE_TOKEN, SYN_ERR_PAR_FERM);
        if (MODE_GENERATION) Emettre(")");
    } else if (Sym_Cour.CODE == CHAINE_LITTERALE_TOKEN) {
        if (MODE_GENERATION) Emettre("\"%s\"", Sym_Cour.NOM);
        Sym_Suiv();
    } else if (Sym_Cour.CODE == BOOLEEN_LITTERALE_TOKEN) {
        if (MODE_GENERATION) {
            if (strcasecmp(Sym_Cour.NOM, "vrai") == 0) Emettre("true");
            else Emettre("false");
        }
        Sym_Suiv();
    } else {
        // Numérique ou identificateur
        if (Sym_Cour.CODE == ID_TOKEN) {
            GENERER_VARIABLE(Sym_Cour.NOM);
        } else if (Sym_Cour.CODE == NUM_ENTIER_TOKEN || Sym_Cour.CODE == NUM_REEL_TOKEN) {
            if (MODE_GENERATION) Emettre("%s", Sym_Cour.NOM);
            Sym_Suiv();
        } else if (Sym_Cour.CODE == PLUS_TOKEN || Sym_Cour.CODE == MOINS_TOKEN) {
            SIGNE();
            if (MODE_GENERATION) {
                if (Sym_Cour.CODE == MOINS_TOKEN) Emettre("-");
                else Emettre("+");
            }
            GENERER_FACT();
        } else {
            Erreur_Syntaxique(SYN_ERR_FACT);
        }
    }
}
void GENERER_EXPR_ECRIRE() {
    if (Sym_Cour.CODE == CHAINE_LITTERALE_TOKEN) {
        if (MODE_GENERATION) {
            // Pour les chaînes littérales, on ajoute directement le texte au format
            // On doit échapper les caractères spéciaux pour le format printf
            char escaped_str[512];
            char *src = Sym_Cour.NOM;
            char *dst = escaped_str;

            while (*src) {
                if (*src == '%') {
                    *dst++ = '%';
                    *dst++ = '%';
                } else {
                    *dst++ = *src;
                }
                src++;
            }
            *dst = '\0';

            Ajouter_Format_Printf(escaped_str);
            // Pas d'argument pour le texte littéral
        }
        Sym_Suiv();
    }
    else if (Sym_Cour.CODE == BOOLEEN_LITTERALE_TOKEN) {
        if (MODE_GENERATION) {
            Ajouter_Format_Printf((strcasecmp(Sym_Cour.NOM, "vrai") == 0) ? "1" : "0");
        }
        Sym_Suiv();
    }
    else if (Sym_Cour.CODE == ID_TOKEN) {
        // Construire une expression d'argument (ex: etudiant.nom)
        char expr_arg[128];
        strcpy(expr_arg, Sym_Cour.NOM);

        int index_var = Rechercher_Sym(Sym_Cour.NOM);
        TYPE_DONNEES type_arg = (index_var != -1) ? TAB_SYM[index_var].TYPE : TYPE_UNDEFINED;

        Sym_Suiv(); // consommer l'identificateur

        // Champ de structure : etudiant.nom
        if (Sym_Cour.CODE == POINT_TOKEN) {
            Sym_Suiv();
            if (Sym_Cour.CODE != ID_TOKEN) {
                Erreur_Syntaxique(SYN_ERR_ID);
            }
            strcat(expr_arg, ".");
            strcat(expr_arg, Sym_Cour.NOM);

            // Déterminer le type du champ si possible (à partir du type de la structure)
            if (index_var != -1 && TAB_SYM[index_var].TYPE == TYPE_STRUCT && TAB_SYM[index_var].REF_TYPE != -1) {
                int t = TAB_SYM[index_var].REF_TYPE;
                for (int j = 0; j < TAB_SYM[t].NB_CHAMPS; j++) {
                    if (strcasecmp(TAB_SYM[t].CHAMPS[j].NOM, Sym_Cour.NOM) == 0) {
                        type_arg = TAB_SYM[t].CHAMPS[j].TYPE;
                        break;
                    }
                }
            }
            Sym_Suiv(); // consommer le nom du champ
        }

        // Indice de tableau : notes[i] (format/type approximatif ici)
        if (Sym_Cour.CODE == CROCHET_OUVRANT_TOKEN) {
            Sym_Suiv();
            // On génère le texte de l'indice comme expression C
            // (pour un vrai support complet, il faudrait construire une chaîne temporaire)
            // Ici, on consomme juste l'expression pour garder l'analyse synchronisée.
            GENERER_EXPR();
            Test_Symbole(CROCHET_FERMANT_TOKEN, SYN_ERR_CROCHET_FERM);
        }

        // Ajouter format + argument selon le type détecté
        if (MODE_GENERATION) {
            if (type_arg == TYPE_REEL) {
                Ajouter_Format_Printf("%lf");
                Ajouter_Argument_Printf(expr_arg);
            } else if (type_arg == TYPE_CHAINE) {
                Ajouter_Format_Printf("%s");
                Ajouter_Argument_Printf(expr_arg);
            } else if (type_arg == TYPE_CARACTERE) {
                Ajouter_Format_Printf("%c");
                Ajouter_Argument_Printf(expr_arg);
            } else if (type_arg == TYPE_BOOLEEN) {
                Ajouter_Format_Printf("%s");
                char bool_expr[170];
                snprintf(bool_expr, sizeof(bool_expr), "(%s ? \"vrai\" : \"faux\")", expr_arg);
                Ajouter_Argument_Printf(bool_expr);
            } else {
                // ENTIER / défaut
                Ajouter_Format_Printf("%d");
                Ajouter_Argument_Printf(expr_arg);
            }
        }
    } else if (Sym_Cour.CODE == NUM_ENTIER_TOKEN || Sym_Cour.CODE == NUM_REEL_TOKEN) {
        if (MODE_GENERATION) {
            if (Sym_Cour.CODE == NUM_REEL_TOKEN) Ajouter_Format_Printf("%lf");
            else Ajouter_Format_Printf("%d");
            Ajouter_Argument_Printf(Sym_Cour.NOM);
        }
        Sym_Suiv();
    } else {
        // Expression complexe
        if (MODE_GENERATION) {
            // Pour les expressions, on génère d'abord le code de l'expression
            // dans une variable temporaire
            static int temp_counter = 0;
            char temp_var[20];
            sprintf(temp_var, "__temp_%d", temp_counter++);

            // Émettre l'affectation de la variable temporaire
            EmettreIndente("int %s = ", temp_var);
            GENERER_EXPR();
            Emettre(";\n");

            // Ajouter au format et arguments
            Ajouter_Format_Printf("%d");
            Ajouter_Argument_Printf(temp_var);
        } else {
            // En mode analyse, juste consommer l'expression
            GENERER_EXPR();
        }
    }
}
void GENERER_COND() {
    GENERER_COND_TERM();
    while (Sym_Cour.CODE == OU_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) Emettre(" || ");
        GENERER_COND_TERM();
    }
}

void GENERER_COND_TERM() {
    GENERER_COND_FACT();
    while (Sym_Cour.CODE == ET_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) Emettre(" && ");
        GENERER_COND_FACT();
    }
}

void GENERER_COND_FACT() {
    if (Sym_Cour.CODE == NON_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) Emettre("!");
        GENERER_COND_FACT();
    } else if (Sym_Cour.CODE == PARENTHESE_OUVRANTE_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) Emettre("(");
        GENERER_COND();
        Test_Symbole(PARENTHESE_FERMANTE_TOKEN, SYN_ERR_PAR_FERM);
        if (MODE_GENERATION) Emettre(")");
    } else {
        // Expression relationnelle
        GENERER_EXPR();

        if (Sym_Cour.CODE == EGAL_TOKEN || Sym_Cour.CODE == DIFFERENT_TOKEN ||
            Sym_Cour.CODE == INFERIEUR_TOKEN || Sym_Cour.CODE == SUPERIEUR_TOKEN ||
            Sym_Cour.CODE == INFERIEUR_EGAL_TOKEN || Sym_Cour.CODE == SUPERIEUR_EGAL_TOKEN) {

            CODE_LEX op = Sym_Cour.CODE;
            RELOP();

            if (MODE_GENERATION) GENERER_RELOP(op);

            GENERER_EXPR();
        }
    }
}

void GENERER_RELOP(CODE_LEX op) {
    switch (op) {
        case EGAL_TOKEN: Emettre(" == "); break;
        case DIFFERENT_TOKEN: Emettre(" != "); break;
        case INFERIEUR_TOKEN: Emettre(" < "); break;
        case SUPERIEUR_TOKEN: Emettre(" > "); break;
        case INFERIEUR_EGAL_TOKEN: Emettre(" <= "); break;
        case SUPERIEUR_EGAL_TOKEN: Emettre(" >= "); break;
        default: Emettre(" == "); break;
    }
}

void GENERER_VARIABLE(char *nom) {
    // IMPORTANT: ne pas consommer des tokens en trop.
    // L'ancienne version utilisait Test_Symbole(ID_TOKEN) puis lisait Sym_Cour.NOM,
    // ce qui faisait avancer au token suivant (ex: ':=') et cassait l'analyse.
    if (MODE_GENERATION) Emettre("%s", nom);
    Sym_Suiv();  // Consommer l'identificateur (base)

    if (Sym_Cour.CODE == POINT_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) Emettre(".");
        if (Sym_Cour.CODE != ID_TOKEN) {
            Erreur_Syntaxique(SYN_ERR_ID);
        }
        if (MODE_GENERATION) Emettre("%s", Sym_Cour.NOM);
        Sym_Suiv(); // Consommer le nom du champ
    }

    if (Sym_Cour.CODE == CROCHET_OUVRANT_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) Emettre("[");
        GENERER_EXPR();
        Test_Symbole(CROCHET_FERMANT_TOKEN, SYN_ERR_CROCHET_FERM);
        if (MODE_GENERATION) Emettre("]");
    }
}

// Même logique que GENERER_VARIABLE, mais écrit dans un buffer (sans produire de sortie)
void GENERER_VARIABLE_BUFFER(const char *nom, char *out, size_t cap) {
    if (cap == 0) return;
    out[0] = '\0';
    strncat(out, nom, cap - 1);
    Sym_Suiv();

    if (Sym_Cour.CODE == POINT_TOKEN) {
        Sym_Suiv();
        if (Sym_Cour.CODE == ID_TOKEN) {
            strncat(out, ".", cap - strlen(out) - 1);
            strncat(out, Sym_Cour.NOM, cap - strlen(out) - 1);
            Sym_Suiv();
        } else {
            Erreur_Syntaxique(SYN_ERR_ID);
        }
    }

    if (Sym_Cour.CODE == CROCHET_OUVRANT_TOKEN) {
        Sym_Suiv();
        char idx[32] = {0};
        if (Sym_Cour.CODE == NUM_ENTIER_TOKEN) {
            snprintf(idx, sizeof(idx), "%d", (int)Sym_Cour.val);
            Sym_Suiv();
        } else if (Sym_Cour.CODE == ID_TOKEN) {
            strncpy(idx, Sym_Cour.NOM, sizeof(idx) - 1);
            Sym_Suiv();
        } else {
            Erreur_Syntaxique(SYN_ERR_ID);
        }
        Test_Symbole(CROCHET_FERMANT_TOKEN, SYN_ERR_CROCHET_FERM);

        strncat(out, "[", cap - strlen(out) - 1);
        strncat(out, idx, cap - strlen(out) - 1);
        strncat(out, "]", cap - strlen(out) - 1);
    }
}

// =====================================================
// FONCTIONS SYNTAXIQUES AVEC SEMANTIQUE ET GENERATION
// =====================================================

void PROGRAM() {
    Test_Symbole(ALGORITHME_TOKEN, SYN_ERR_ALGORITHME);
    strcpy(NOM_PROGRAMME, Sym_Cour.NOM);
    Ajouter_Sym(Sym_Cour.NOM, TPROG, TYPE_UNDEFINED, 0, 0);

    Test_Symbole(ID_TOKEN, SYN_ERR_ID);
    Test_Symbole(POINT_VIRGULE_TOKEN, SYN_ERR_POINT_VIRGULE);

    AVANT_BEGIN = 1;
    BLOCK();
    AVANT_BEGIN = 0;
}

void BLOCK() {
    VARIABLES();
    CONSTANTES();
    INSTS_PRINCIPAL();
}

void VARIABLES() {
    switch (Sym_Cour.CODE) {
        case VARIABLES_TOKEN:
            Sym_Suiv();
            LISTE_VARS();
            break;
        case CONSTANTE_TOKEN:
        case DEBUT_TOKEN:
            break;
        default:
            Erreur_Syntaxique(SYN_ERR_VARIABLES);
    }
}

void LISTE_VARS() {
    char noms[10][20];
    int nb_ids = 0;

    strcpy(noms[nb_ids++], Sym_Cour.NOM);
    Test_Symbole(ID_TOKEN, SYN_ERR_ID);

    while (Sym_Cour.CODE == VIRGULE_TOKEN) {
        Sym_Suiv();
        strcpy(noms[nb_ids++], Sym_Cour.NOM);
        Test_Symbole(ID_TOKEN, SYN_ERR_ID);
    }

    Test_Symbole(DEUX_POINTS_TOKEN, SYN_ERR_DEUX_POINTS);

    int taille_tableau = 0;
    TYPE_DONNEES type_elem = TYPE_UNDEFINED;
    TYPE_DONNEES type_var = Analyse_TYPE(&taille_tableau, &type_elem);

    Test_Symbole(POINT_VIRGULE_TOKEN, SYN_ERR_POINT_VIRGULE);

    for (int i = 0; i < nb_ids; i++) {
        Verifier_Non_Prog_ID(noms[i], NOM_PROGRAMME);
        int index = Ajouter_Sym(noms[i], TVAR, type_var, 0, OFFSET);

        if (index != -1 && type_var == TYPE_TABLEAU) {
            TAB_SYM[index].TAILLE = taille_tableau;
            TAB_SYM[index].TYPE_ELEM = type_elem;
            TAB_SYM[index].TAILLE = taille_tableau;
            OFFSET += taille_tableau;
        } else if (index != -1 && type_var == TYPE_STRUCT) {
            TAB_SYM[index].REF_TYPE = DERNIER_TYPE_STRUCT_INDEX;
        }
    }

    while (Sym_Cour.CODE == ID_TOKEN) {
        nb_ids = 0;
        strcpy(noms[nb_ids++], Sym_Cour.NOM);
        Test_Symbole(ID_TOKEN, SYN_ERR_ID);

        while (Sym_Cour.CODE == VIRGULE_TOKEN) {
            Sym_Suiv();
            strcpy(noms[nb_ids++], Sym_Cour.NOM);
            Test_Symbole(ID_TOKEN, SYN_ERR_ID);
        }

        Test_Symbole(DEUX_POINTS_TOKEN, SYN_ERR_DEUX_POINTS);
        taille_tableau = 0;
        type_elem = TYPE_UNDEFINED;
        type_var = Analyse_TYPE(&taille_tableau, &type_elem);
        Test_Symbole(POINT_VIRGULE_TOKEN, SYN_ERR_POINT_VIRGULE);

        for (int i = 0; i < nb_ids; i++) {
            Verifier_Non_Prog_ID(noms[i], NOM_PROGRAMME);
            int index = Ajouter_Sym(noms[i], TVAR, type_var, 0, OFFSET);

            if (index != -1 && type_var == TYPE_TABLEAU) {
                TAB_SYM[index].TAILLE = taille_tableau;
            TAB_SYM[index].TYPE_ELEM = type_elem;
            TAB_SYM[index].TAILLE = taille_tableau;
                OFFSET += taille_tableau;
            } else if (index != -1 && type_var == TYPE_STRUCT) {
                TAB_SYM[index].REF_TYPE = DERNIER_TYPE_STRUCT_INDEX;
            }
        }
    }
}

void CONSTANTES() {
    if (Sym_Cour.CODE == CONSTANTE_TOKEN) {
        Sym_Suiv();

        char nom_const[20];
        strcpy(nom_const, Sym_Cour.NOM);
        Test_Symbole(ID_TOKEN, SYN_ERR_ID);

        Test_Symbole(EGAL_TOKEN, SYN_ERR_EGAL);

        int valeur = 0;
        if (Sym_Cour.CODE == NUM_ENTIER_TOKEN) {
            valeur = atoi(Sym_Cour.NOM);
        } else if (Sym_Cour.CODE == NUM_REEL_TOKEN) {
            valeur = (int)atof(Sym_Cour.NOM);
        } else {
            Erreur_Syntaxique(SYN_ERR_NUM);
        }

        Sym_Suiv();
        Test_Symbole(POINT_VIRGULE_TOKEN, SYN_ERR_POINT_VIRGULE);

        Verifier_Non_Prog_ID(nom_const, NOM_PROGRAMME);
        Ajouter_Sym(nom_const, TCONST, TYPE_ENTIER, valeur, 0);

        while (Sym_Cour.CODE == ID_TOKEN) {
            strcpy(nom_const, Sym_Cour.NOM);
            Sym_Suiv();

            Test_Symbole(EGAL_TOKEN, SYN_ERR_EGAL);

            if (Sym_Cour.CODE == NUM_ENTIER_TOKEN) {
                valeur = atoi(Sym_Cour.NOM);
            } else if (Sym_Cour.CODE == NUM_REEL_TOKEN) {
                valeur = (int)atof(Sym_Cour.NOM);
            } else {
                Erreur_Syntaxique(SYN_ERR_NUM);
            }

            Sym_Suiv();
            Test_Symbole(POINT_VIRGULE_TOKEN, SYN_ERR_POINT_VIRGULE);

            Verifier_Non_Prog_ID(nom_const, NOM_PROGRAMME);
            Ajouter_Sym(nom_const, TCONST, TYPE_ENTIER, valeur, 0);
        }
    }
}

void TYPE() {
    switch (Sym_Cour.CODE) {
        case TABLEAU_TOKEN:
            Sym_Suiv();
            Test_Symbole(CROCHET_OUVRANT_TOKEN, SYN_ERR_CROCHET_OUV);
            Test_Symbole(NUM_ENTIER_TOKEN, SYN_ERR_NUM);
            Test_Symbole(CROCHET_FERMANT_TOKEN, SYN_ERR_CROCHET_FERM);
            Test_Symbole(DE_TYPE_TOKEN, SYN_ERR_DE_TYPE);
            TYPE_BASE();
            break;
        case STRUCTURE_TOKEN:
            Sym_Suiv();
            char nom_struct[30];
            sprintf(nom_struct, "structure_%d", NBR_SYM);
            int struct_index = Ajouter_Sym(nom_struct, TTYPE, TYPE_STRUCT, 0, 0);
            CHAMPS(struct_index);
            Test_Symbole(FIN_STRUCTURE_TOKEN, SYN_ERR_FIN_STRUCTURE);
            break;
        default:
            TYPE_BASE();
    }
}

TYPE_DONNEES Analyse_TYPE(int *taille_retournee, TYPE_DONNEES *type_elem_retourne) {
    TYPE_DONNEES type = TYPE_UNDEFINED;
    int taille = 0;

    if (taille_retournee) *taille_retournee = 0;
    if (type_elem_retourne) *type_elem_retourne = TYPE_UNDEFINED;

    switch (Sym_Cour.CODE) {
        case ENTIER_TOKEN:
            type = TYPE_ENTIER;
            Sym_Suiv();
            break;
        case REEL_TOKEN:
            type = TYPE_REEL;
            Sym_Suiv();
            break;
        case CARACTERE_TOKEN:
            type = TYPE_CARACTERE;
            Sym_Suiv();
            break;
        case CHAINE_TOKEN:
            type = TYPE_CHAINE;
            Sym_Suiv();
            break;
        case BOOLEEN_TOKEN:
            type = TYPE_BOOLEEN;
            Sym_Suiv();
            break;
        case TABLEAU_TOKEN:
            type = TYPE_TABLEAU;
            Sym_Suiv();
            Test_Symbole(CROCHET_OUVRANT_TOKEN, SYN_ERR_CROCHET_OUV);

            if (Sym_Cour.CODE != NUM_ENTIER_TOKEN) {
                Erreur_Syntaxique(SYN_ERR_NUM);
            }

            taille = atoi(Sym_Cour.NOM);
            if (taille <= 0) {
                Erreur_Semantique_Format(SEM_ERR_INDEX_TAB, "taille de tableau invalide %d", taille);
            }

            if (taille_retournee) *taille_retournee = taille;

            Sym_Suiv();
            Test_Symbole(CROCHET_FERMANT_TOKEN, SYN_ERR_CROCHET_FERM);
            Test_Symbole(DE_TYPE_TOKEN, SYN_ERR_DE_TYPE);

            TYPE_DONNEES type_base = Analyse_TYPE(NULL, NULL);
            if (type_elem_retourne) *type_elem_retourne = type_base;
            char nom_type[30];
            sprintf(nom_type, "tableau_%d", NBR_SYM);
            int index = Ajouter_Sym(nom_type, TTYPE, TYPE_TABLEAU, 0, 0);
            if (index != -1) {
                TAB_SYM[index].TAILLE = taille;
                TAB_SYM[index].TYPE_ELEM = type_base;
            }
            break;
        case STRUCTURE_TOKEN:
            type = TYPE_STRUCT;
            Sym_Suiv();
            char nom_struct[30];
            sprintf(nom_struct, "structure_%d", NBR_SYM);
            int struct_index = Ajouter_Sym(nom_struct, TTYPE, TYPE_STRUCT, 0, 0);
            // Mémoriser le type de structure courant pour rattacher la(les) variable(s) déclarée(s)
            DERNIER_TYPE_STRUCT_INDEX = struct_index;
            CHAMPS(struct_index);
            Test_Symbole(FIN_STRUCTURE_TOKEN, SYN_ERR_FIN_STRUCTURE);
            break;
        case ID_TOKEN:
            type = TYPE_UNDEFINED;
            Sym_Suiv();
            break;
        default:
            Erreur_Syntaxique(SYN_ERR_TYPE);
    }

    return type;
}

void TYPE_BASE() {
    switch (Sym_Cour.CODE) {
        case ENTIER_TOKEN:
        case REEL_TOKEN:
        case CARACTERE_TOKEN:
        case CHAINE_TOKEN:
        case BOOLEEN_TOKEN:
        case ID_TOKEN:
            Sym_Suiv();
            break;
        default:
            Erreur_Syntaxique(SYN_ERR_TYPE);
    }
}

void CHAMPS(int struct_index) {
    int champ_index = 0;

    do {
        if (Sym_Cour.CODE == POINT_VIRGULE_TOKEN) {
            Sym_Suiv();
            if (Sym_Cour.CODE == FIN_STRUCTURE_TOKEN) {
                break;
            }
        }

        champ_index = 0;
        char noms_champs[10][20];
        strcpy(noms_champs[champ_index++], Sym_Cour.NOM);

        Ajouter_Sym(Sym_Cour.NOM, TSTRUCT, TYPE_UNDEFINED, 0, 0);
        strcpy(TAB_SYM[struct_index].CHAMPS[TAB_SYM[struct_index].NB_CHAMPS + champ_index - 1].NOM,
               Sym_Cour.NOM);

        Test_Symbole(ID_TOKEN, SYN_ERR_ID);

        while (Sym_Cour.CODE == VIRGULE_TOKEN) {
            Sym_Suiv();
            strcpy(noms_champs[champ_index], Sym_Cour.NOM);
            Ajouter_Sym(Sym_Cour.NOM, TSTRUCT, TYPE_UNDEFINED, 0, 0);
            strcpy(TAB_SYM[struct_index].CHAMPS[TAB_SYM[struct_index].NB_CHAMPS + champ_index].NOM,
                   Sym_Cour.NOM);
            Test_Symbole(ID_TOKEN, SYN_ERR_ID);
            champ_index++;
        }

        Test_Symbole(DEUX_POINTS_TOKEN, SYN_ERR_DEUX_POINTS);

        TYPE_DONNEES type_champ = TYPE_UNDEFINED;
        switch (Sym_Cour.CODE) {
            case ENTIER_TOKEN: type_champ = TYPE_ENTIER; break;
            case REEL_TOKEN: type_champ = TYPE_REEL; break;
            case CARACTERE_TOKEN: type_champ = TYPE_CARACTERE; break;
            case CHAINE_TOKEN: type_champ = TYPE_CHAINE; break;
            case BOOLEEN_TOKEN: type_champ = TYPE_BOOLEEN; break;
            default: Erreur_Syntaxique(SYN_ERR_TYPE);
        }
        Sym_Suiv();

        for (int i = 0; i < champ_index; i++) {
            int index_champ = TAB_SYM[struct_index].NB_CHAMPS + i;
            TAB_SYM[struct_index].CHAMPS[index_champ].TYPE = type_champ;

            for (int j = NBR_SYM - 1; j >= 0; j--) {
                if (strcasecmp(TAB_SYM[j].NOM, noms_champs[i]) == 0
                    && TAB_SYM[j].TIDF == TSTRUCT) {
                    TAB_SYM[j].TYPE = type_champ;
                    break;
                }
            }
        }

        TAB_SYM[struct_index].NB_CHAMPS += champ_index;

    } while (Sym_Cour.CODE == POINT_VIRGULE_TOKEN);
}

void INSTS_PRINCIPAL() {
    Test_Symbole(DEBUT_TOKEN, SYN_ERR_DEBUT);
    AVANT_BEGIN = 0;

    if (MODE_GENERATION) {
        EmettreLigne("int main() {");
        NIVEAU_INDENTATION++;
    }

    INST();
    while (Sym_Cour.CODE == POINT_VIRGULE_TOKEN) {
        Sym_Suiv();
        INST();
    }
    Test_Symbole(FIN_TOKEN, SYN_ERR_FIN);

    if (MODE_GENERATION) {
        EmettreLigne("return 0;");
        NIVEAU_INDENTATION--;
        EmettreLigne("}");
    }
}

void INSTS_CORPS() {
    switch (Sym_Cour.CODE) {
        case FIN_TOKEN:
        case FIN_SI_TOKEN:
        case FIN_SELON_TOKEN:
        case FIN_POUR_TOKEN:
        case FIN_TANT_QUE_TOKEN:
        case JUSQUA_QUE_TOKEN:
        case SINON_TOKEN:
            return;
        default:
            break;
    }

    INST();
    while (Sym_Cour.CODE == POINT_VIRGULE_TOKEN) {
        Sym_Suiv();
        switch (Sym_Cour.CODE) {
            case FIN_TOKEN:
            case FIN_SI_TOKEN:
            case FIN_SELON_TOKEN:
            case FIN_POUR_TOKEN:
            case FIN_TANT_QUE_TOKEN:
            case JUSQUA_QUE_TOKEN:
            case SINON_TOKEN:
                return;
            default:
                break;
        }
        INST();
    }
}

void INST() {
    switch (Sym_Cour.CODE) {
        case ID_TOKEN:
            AFFEC();
            break;
        case SI_TOKEN:
            SI();
            break;
        case SELON_TOKEN:
            SELON();
            break;
        case POUR_TOKEN:
            POUR();
            break;
        case TANT_QUE_TOKEN:
            TANT_QUE();
            break;
        case REPETER_TOKEN:
            REPETER();
            break;
        case ECRIRE_TOKEN:
            ECRIRE();
            break;
        case LIRE_TOKEN:
            LIRE();
            break;
        case POINT_VIRGULE_TOKEN:
        case FIN_TOKEN:
        case SINON_TOKEN:
        case FIN_SI_TOKEN:
        case FIN_SELON_TOKEN:
        case FIN_POUR_TOKEN:
        case FIN_TANT_QUE_TOKEN:
        case JUSQUA_QUE_TOKEN:
        case VIRGULE_TOKEN:
            break;
        default:
            Erreur_Syntaxique(SYN_ERR_SI);
    }
}

// =====================================================
// FONCTIONS AVEC VERIFICATION DES TYPES ET GENERATION
// =====================================================

void VARIABLE() {
    char nom_base[20];
    int index = -1;
    int est_tableau = 0;
    int taille_tableau = 0;

    strcpy(nom_base, Sym_Cour.NOM);
    if (MODE_GENERATION) {
        if (CAPTURE_LVALUE) {
            snprintf(LVALUE_BUF, sizeof(LVALUE_BUF), "%s", nom_base);
        } else {
            Emettre("%s", nom_base);
        }
    }
    index = Verifier_ID_Declare(nom_base, AVANT_BEGIN);
    Test_Symbole(ID_TOKEN, SYN_ERR_ID);

    if (index != -1) {
        TYPE_CIBLE_AFFEC = TAB_SYM[index].TYPE;
        if (TAB_SYM[index].TYPE == TYPE_TABLEAU) {
            est_tableau = 1;
            taille_tableau = TAB_SYM[index].TAILLE;
        }
    } else {
        TYPE_CIBLE_AFFEC = TYPE_UNDEFINED;
    }

    if (Sym_Cour.CODE == POINT_TOKEN) {
        Sym_Suiv();
        char nom_champ[20];
        strcpy(nom_champ, Sym_Cour.NOM);
        if (MODE_GENERATION) {
            if (CAPTURE_LVALUE) {
                strncat(LVALUE_BUF, ".", sizeof(LVALUE_BUF) - strlen(LVALUE_BUF) - 1);
                strncat(LVALUE_BUF, nom_champ, sizeof(LVALUE_BUF) - strlen(LVALUE_BUF) - 1);
            } else {
                Emettre(".%s", nom_champ);
            }
        }
        Test_Symbole(ID_TOKEN, SYN_ERR_ID);

        if (index != -1) {
            if (TAB_SYM[index].TYPE == TYPE_STRUCT) {
                int champ_trouve = 0;
                TYPE_DONNEES type_champ = TYPE_UNDEFINED;
                int struct_type_index = TAB_SYM[index].REF_TYPE;

                // Si on connaît exactement le type de la structure, on cherche seulement dans ce type
                if (struct_type_index >= 0 && struct_type_index < NBR_SYM &&
                    TAB_SYM[struct_type_index].TIDF == TTYPE &&
                    TAB_SYM[struct_type_index].TYPE == TYPE_STRUCT) {
                    for (int j = 0; j < TAB_SYM[struct_type_index].NB_CHAMPS; j++) {
                        if (strcasecmp(TAB_SYM[struct_type_index].CHAMPS[j].NOM, nom_champ) == 0) {
                            champ_trouve = 1;
                            type_champ = TAB_SYM[struct_type_index].CHAMPS[j].TYPE;
                            break;
                        }
                    }
                } else {
                    // Sinon (cas ancien), on cherche dans toutes les structures définies
                    for (int i = 0; i < NBR_SYM; i++) {
                        if (TAB_SYM[i].TIDF == TTYPE && TAB_SYM[i].TYPE == TYPE_STRUCT) {
                            for (int j = 0; j < TAB_SYM[i].NB_CHAMPS; j++) {
                                if (strcasecmp(TAB_SYM[i].CHAMPS[j].NOM, nom_champ) == 0) {
                                    champ_trouve = 1;
                                    struct_type_index = i;
                                    type_champ = TAB_SYM[i].CHAMPS[j].TYPE;
                                    break;
                                }
                            }
                            if (champ_trouve) break;
                        }
                    }
                }

                if (!champ_trouve) {
                    int champ_dans_table = 0;
                    for (int i = 0; i < NBR_SYM; i++) {
                        if (TAB_SYM[i].TIDF == TSTRUCT &&
                            strcasecmp(TAB_SYM[i].NOM, nom_champ) == 0) {
                            champ_dans_table = 1;
                            break;
                        }
                    }

                    if (!champ_dans_table) {
                        Erreur_Semantique_Format(SEM_ERR_STRUCT_CHAMP, "%s.%s", nom_base, nom_champ);
                    }
                }
            } else {
                Erreur_Semantique_Format(SEM_ERR_INDEX_TAB, "'%s' n'est pas une structure", nom_base);
            }
        }

        est_tableau = 0;
        taille_tableau = 0;
        // Pour une affectation, la cible est le champ : on retient le type du champ.
        // (type_champ est calculé dans les tests ci-dessus lorsque le champ existe)
        // Si le champ n'existe pas, on garde TYPE_UNDEFINED.
        if (TAB_SYM[index].TYPE == TYPE_STRUCT) {
            // Recalcul local rapide du type du champ (sécurise le cas où il n'a pas été trouvé)
            TYPE_DONNEES type_champ2 = TYPE_UNDEFINED;
            int st = TAB_SYM[index].REF_TYPE;
            if (st >= 0) {
                for (int j = 0; j < TAB_SYM[st].NB_CHAMPS; j++) {
                    if (strcasecmp(TAB_SYM[st].CHAMPS[j].NOM, nom_champ) == 0) {
                        type_champ2 = TAB_SYM[st].CHAMPS[j].TYPE;
                        break;
                    }
                }
            }
            TYPE_CIBLE_AFFEC = type_champ2;
        }
    }

    if (Sym_Cour.CODE == CROCHET_OUVRANT_TOKEN) {
        Sym_Suiv();

        if (index != -1 && TAB_SYM[index].TYPE == TYPE_TABLEAU) {
            TYPE_CIBLE_AFFEC = TAB_SYM[index].TYPE_ELEM;
        }

        if (index != -1 && TAB_SYM[index].TYPE != TYPE_TABLEAU) {
            Erreur_Semantique_Format(SEM_ERR_INDEX_TAB,
                                   "indice sur une variable non-tableau '%s'", nom_base);
            est_tableau = 0;
            taille_tableau = 0;
        }

        if (Sym_Cour.CODE == NUM_ENTIER_TOKEN) {
            int indice = atoi(Sym_Cour.NOM);

            if (est_tableau && taille_tableau > 0) {
                if (indice < 1 || indice > taille_tableau) {
                    Erreur_Semantique_Format(SEM_ERR_INDEX_TAB,
                                           "%s[%d], taille=%d",
                                           nom_base, indice, taille_tableau);
                }
            } else if (est_tableau && taille_tableau == 0) {
                Erreur_Semantique_Format(SEM_ERR_INDEX_TAB,
                                       "tableau '%s' a une taille non définie (0)", nom_base);
            }

            Sym_Suiv();
        } else {
            EXPR_TYPE();
        }
        Test_Symbole(CROCHET_FERMANT_TOKEN, SYN_ERR_CROCHET_FERM);
    }
}

void AFFEC() {
    char nom_var[20];
    strcpy(nom_var, Sym_Cour.NOM);

    // Récupérer le type de la variable de destination
    TYPE_DONNEES type_dest = TYPE_UNDEFINED;
    int var_index = Rechercher_Sym(nom_var);
    if (var_index != -1) {
        type_dest = TAB_SYM[var_index].TYPE;
    }

    if (MODE_GENERATION) {
        // On parse la variable cible pour connaitre son type (ex: etudiant.nom) et
        // pour capturer le lvalue sous forme de texte, sans l'émettre tout de suite.
        TYPE_CIBLE_AFFEC = TYPE_UNDEFINED;
        CAPTURE_LVALUE = 1;
        VARIABLE();
        CAPTURE_LVALUE = 0;
        type_dest = TYPE_CIBLE_AFFEC;
    } else {
        TYPE_CIBLE_AFFEC = TYPE_UNDEFINED;
        VARIABLE();
        type_dest = TYPE_CIBLE_AFFEC;
    }

    if (var_index != -1) {
        Verifier_Non_Constante(nom_var);
    }
    Verifier_Non_Prog_ID(nom_var, NOM_PROGRAMME);
    Test_Symbole(AFFECTATION_TOKEN, SYN_ERR_AFFECTATION);

    // Analyser l'expression et récupérer son type
    TYPE_DONNEES type_expr = TYPE_UNDEFINED;

    if (MODE_GENERATION) {
        // Pour les chaînes, on génère strcpy(lvalue, rhs);
        // Pour les booléens, on génère lvalue = (condition);
        // Pour les autres, on génère lvalue = (expression);
        EmettreIndente("");

        if (type_dest == TYPE_CHAINE) {
            Emettre("strcpy(%s, ", LVALUE_BUF);
            if (Sym_Cour.CODE == CHAINE_LITTERALE_TOKEN) {
                Emettre("\"%s\"", Sym_Cour.NOM);
                Sym_Suiv();
            } else {
                // Variable/expr chaîne (ex: autre_var, etudiant.nom, ...)
                GENERER_EXPR();
            }
            Emettre(");\n");
        } else if (type_dest == TYPE_BOOLEEN) {
            Emettre("%s = ", LVALUE_BUF);
            GENERER_COND();
            Emettre(";\n");
        } else {
            Emettre("%s = ", LVALUE_BUF);
            GENERER_EXPR();
            Emettre(";\n");
        }

        // En mode génération, la vérification de type est faite en phase 1
        type_expr = TYPE_UNDEFINED;
    } else {
        // En phase 1 : si la cible est booléenne, on doit analyser une CONDITION.
        // Sinon, on analyse une expression arithmétique/chaine.
        if (type_dest == TYPE_BOOLEEN) {
            COND();
            type_expr = TYPE_BOOLEEN;
        } else {
            type_expr = EXPR_TYPE();
        }
    }

    // Vérifier la compatibilité des types pour l'affectation
    if (type_dest != TYPE_UNDEFINED && type_expr != TYPE_UNDEFINED) {
        if (!Types_Compatibles(type_dest, type_expr, 1)) {  // 1 = pour affectation
            Erreur_Semantique_Format(SEM_ERR_TYPE_MISMATCH,
                "%s (%s) := %s",
                nom_var, Nom_Type(type_dest), Nom_Type(type_expr));
        }
    }

    // En mode génération, la fin de ligne est déjà émise ci-dessus.
}

TYPE_DONNEES EXPR_TYPE() {
    TYPE_DONNEES type_term1 = TERM_TYPE();

    while (Sym_Cour.CODE == PLUS_TOKEN || Sym_Cour.CODE == MOINS_TOKEN || Sym_Cour.CODE == OU_TOKEN) {
        CODE_LEX op = Sym_Cour.CODE;
        Sym_Suiv();

        TYPE_DONNEES type_term2 = TERM_TYPE();

        // Vérifier la compatibilité des types pour l'opération
        if (!Types_Compatibles(type_term1, type_term2, 0)) {  // 0 = pas pour affectation
            Erreur_Semantique_Format(SEM_ERR_TYPE_MISMATCH,
                "operation %s entre %s et %s",
                (op == PLUS_TOKEN) ? "+" : (op == MOINS_TOKEN) ? "-" : "ou",
                Nom_Type(type_term1), Nom_Type(type_term2));
        }

        // Déterminer le type résultant
        if (type_term1 == TYPE_CHAINE || type_term2 == TYPE_CHAINE) {
            if (op == PLUS_TOKEN) {
                type_term1 = TYPE_CHAINE;  // Concaténation de chaînes
            } else {
                Erreur_Semantique_Format(SEM_ERR_TYPE_MISMATCH,
                    "operation non autorisee pour les chaines");
            }
        } else if (type_term1 == TYPE_REEL || type_term2 == TYPE_REEL) {
            type_term1 = TYPE_REEL;  // Promotion vers réel
        } else if (op == OU_TOKEN) {
            type_term1 = TYPE_BOOLEEN;  // OU logique produit un booléen
        }
    }

    return type_term1;
}

TYPE_DONNEES TERM_TYPE() {
    TYPE_DONNEES type_fact1 = FACT_TYPE();

    while (Sym_Cour.CODE == MULT_TOKEN || Sym_Cour.CODE == DIV_TOKEN ||
           Sym_Cour.CODE == ET_TOKEN || Sym_Cour.CODE == MOD_TOKEN) {
        CODE_LEX op = Sym_Cour.CODE;
        Sym_Suiv();

        TYPE_DONNEES type_fact2 = FACT_TYPE();

        // Vérifier la compatibilité des types
        if (!Types_Compatibles(type_fact1, type_fact2, 0)) {
            Erreur_Semantique_Format(SEM_ERR_TYPE_MISMATCH,
                "operation %s entre %s et %s",
                (op == MULT_TOKEN) ? "*" : (op == DIV_TOKEN) ? "/" :
                (op == ET_TOKEN) ? "et" : "mod",
                Nom_Type(type_fact1), Nom_Type(type_fact2));
        }

        // Déterminer le type résultant
        if (type_fact1 == TYPE_CHAINE || type_fact2 == TYPE_CHAINE) {
            Erreur_Semantique_Format(SEM_ERR_TYPE_MISMATCH,
                "operation non autorisee pour les chaines");
        } else if (type_fact1 == TYPE_REEL || type_fact2 == TYPE_REEL) {
            type_fact1 = TYPE_REEL;
        } else if (op == ET_TOKEN) {
            type_fact1 = TYPE_BOOLEEN;  // ET logique produit un booléen
        }
    }

    return type_fact1;
}

TYPE_DONNEES FACT_TYPE() {
    if (Sym_Cour.CODE == CHAINE_LITTERALE_TOKEN) {
        Sym_Suiv();
        return TYPE_CHAINE;
    }

    if (Sym_Cour.CODE == BOOLEEN_LITTERALE_TOKEN) {
        Sym_Suiv();
        return TYPE_BOOLEEN;
    }

    if (Sym_Cour.CODE == NON_TOKEN) {
        Sym_Suiv();
        TYPE_DONNEES type_fact = FACT_TYPE();
        // "non" ne s'applique qu'aux booléens
        if (type_fact != TYPE_BOOLEEN) {
            Erreur_Semantique_Format(SEM_ERR_TYPE_MISMATCH,
                "'non' applique a un %s au lieu d'un booleen",
                Nom_Type(type_fact));
        }
        return TYPE_BOOLEEN;
    }

    return NUM_FACT_TYPE();
}

TYPE_DONNEES NUM_FACT_TYPE() {
    switch (Sym_Cour.CODE) {
        case ID_TOKEN: {
            char nom_var[20];
            strcpy(nom_var, Sym_Cour.NOM);

            int index = Verifier_ID_Declare(nom_var, 0);

            if (index != -1) {
                TYPE_DONNEES type_var = TAB_SYM[index].TYPE;

                // Consommer le token ID
                Sym_Suiv();

                // Gérer les champs de structure et indices de tableau
                if (Sym_Cour.CODE == POINT_TOKEN) {
                    Sym_Suiv();
                    Sym_Suiv();  // Consommer le nom du champ
                    // Pour simplifier, on retourne TYPE_UNDEFINED pour les champs
                    return TYPE_UNDEFINED;
                }
                if (Sym_Cour.CODE == CROCHET_OUVRANT_TOKEN) {
                    // Acces a un element de tableau
                    Sym_Suiv();
                    EXPR_TYPE();
                    Test_Symbole(CROCHET_FERMANT_TOKEN, SYN_ERR_CROCHET_FERM);
                    if (type_var == TYPE_TABLEAU) {
                        return TAB_SYM[index].TYPE_ELEM;
                    }
                }

                return type_var;
            } else {
                Sym_Suiv();
                return TYPE_UNDEFINED;
            }
        }

        case NUM_ENTIER_TOKEN:
            Sym_Suiv();
            return TYPE_ENTIER;

        case NUM_REEL_TOKEN:
            Sym_Suiv();
            return TYPE_REEL;

        case PARENTHESE_OUVRANTE_TOKEN:
            Sym_Suiv();
            TYPE_DONNEES type_expr = EXPR_TYPE();
            Test_Symbole(PARENTHESE_FERMANTE_TOKEN, SYN_ERR_PAR_FERM);
            return type_expr;

        case PLUS_TOKEN:
        case MOINS_TOKEN:
            SIGNE();
            return NUM_FACT_TYPE();

        default:
            Erreur_Syntaxique(SYN_ERR_FACT);
            return TYPE_UNDEFINED;
    }
}

// Versions originales des fonctions (pour compatibilité)
void EXPR() {
    if (MODE_GENERATION) {
        GENERER_EXPR();
    } else {
        EXPR_TYPE();
    }
}

void TERM() {
    if (MODE_GENERATION) {
        GENERER_TERM();
    } else {
        TERM_TYPE();
    }
}

void FACT() {
    if (MODE_GENERATION) {
        GENERER_FACT();
    } else {
        FACT_TYPE();
    }
}

void NUM_FACT() {
    NUM_FACT_TYPE();
}
void ECRIRE() {
    Test_Symbole(ECRIRE_TOKEN, SYN_ERR_ECRIRE);
    Test_Symbole(PARENTHESE_OUVRANTE_TOKEN, SYN_ERR_PAR_FERM);

    if (MODE_GENERATION) {
        Init_Printf_Buffers();
    }

    // Premier élément
    if (MODE_GENERATION) {
        GENERER_EXPR_ECRIRE();
    } else {
        EXPR_TYPE();
    }

    // Éléments suivants
    while (Sym_Cour.CODE == VIRGULE_TOKEN) {
        Sym_Suiv();
        if (MODE_GENERATION) {
            GENERER_EXPR_ECRIRE();
        } else {
            EXPR_TYPE();
        }
    }

    if (MODE_GENERATION) {
        // Émettre le printf final
        EmettreIndente("printf(\"%s\"", FORMAT_PRINTF);
        if (ARGS_INDEX > 0) {
            Emettre(", %s", ARGS_PRINTF);
        }
        Emettre(");\n");
    }

    Test_Symbole(PARENTHESE_FERMANTE_TOKEN, SYN_ERR_PAR_FERM);
}

void LIRE() {
    Test_Symbole(LIRE_TOKEN, SYN_ERR_LIRE);
    Test_Symbole(PARENTHESE_OUVRANTE_TOKEN, SYN_ERR_PAR_FERM);

    char nom_var[20];
    strcpy(nom_var, Sym_Cour.NOM);

    TYPE_DONNEES type_var = TYPE_UNDEFINED;
    int var_index = Rechercher_Sym(nom_var);
    if (var_index != -1) {
        type_var = TAB_SYM[var_index].TYPE;
        Verifier_Non_Constante(nom_var);
    }

    if (MODE_GENERATION) {
        EmettreIndente("scanf(\"");
        if (type_var == TYPE_ENTIER) {
            Emettre("%%d");
        } else if (type_var == TYPE_REEL) {
            Emettre("%%lf");
        } else if (type_var == TYPE_CHAINE) {
            Emettre("%%s");
        } else if (type_var == TYPE_CARACTERE) {
            Emettre(" %%c");
        } else {
            Emettre("%%d");
        }
        Emettre("\", &%s)", nom_var);
    }

    // En génération, on a déjà écrit la variable dans scanf(&var).
    // On appelle VARIABLE() uniquement pour consommer la syntaxe (indices/champs)
    // sans ré-émettre du code (sinon on obtient: scanf(...,&x)x).
    if (MODE_GENERATION) {
        int saved_mode = MODE_GENERATION;
        MODE_GENERATION = 0;
        VARIABLE();
        MODE_GENERATION = saved_mode;
    } else {
        VARIABLE();
    }

    int premier = 0;
    while (Sym_Cour.CODE == VIRGULE_TOKEN) {
        Sym_Suiv();
        strcpy(nom_var, Sym_Cour.NOM);

        var_index = Rechercher_Sym(nom_var);
        if (var_index != -1) {
            TYPE_DONNEES type_var2 = TAB_SYM[var_index].TYPE;

            if (type_var != TYPE_UNDEFINED && type_var2 != TYPE_UNDEFINED &&
                type_var != type_var2) {
                Erreur_Semantique_Format(SEM_ERR_TYPE_MISMATCH,
                    "types incompatibles dans lire(): %s et %s",
                    Nom_Type(type_var), Nom_Type(type_var2));
            }

            Verifier_Non_Constante(nom_var);
        }

        if (MODE_GENERATION) {
            if (premier == 0) {
                Emettre(";\n");
                premier = 1;
            }
            EmettreIndente("scanf(\"");
            if (type_var == TYPE_ENTIER) {
                Emettre("%%d");
            } else if (type_var == TYPE_REEL) {
                Emettre("%%lf");
            } else if (type_var == TYPE_CHAINE) {
                Emettre("%%s");
            } else if (type_var == TYPE_CARACTERE) {
                Emettre(" %%c");
            } else {
                Emettre("%%d");
            }
            Emettre("\", &%s)", nom_var);
        }

        if (MODE_GENERATION) {
            int saved_mode = MODE_GENERATION;
            MODE_GENERATION = 0;
            VARIABLE();
            MODE_GENERATION = saved_mode;
        } else {
            VARIABLE();
        }
    }

    if (MODE_GENERATION) {
        Emettre(";\n");
    }

    Test_Symbole(PARENTHESE_FERMANTE_TOKEN, SYN_ERR_PAR_FERM);
}

void SI() {
    Test_Symbole(SI_TOKEN, SYN_ERR_SI);

    if (MODE_GENERATION) {
        EmettreIndente("if (");
    }

    if (MODE_GENERATION) {
        GENERER_COND();
    } else {
        COND();
    }

    if (MODE_GENERATION) {
        Emettre(") {\n");
        NIVEAU_INDENTATION++;
    }
    Test_Symbole(ALORS_TOKEN, SYN_ERR_ALORS);
    INSTS_CORPS();
    if (MODE_GENERATION) {
        NIVEAU_INDENTATION--;
        EmettreIndente("}");
    }

    if (Sym_Cour.CODE == SINON_TOKEN) {
        Sym_Suiv();

        if (MODE_GENERATION) {
            Emettre(" else {\n");
            NIVEAU_INDENTATION++;
        }

        INSTS_CORPS();

        if (MODE_GENERATION) {
            NIVEAU_INDENTATION--;
            EmettreIndente("}");
        }
    }

    if (MODE_GENERATION) {
        Emettre("\n");
    }

    Test_Symbole(FIN_SI_TOKEN, SYN_ERR_SI);
}

void SELON() {
    Test_Symbole(SELON_TOKEN, SYN_ERR_SELON);
    Test_Symbole(PARENTHESE_OUVRANTE_TOKEN, SYN_ERR_PAR_FERM);

    char nom_var[20];
    strcpy(nom_var, Sym_Cour.NOM);
    Test_Symbole(ID_TOKEN, SYN_ERR_ID);

    if (MODE_GENERATION) {
        EmettreIndente("switch (%s) {\n", nom_var);
        NIVEAU_INDENTATION++;
    }

    Test_Symbole(PARENTHESE_FERMANTE_TOKEN, SYN_ERR_PAR_FERM);
    LISTE_CAS();
    CAS_AUTRES();

    if (MODE_GENERATION) {
        NIVEAU_INDENTATION--;
        EmettreIndente("}\n");
    }

    Test_Symbole(FIN_SELON_TOKEN, SYN_ERR_FIN_SELON);
}

void LISTE_CAS() {
    Test_Symbole(SI_TOKEN, SYN_ERR_SI);
    if (MODE_GENERATION) {
        EmettreIndente("case ");
    }
    VALEUR();
    Test_Symbole(DEUX_POINTS_TOKEN, SYN_ERR_DEUX_POINTS);

    if (MODE_GENERATION) {
        Emettre(":\n");
        NIVEAU_INDENTATION++;
    }

    INSTS_CAS();

    if (MODE_GENERATION) {
        EmettreIndente("break;\n");
        NIVEAU_INDENTATION--;
    }

    Test_Symbole(VIRGULE_TOKEN, SYN_ERR_VIRGULE);
    Test_Symbole(SORTIR_TOKEN, SYN_ERR_SORTIR);

    while (Sym_Cour.CODE == SI_TOKEN) {
        char saved_char = Car_Cour;
        long saved_pos = ftell(Fichier);
        TSym_Cour saved_sym = Sym_Cour;

        Sym_Suiv();
        CODE_LEX next_token = Sym_Cour.CODE;

        fseek(Fichier, saved_pos, SEEK_SET);
        Car_Cour = saved_char;
        Sym_Cour = saved_sym;

        if (next_token == AUTRE_CAS_TOKEN) {
            break;
        }

        Sym_Suiv();

        if (MODE_GENERATION) {
            EmettreIndente("case ");
        }

        VALEUR();
        Test_Symbole(DEUX_POINTS_TOKEN, SYN_ERR_DEUX_POINTS);

        if (MODE_GENERATION) {
            Emettre(":\n");
            NIVEAU_INDENTATION++;
        }

        INSTS_CAS();

        if (MODE_GENERATION) {
            EmettreIndente("break;\n");
            NIVEAU_INDENTATION--;
        }

        Test_Symbole(VIRGULE_TOKEN, SYN_ERR_VIRGULE);
        Test_Symbole(SORTIR_TOKEN, SYN_ERR_SORTIR);
    }
}

void CAS_AUTRES() {
    if (Sym_Cour.CODE == SI_TOKEN) {
        Sym_Suiv();
        Test_Symbole(AUTRE_CAS_TOKEN, SYN_ERR_AUTRE_CAS);
        Test_Symbole(DEUX_POINTS_TOKEN, SYN_ERR_DEUX_POINTS);

        if (MODE_GENERATION) {
            EmettreIndente("default:\n");
            NIVEAU_INDENTATION++;
        }

        INSTS_CAS();

        if (MODE_GENERATION) {
            EmettreIndente("break;\n");
            NIVEAU_INDENTATION--;
        }
    }
}

void INSTS_CAS() {
    INST();
    while (Sym_Cour.CODE == POINT_VIRGULE_TOKEN) {
        Sym_Suiv();
        INST();
    }
}

void POUR() {
    Test_Symbole(POUR_TOKEN, SYN_ERR_POUR);

    char nom_compteur[20];
    strcpy(nom_compteur, Sym_Cour.NOM);
    Test_Symbole(ID_TOKEN, SYN_ERR_ID);

    if (MODE_GENERATION) {
        EmettreIndente("for (%s = ", nom_compteur);
    }

    Test_Symbole(AFFECTATION_TOKEN, SYN_ERR_AFFECTATION);

    if (MODE_GENERATION) {
        GENERER_EXPR();
    } else {
        EXPR_TYPE();
    }

    if (MODE_GENERATION) {
        Emettre("; %s <= ", nom_compteur);
    }

    Test_Symbole(JUSQUA_TOKEN, SYN_ERR_JUSQUA_QUE);

    if (MODE_GENERATION) {
        GENERER_EXPR();
    } else {
        EXPR_TYPE();
    }

    if (Sym_Cour.CODE == PAS_TOKEN) {
        Sym_Suiv();

        if (MODE_GENERATION) {
            Emettre("; %s += ", nom_compteur);
            GENERER_EXPR();
        } else {
            EXPR_TYPE();
        }
    } else {
        if (MODE_GENERATION) {
            Emettre("; %s++", nom_compteur);
        }
    }

    if (MODE_GENERATION) {
        Emettre(") {\n");
        NIVEAU_INDENTATION++;
    }

    Test_Symbole(FAIRE_TOKEN, SYN_ERR_ALORS);
    INSTS_CORPS();

    if (MODE_GENERATION) {
        NIVEAU_INDENTATION--;
        EmettreIndente("}\n");
    }

    Test_Symbole(FIN_POUR_TOKEN, SYN_ERR_FIN_POUR);
}

void TANT_QUE() {
    Test_Symbole(TANT_QUE_TOKEN, SYN_ERR_TANT_QUE);

    if (MODE_GENERATION) {
        EmettreIndente("while (");
    }

    if (MODE_GENERATION) {
        GENERER_COND();
    } else {
        COND();
    }

    if (MODE_GENERATION) {
        Emettre(") {\n");
        NIVEAU_INDENTATION++;
    }

    Test_Symbole(FAIRE_TOKEN, SYN_ERR_ALORS);
    INSTS_CORPS();

    if (MODE_GENERATION) {
        NIVEAU_INDENTATION--;
        EmettreIndente("}\n");
    }

    Test_Symbole(FIN_TANT_QUE_TOKEN, SYN_ERR_FIN_TANT_QUE);
}

void REPETER() {
    Test_Symbole(REPETER_TOKEN, SYN_ERR_REPETER);

    if (MODE_GENERATION) {
        EmettreIndente("do {\n");
        NIVEAU_INDENTATION++;
    }

    INSTS_CORPS();

    if (MODE_GENERATION) {
        NIVEAU_INDENTATION--;
        EmettreIndente("} while (!(");
    }

    Test_Symbole(JUSQUA_QUE_TOKEN, SYN_ERR_JUSQUA_QUE);

    if (MODE_GENERATION) {
        GENERER_COND();
        Emettre("));\n");
    } else {
        COND();
    }
}

void COND() {
    if (MODE_GENERATION) {
        GENERER_COND();
    } else {
        COND_TERM();
        while (Sym_Cour.CODE == OU_TOKEN) {
            Sym_Suiv();
            COND_TERM();
        }
    }
}

void COND_TERM() {
    COND_FACT();
    while (Sym_Cour.CODE == ET_TOKEN) {
        Sym_Suiv();
        COND_FACT();
    }
}

void COND_FACT() {
    if (Sym_Cour.CODE == NON_TOKEN) {
        Sym_Suiv();
        COND_FACT();
    } else if (Sym_Cour.CODE == PARENTHESE_OUVRANTE_TOKEN) {
        Sym_Suiv();
        COND();
        Test_Symbole(PARENTHESE_FERMANTE_TOKEN, SYN_ERR_PAR_FERM);
    } else {
        TYPE_DONNEES type_expr1 = EXPR_TYPE();

        if (Sym_Cour.CODE == EGAL_TOKEN || Sym_Cour.CODE == DIFFERENT_TOKEN ||
            Sym_Cour.CODE == INFERIEUR_TOKEN || Sym_Cour.CODE == SUPERIEUR_TOKEN ||
            Sym_Cour.CODE == INFERIEUR_EGAL_TOKEN || Sym_Cour.CODE == SUPERIEUR_EGAL_TOKEN) {

            RELOP();

            TYPE_DONNEES type_expr2 = EXPR_TYPE();

            if (!Types_Compatibles(type_expr1, type_expr2, 0)) {
                Erreur_Semantique_Format(SEM_ERR_TYPE_MISMATCH,
                    "comparaison entre %s et %s",
                    Nom_Type(type_expr1), Nom_Type(type_expr2));
            }
        }
    }
}

void RELOP() {
    switch (Sym_Cour.CODE) {
        case EGAL_TOKEN:
        case DIFFERENT_TOKEN:
        case INFERIEUR_TOKEN:
        case SUPERIEUR_TOKEN:
        case INFERIEUR_EGAL_TOKEN:
        case SUPERIEUR_EGAL_TOKEN:
            Sym_Suiv();
            break;
        default:
            Erreur_Syntaxique(SYN_ERR_RELOP);
    }
}

void VALEUR() {
    switch (Sym_Cour.CODE) {
        case NUM_ENTIER_TOKEN:
        case NUM_REEL_TOKEN:
        case CHAINE_LITTERALE_TOKEN:
            if (MODE_GENERATION) {
                Emettre("%s", Sym_Cour.NOM);
            }
            Sym_Suiv();
            break;
        default:
            Erreur_Syntaxique(SYN_ERR_VALEUR);
    }
}

void SIGNE() {
    if (Sym_Cour.CODE == PLUS_TOKEN || Sym_Cour.CODE == MOINS_TOKEN) {
        Sym_Suiv();
    } else {
        Erreur_Syntaxique(SYN_ERR_SIGNE);
    }
}

// =====================================================
// FONCTIONS UTILITAIRES POUR AFFICHAGE
// =====================================================

void Afficher_Table_Symboles() {
    printf("\n=== TABLE DES SYMBOLES ===\n");
    printf("%-20s %-10s %-15s %-10s %-10s\n",
           "NOM", "TYPE", "ELEM", "CATEGORIE", "VALEUR", "ADRESSE");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < NBR_SYM; i++) {
        char *categorie;
        switch (TAB_SYM[i].TIDF) {
            case TPROG: categorie = "PROG"; break;
            case TCONST: categorie = "CONST"; break;
            case TVAR: categorie = "VAR"; break;
            case TSTRUCT: categorie = "CHAMP"; break;
            case TTYPE: categorie = "TYPE"; break;
            default: categorie = "???"; break;
        }

        char *type;
        switch (TAB_SYM[i].TYPE) {
            case TYPE_ENTIER: type = "ENTIER"; break;
            case TYPE_REEL: type = "REEL"; break;
            case TYPE_CARACTERE: type = "CARACTERE"; break;
            case TYPE_CHAINE: type = "CHAINE"; break;
            case TYPE_BOOLEEN: type = "BOOLEEN"; break;
            case TYPE_TABLEAU: type = "TABLEAU"; break;
            case TYPE_STRUCT: type = "STRUCT"; break;
            default: type = "UNDEF"; break;
        }

        char *type_elem = "-";
        if (TAB_SYM[i].TYPE == TYPE_TABLEAU) {
            switch (TAB_SYM[i].TYPE_ELEM) {
                case TYPE_ENTIER: type_elem = "ENTIER"; break;
                case TYPE_REEL: type_elem = "REEL"; break;
                case TYPE_CARACTERE: type_elem = "CARACTERE"; break;
                case TYPE_CHAINE: type_elem = "CHAINE"; break;
                case TYPE_BOOLEEN: type_elem = "BOOLEEN"; break;
                default: type_elem = "UNDEF"; break;
            }
        }

        printf("%-20s %-10s %-10s %-15s %-10d %-10d\n",
               TAB_SYM[i].NOM, type, type_elem, categorie, TAB_SYM[i].VAL, TAB_SYM[i].ADRESSE);
    }
    printf("Nombre total de symboles: %d\n", NBR_SYM);

    printf("\n=== RELATIONS STRUCTURES/CHAMPS ===\n");
    for (int i = 0; i < NBR_SYM; i++) {
        if (TAB_SYM[i].TYPE == TYPE_STRUCT && TAB_SYM[i].TIDF == TTYPE) {
            printf("Structure '%s' a %d champs:\n", TAB_SYM[i].NOM, TAB_SYM[i].NB_CHAMPS);
            for (int j = 0; j < TAB_SYM[i].NB_CHAMPS; j++) {
                char *type_champ;
                switch (TAB_SYM[i].CHAMPS[j].TYPE) {
                    case TYPE_ENTIER: type_champ = "ENTIER"; break;
                    case TYPE_REEL: type_champ = "REEL"; break;
                    case TYPE_CARACTERE: type_champ = "CARACTERE"; break;
                    case TYPE_CHAINE: type_champ = "CHAINE"; break;
                    case TYPE_BOOLEEN: type_champ = "BOOLEEN"; break;
                    case TYPE_TABLEAU: type_champ = "TABLEAU"; break;
                    case TYPE_STRUCT: type_champ = "STRUCT"; break;
                    default: type_champ = "UNDEF"; break;
                }
                printf("  - %s : %s\n",
                       TAB_SYM[i].CHAMPS[j].NOM,
                       type_champ);
            }
        }
    }
}

// =====================================================
// FONCTION PRINCIPALE
// =====================================================

int main() {
    printf("=== COMPILATEUR PSEUDO-CODE VERS C ===\n\n");

    // ==================== PHASE 1: ANALYSE ====================
    printf("=== PHASE 1: Analyse lexicale, syntaxique et semantique ===\n");

    Fichier = fopen("algo.txt", "r");
    if (!Fichier) {
        printf("Erreur : fichier algo.txt introuvable.\n");
        return 1;
    }

    printf("=== Contenu du fichier algo.txt ===\n\n");
    int c;
    while ((c = fgetc(Fichier)) != EOF) {
        putchar(c);
    }
    printf("\n=== Fin du contenu ===\n\n");

    rewind(Fichier);

    NBR_SYM = 0;
    OFFSET = 0;
    AVANT_BEGIN = 1;
    NB_ERREURS_SEMANTIQUES = 0;
    MODE_GENERATION = 0;

    Lire_Car();
    if (Car_Cour == EOF) {
        printf("Erreur : fichier vide\n");
        fclose(Fichier);
        return 1;
    }

    Sym_Suiv();
    PROGRAM();

    if (Sym_Cour.CODE != FIN_FICHIER_TOKEN) {
        printf("Erreur : contenu apres la fin du programme\n");
        printf("Token restant : %s (%d)\n", Sym_Cour.NOM, Sym_Cour.CODE);
        NB_ERREURS_SEMANTIQUES++;
    }

    if (NB_ERREURS_SEMANTIQUES == 0) {
        printf("\nSucces : programme syntaxiquement et semantiquement correct !\n");
    } else {
        printf("\nEchec : programme contient %d erreur(s) semantique(s) !\n",
               NB_ERREURS_SEMANTIQUES);
        fclose(Fichier);
        return 1;
    }

    Afficher_Table_Symboles();
    fclose(Fichier);

    // ==================== PHASE 2: GENERATION ====================
    printf("\n=== PHASE 2: Generation de code C ===\n");

    Fichier = fopen("algo.txt", "r");
    if (!Fichier) {
        printf("Erreur : fichier algo.txt introuvable (phase 2).\n");
        return 1;
    }

    Fichier_C = fopen("output.c", "w");
    if (!Fichier_C) {
        printf("Erreur : impossible de creer output.c\n");
        fclose(Fichier);
        return 1;
    }

    MODE_GENERATION = 1;
    NIVEAU_INDENTATION = 0;

    // Générer l'en-tête et les déclarations
    Generer_Declarations_Globales();

    // Réinitialiser l'analyseur lexical
    rewind(Fichier);
    NBR_SYM = 0;
    OFFSET = 0;
    AVANT_BEGIN = 1;
    Lire_Car();
    Sym_Suiv();

    // Régénérer le programme pour la génération de code
    PROGRAM();

    fclose(Fichier);
    fclose(Fichier_C);

    printf("\nSucces : code C genere dans output.c\n");

    // Afficher le code généré
    printf("\n=== Contenu de output.c ===\n\n");
    FILE *f = fopen("output.c", "r");
    if (f) {
        while ((c = fgetc(f)) != EOF) {
            putchar(c);
        }
        fclose(f);
    }
    printf("\n=== Fin du code genere ===\n");

    return 0;
}
