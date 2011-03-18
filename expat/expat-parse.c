#include <glib.h>
#include <string.h>
#include <expat.h>

typedef struct {
	int depth;
	int bound;
} Data;

void
expat_xml_start_element (void *data,
			 const char *name,
			 const char **atts)
{
	Data *ptr = (Data *)data;
	int i;

/**
 * Start - Take care the depth and bound
 **/
	ptr->depth++;

	if (ptr->depth > ptr->bound)
		return;
	else if (ptr->depth < ptr->bound)
		ptr->bound = ptr->depth + 1;
/**
 * End - Take care the depth and bound
 **/

	g_print ("== Start ==\n");
	g_print ("tag = %s , depth = %d, bound = %d\n", name, ptr->depth, ptr->bound);
	g_print ("attribute:\n");
	for (i = 0; atts[i]; i++) {
		g_print ("atts[%d] = %s\n", i, atts[i]);
		if (i == 3 && (g_strcmp0 (atts[i], "button")  == 0)) {
			/* Stop in this level */
			g_print ("\nmatched, bound = %d\n", ptr->bound);
			return;
		}
	}

/**
 * Start - Take care the depth and bound
 **/
	ptr->bound++;
/**
 * End - Take care the depth and bound
 **/
}

void
expat_xml_end_element (void *data,
		       const char *name)
{
	Data *ptr = (Data *)data;

/**
 * Start - Take care the depth and bound
 **/
	if (ptr->bound > ptr->depth)
		ptr->bound = ptr->depth;
/**
 * End - Take care the depth and bound
 **/

	g_print ("== End ==\n");
	g_print ("tag = %s , depth = %d, bound = %d\n", name, ptr->depth, ptr->bound);

/**
 * Start - Take care the depth and bound
 **/
	ptr->depth--;
/**
 * End - Take care the depth and bound
 **/
}

int
expat_xml_parse (const char *content)
{
	XML_Parser parser;
	int len;
	Data data;

	data.depth = 0;
	data.bound = data.depth + 1;

	parser = XML_ParserCreate (NULL);
	XML_SetUserData (parser, (void *)&data);
	XML_SetElementHandler (parser,
			       expat_xml_start_element,
			       expat_xml_end_element);
	len = strlen (content);

	if (XML_Parse (parser, content, len, 1) == XML_STATUS_ERROR) {
		g_print ("Parse error\n");
		return -1;
	}

	XML_ParserFree (parser);
}

int
main (int argc, char *argv[])
{
	char *filename;
	char *content;
	gsize length;
	gboolean ret;

	if (argc < 2) {
		return -1;
	}

	filename = argv[1];

	if (!g_file_get_contents (filename, &content, &length, NULL)) {
		g_print ("Cannot open %s\n", filename);
		return -1;
	}

	expat_xml_parse (content);

	return 0;
}
