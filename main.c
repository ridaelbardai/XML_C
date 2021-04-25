#include "lxml.h"

int main()
{
    XMLDocument doc;

    if (XMLDocument_charger(&doc, "test.xml"))
    {

        XMLBalise *str = XMLBalise_fils_pos(doc.root, 0);
        printf("Struct: %s\n", XMLBalise_attr_val(str, "name"));

        XMLBaliseListe *fields = XMLBalise_fils(str, "field");
        for (int i = 0; i < fields->taille; i++)
        {
            XMLBalise *field = XMLBaliseListe_pos(fields, i);
            printf(" %s (%s)\n", XMLBalise_attr_val(field, "name"), XMLBalise_attr_val(field, "type"));
        }

        XMLDocument_free(&doc);
    }

    return 0;
}

