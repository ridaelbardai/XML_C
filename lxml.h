#ifndef LITTLE_XML_H
#define LITTLE_XML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//
//  Definitions
//

struct _XMLAttribut
{
    char *clef;
    char *valeur;
};
typedef struct _XMLAttribut XMLAttribut;

void XMLAttribut_free(XMLAttribut *attr);

struct _XMLAttributListe
{
    int taille_tas;
    int taille;
    XMLAttribut *donees;
};
typedef struct _XMLAttributListe XMLAttributListe;

void XMLAttributListe_init(XMLAttributListe *Liste);
void XMLAttributListe_ajouter(XMLAttributListe *Liste, XMLAttribut *attr);

struct _XMLBaliseListe
{
    int taille_tas;
    int taille;
    struct _XMLBalise **donees;
};
typedef struct _XMLBaliseListe XMLBaliseListe;

void XMLBaliseListe_init(XMLBaliseListe *Liste);
void XMLBaliseListe_ajouter(XMLBaliseListe *Liste, struct _XMLBalise *node);
struct _XMLBalise *XMLBaliseListe_pos(XMLBaliseListe *Liste, int index);
void XMLBaliseListe_free(XMLBaliseListe *Liste);

struct _XMLBalise
{
    char *tag;
    char *text;
    struct _XMLBalise *parent;
    XMLAttributListe attributs;
    XMLBaliseListe fils;
};
typedef struct _XMLBalise XMLBalise;

XMLBalise *XMLBalise_new(XMLBalise *parent);
void XMLBalise_free(XMLBalise *node);
XMLBalise *XMLBalise_fils_pos(XMLBalise *parent, int index);
XMLBaliseListe *XMLBalise_fils(XMLBalise *parent, const char *tag);
char *XMLBalise_attr_val(XMLBalise *node, char *clef);
XMLAttribut *XMLBalise_attr(XMLBalise *node, char *clef);

struct _XMLDocument
{
    XMLBalise *root;
};
typedef struct _XMLDocument XMLDocument;

int XMLDocument_charger(XMLDocument *doc, const char *path);
int XMLDocument_ecrire(XMLDocument *doc, const char *path, int indent);
void XMLDocument_free(XMLDocument *doc);

/**
 * fini_par
 * entrées: haystaque(la phrase à vérifier)
 *          needle(la valeur qui doit se trouver à la fin de haystaque)
 * sorties: TRUE si c'est vrai
 * cette fonction est utilisé pour savoir si un commentaire se términe par "-->"
 */
int fini_par(const char *haystack, const char *needle)
{
    int h_taille = strlen(haystack);
    int n_taille = strlen(needle);

    if (h_taille < n_taille)
        return FALSE;

    for (int i = 0; i < n_taille; i++)
    {
        if (haystack[h_taille - n_taille + i] != needle[i])
            return FALSE;
    }

    return TRUE;
}

/**
 * libérer les valeurs d'un attribut temporaire
 */
void XMLAttribut_free(XMLAttribut *attr)
{
    free(attr->clef);
    free(attr->valeur);
}
/**
 * initialiser la liste des attributs
 */
void XMLAttributListe_init(XMLAttributListe *Liste)
{
    Liste->taille_tas = 1;
    Liste->taille = 0;
    Liste->donees = (XMLAttribut *)malloc(sizeof(XMLAttribut) * Liste->taille_tas);
}

/**
 * ajouter un attribut à une liste des attributs
 */
void XMLAttributListe_ajouter(XMLAttributListe *Liste, XMLAttribut *attr)
{
    while (Liste->taille >= Liste->taille_tas)
    {
        Liste->taille_tas *= 2;
        Liste->donees = (XMLAttribut *)realloc(Liste->donees, sizeof(XMLAttribut) * Liste->taille_tas);
    }

    Liste->donees[Liste->taille++] = *attr;
}

/**
 * initialiser une balise
 */
void XMLBaliseListe_init(XMLBaliseListe *Liste)
{
    Liste->taille_tas = 1;
    Liste->taille = 0;
    Liste->donees = (XMLBalise **)malloc(sizeof(XMLBalise *) * Liste->taille_tas);
}

/**
 * ajouter une balise à une liste des balises
 */
void XMLBaliseListe_ajouter(XMLBaliseListe *Liste, XMLBalise *node)
{
    while (Liste->taille >= Liste->taille_tas)
    {
        Liste->taille_tas *= 2;
        Liste->donees = (XMLBalise **)realloc(Liste->donees, sizeof(XMLBalise *) * Liste->taille_tas);
    }

    Liste->donees[Liste->taille++] = node;
}

/**
 * récupérer une balise selon sa position dans une liste des balise
 */
XMLBalise *XMLBaliseListe_pos(XMLBaliseListe *Liste, int index)
{
    return Liste->donees[index];
}
/**
 * libérer une liste des balises
 */
