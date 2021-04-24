#include "lxml.h"

int main()
{
    XMLDocument doc;

    if (XMLDocument_load(&doc, "test.xml"))
    {

        XMLNode* more = XMLNode_child(XMLNode_child(XMLNode_child(doc.root, 0), 0), 0);
        printf("%s: %s\n", more->tag, more->inner_text);
       

        XMLNode* another = XMLNode_child(XMLNode_child(doc.root, 0),1);
        printf("%s: %s \n", another->tag, another->inner_text);
   

        XMLDocument_free(&doc);
    }

    return 0;
}

// <root><inner><more>hada more</more></inner><another>hada other </another></root>
