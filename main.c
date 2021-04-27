#include "lxml.h"

int main()
{
    XMLDocument doc;

    if (XMLDocument_charger(&doc, "test.xml"))
    {

        XMLBalise *str = XMLBalise_fils_pos(doc.root, 0);
        printf("Struct: %s\n", XMLBalise_attr_val(str, "nom"));

        XMLBaliseListe *champs = XMLBalise_fils(str, "champ");
        for (int i = 0; i < champs->taille; i++)
        {
            XMLBalise *champ = XMLBaliseListe_pos(champs, i);
            printf(" %s (%s)\n", XMLBalise_attr_val(champ, "nom"), XMLBalise_attr_val(champ, "type"));
        }

        XMLDocument_free(&doc);
    }

    return 0;
}