void XMLBaliseListe_free(XMLBaliseListe *Liste)
{
    free(Liste);
}

/**
 * XMLBalise_new
 * entrées: parent(balise père de la balise à créer)
 * sortie : nouvelle balise créer
 * cette fonction crée une balise et l'initialise
 */
XMLBalise *XMLBalise_new(XMLBalise *parent)
{
    XMLBalise *node = (XMLBalise *)malloc(sizeof(XMLBalise));
    node->parent = parent;
    node->tag = NULL;
    node->text = NULL;
    XMLAttributListe_init(&node->attributs);
    XMLBaliseListe_init(&node->fils);
    if (parent)
        XMLBaliseListe_ajouter(&parent->fils, node);
    return node;
}
/**
 * libérer une balise
 */
void XMLBalise_free(XMLBalise *node)
{
    if (node->tag)
        free(node->tag);
    if (node->text)
        free(node->text);
    for (int i = 0; i < node->attributs.taille; i++)
        XMLAttribut_free(&node->attributs.donees[i]);
    free(node);
}
/**
 * récupérer un fils d'une balise selon sa positin (index)
 */
XMLBalise *XMLBalise_fils_pos(XMLBalise *parent, int index)
{
    return parent->fils.donees[index];
}

/**
 * XMLBalise_fils
 * entrées: balise père, nom de la balise à chercher
 * sortie : liste des balises
 * cette fonction reçoit une balise est le nom d'une balise
 * qui peut être dedant, si la balise <tag/> est trouvé elle retourne
 * touts ces données
 */
XMLBaliseListe *XMLBalise_fils(XMLBalise *parent, const char *tag)
{
    XMLBaliseListe *Liste = (XMLBaliseListe *)malloc(sizeof(XMLBaliseListe));
    XMLBaliseListe_init(Liste);

    for (int i = 0; i < parent->fils.taille; i++)
    {
        XMLBalise *child = parent->fils.donees[i];
        if (!strcmp(child->tag, tag))
            XMLBaliseListe_ajouter(Liste, child);
    }
    return Liste;
}

/**
 * recoit une balise et le nom de l'un de ces attributs
 * pour retourner ca valeur
 */
char *XMLBalise_attr_val(XMLBalise *node, char *clef)
{
    for (int i = 0; i < node->attributs.taille; i++)
    {
        XMLAttribut attr = node->attributs.donees[i];
        if (!strcmp(attr.clef, clef))
            return attr.valeur;
    }
    return NULL;
}

/**
 * recoit une balise et le nom de l'un de ces attributs
 * pour retourner tout l'attribut
 */
XMLAttribut *XMLBalise_attr(XMLBalise *node, char *clef)
{
    for (int i = 0; i < node->attributs.taille; i++)
    {
        XMLAttribut *attr = &node->attributs.donees[i];
        if (!strcmp(attr->clef, clef))
            return attr;
    }
    return NULL;
}

//enumération des types d'un tag
enum _typeTag
{
    TAG_DEBUT,
    TAG_ENLIGNE
};
typedef enum _typeTag typeTag;


/**
 * cette fonction permet d'identifier
 * si cette balise est de type :
 * DEBUT: <tag></tag>
 * ENLIGNE: <tag/>
 * et traiter les attributs et leurs valeurs
 */
static typeTag parse_attrs(char *buf, int *i, char *lex, int *lexi, XMLBalise *curr_node)
{ //variable pour stocker les attributs en cours
    XMLAttribut curr_attr = {0, 0};
    //tantque c'est pas la fin de la balise
    while (buf[*i] != '>')
    {
        lex[(*lexi)++] = buf[(*i)++];

        // stocker nom de la balise (tag)
        if (buf[*i] == ' ' && !curr_node->tag)
        {
            lex[*lexi] = '\0';
            curr_node->tag = strdup(lex);
            *lexi = 0;
            (*i)++;
            continue;
        }

        // ignorer les espaces
        if (lex[*lexi - 1] == ' ')
        {
            (*lexi)--;
        }

        // Nom de l'attribut est stocké après avoir trouver "="
        if (buf[*i] == '=')
        {
            lex[*lexi] = '\0';
            curr_attr.clef = strdup(lex);
            *lexi = 0;
            continue;
        }

        // Valeur de l'attribut commence après (")
        if (buf[*i] == '"')
        { //nom de l'attribut non définit
            if (!curr_attr.clef)
            {
                fprintf(stderr, "valeur sans clé d'attribut\n");
                return TAG_DEBUT;
            }
            //réinitialisation
            *lexi = 0;
            (*i)++;
            //sinon tantque c'est pas la fin de la valeur de l'attribut
            while (buf[*i] != '"')
                lex[(*lexi)++] = buf[(*i)++];
            lex[*lexi] = '\0';
            //stocker la valeur dans le champ val de curr_attr
            curr_attr.valeur = strdup(lex);
            //ajouter l'attribut à la liste des attributs de cette balise
            XMLAttributListe_ajouter(&curr_node->attributs, &curr_attr);
            //réinitialisation
            curr_attr.clef = NULL;
            curr_attr.valeur = NULL;
            *lexi = 0;
            (*i)++;
            continue;
        }

        // balise ENLIGNE de ce type : <balise />
        if (buf[*i - 1] == '/' && buf[*i] == '>')
        {
            lex[*lexi] = '\0';
            if (!curr_node->tag)
                curr_node->tag = strdup(lex);
            (*i)++;
            //retourner le type ENLIGNE
            return TAG_ENLIGNE;
        }
    }

    return TAG_DEBUT;
}


/**
 * XMLDocument_charger
 * entrées: doc(variable qui va contenir la balise racine )
 *          initialisé avant et passe par les paramètre
 *          path(chemin du fichier XML)
 * sortie : False en cas d'erreur de syntax dans le fichier XML
 * 
 * -cette fonction recoit une variable de type XMLDocument
 *  chaque document est identifié par une balise racine
 * -ici doc recpresente la racine du document
 */
int XMLDocument_charger(XMLDocument *doc, const char *path)
{
    //ouverture du fichier
    FILE *file = fopen(path, "r");
    if (!file)
    {
        //erreur lors l'ouverture
        fprintf(stderr, "ne peut pas ouvrir le fichier '%s'\n", path);
        return FALSE;
    }

    //pour calculer la taille du fichier
    fseek(file, 0, SEEK_END);
    int taille = ftell(file);
    //revenir au debut
    fseek(file, 0, SEEK_SET);

    //la variable buf stocke le texte chargé
    char *buf = (char *)malloc(sizeof(char) * taille + 1);
    fread(buf, 1, taille, file);
    fclose(file);
    //indiquer la fin du texte
    buf[taille] = '\0';

    //balise racine
    doc->root = XMLBalise_new(NULL);

    //variable temporaire pour stocker le contenue en traitement
    char lex[256];
    // indice pour parcourir "lex"
    int lexi = 0;
    // indice pour parcourir le texte chargé dan "buf"
    int i = 0;

    //contient à chaque fois la balise en traitement
    XMLBalise *curr_node = doc->root;

    //tant que c'est pas la fin du fichier
    while (buf[i] != '\0')
    { //si le fichier commence par "<"
        if (buf[i] == '<')
        { //indiquer la fin du text chargé
            lex[lexi] = '\0';

            // si il existe un texte avant la première balise
            if (lexi > 0)
            {
                if (!curr_node)
                {
                    fprintf(stderr, "Texte avant les balises\n");
                    return FALSE;
                }
                //si curr_node contient deja une balise
                //donc c'est le texte de la balise courante
                curr_node->text = strdup(lex);
                lexi = 0;
            }

            // si c'est une balise de fermeture
            if (buf[i + 1] == '/')
            {
                i += 2;
                //tantque c'est pas la fin de la balise charger son nom dan lex
                while (buf[i] != '>')
                    lex[lexi++] = buf[i++];
                lex[lexi] = '\0';

                if (!curr_node)
                {
                    fprintf(stderr, "Dejà dans la racine\n");
                    return FALSE;
                }
                //balise d'ouverture et de fermeture non identiques
                if (strcmp(curr_node->tag, lex))
                {
                    fprintf(stderr, "Tags différents <%s> != </%s>\n", curr_node->tag, lex);
                    return FALSE;
                }
                //fin de traitement revenir à la balise parent pour traiter l'autre fils si existe
                curr_node = curr_node->parent;
                i++;
                continue;
            }

            // pour les commentaires
            if (buf[i + 1] == '!')
            {
                while (buf[i] != ' ' && buf[i] != '>')
                    lex[lexi++] = buf[i++];
                lex[lexi] = '\0';

                // Commentaire
                if (!strcmp(lex, "<!--"))
                {
                    //doit finir par "-->"
                    lex[lexi] = '\0';
                    while (!fini_par(lex, "-->"))
                    {
                        lex[lexi++] = buf[i++];
                        lex[lexi] = '\0';
                    }
                    continue;
                }
            }

            // MAJ balise courante
            curr_node = XMLBalise_new(curr_node);

            // balise de début
            i++;
            if (parse_attrs(buf, &i, lex, &lexi, curr_node) == TAG_ENLIGNE)
            {
                curr_node = curr_node->parent;
                i++;
                continue;
            }

            // mettre nom de la balise
            lex[lexi] = '\0';
            if (!curr_node->tag)
                curr_node->tag = strdup(lex);

            // Reset lexer
            lexi = 0;
            i++;
            continue;
        }
        //lecture normale
        else
        {
            lex[lexi++] = buf[i++];
        }
    }

    return TRUE;
}

void XMLDocument_free(XMLDocument *doc)
{
    XMLBalise_free(doc->root);
}

#endif // LITTLE_XML_H