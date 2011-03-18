#include <glib.h>
#include <string.h>
#include <expat.h>

typedef struct {
	char *sys_vendor;
	char *bios_date;
	char *bios_vendor;
	char *bios_version;
	char *product_name;
	char *product_version;
} DmiInfo;

#define URF_CONFIG_GET_PRIVATE(obj) (obj)->priv

typedef struct{
	char *user;
	gboolean key_control;
	gboolean master_key;
	gboolean force_sync;
	DmiInfo hardware_info;
	int xml_depth;
	int xml_bound;
} UrfConfigPrivate;

typedef struct {
	UrfConfigPrivate *priv;
} UrfConfig;

static void
urf_config_xml_start_element (void       *data,
			      const char *name,
			      const char **atts)
{
	UrfConfig *config = (UrfConfig *)data;
	UrfConfigPrivate *priv = URF_CONFIG_GET_PRIVATE (config);
	const char *key;
	const char *operation;
	const char *match_body;
	int i;

	priv->xml_depth++;

	if (priv->xml_depth > priv->xml_bound)
		return;
	else if (priv->xml_depth < priv->xml_bound)
		priv->xml_bound = priv->xml_depth + 1;

/**
 * TODO parse the file
 *  if a rule is matched, go on. Otherwise, return immediately.
 **/
g_debug ("== Start ==");
g_debug ("tag = %s , depth = %d, bound = %d", name, priv->xml_depth, priv->xml_bound);
g_debug ("attribute:");

	if (g_strcmp0 (name, "match") == 0) {
		for (i = 0; atts[i]; i++) {
			if (g_strcmp0 (atts[i], "key") == 0) {
				if (!atts[i+1])
					continue;
				key = atts[i+1];
				i++;
			} else if (g_strcmp0 (atts[i], "string") == 0) {
				if (!atts[i+1])
					continue;
				operation = atts[i];
				match_body = atts[i+1];
				i++;
			}
		}
		/*TODO parse match rules */
		g_debug ("  %s, %s, %s", key, operation, match_body);

	} else if (g_strcmp0 (name, "option") == 0) {
		/*TODO parse option rules */
		g_debug ("  option");

	}

/* STOP HERE */

	priv->xml_bound++;
}

void
urf_config_xml_end_element (void       *data,
			    const char *name)
{
	UrfConfig *config = (UrfConfig *)data;
	UrfConfigPrivate *priv = URF_CONFIG_GET_PRIVATE (config);

	if (priv->xml_bound > priv->xml_depth)
		priv->xml_bound = priv->xml_depth;

/* TODO basically nothing to do except debug info */
	g_debug ("== End ==");
	g_debug ("tag = %s , depth = %d, bound = %d", name, priv->xml_depth, priv->xml_bound);
/* STOP HERE */

	priv->xml_depth--;
}

void
urf_config_settings_xml_parse (UrfConfig  *config,
			       const char *filename)
{
	UrfConfigPrivate *priv = URF_CONFIG_GET_PRIVATE (config);
	char *content;
	gsize length;
	XML_Parser parser;
	int len;

	if (!g_file_get_contents (filename, &content, &length, NULL)) {
		g_debug ("Failed to read %s\n", filename);
		return;
	}

	priv->xml_depth = 0;
	priv->xml_bound = priv->xml_depth + 1;

	parser = XML_ParserCreate (NULL);
	XML_SetUserData (parser, (void *)config);
	XML_SetElementHandler (parser,
			       urf_config_xml_start_element,
			       urf_config_xml_end_element);
	len = strlen (content);

	if (XML_Parse (parser, content, len, 1) == XML_STATUS_ERROR) {
		g_warning ("Settings File Parse error: %s\n", filename);
		return;
	}

	XML_ParserFree (parser);
}

int
main (int argc, char *argv[])
{
	char *filename;
	gboolean ret;
	UrfConfig *config;
	UrfConfigPrivate *priv;

	config = g_new0 (UrfConfig, 1);
	config->priv = g_new (UrfConfigPrivate, 1);
	priv = URF_CONFIG_GET_PRIVATE (config);

	priv->user = "root";
	priv->key_control = TRUE;
	priv->master_key = FALSE;
	priv->force_sync = FALSE;
	priv->hardware_info.sys_vendor = "LENOVO";
	priv->hardware_info.bios_date = "03/17/2009";
	priv->hardware_info.bios_vendor = "LENOVO";
	priv->hardware_info.bios_version = "xxxxxxx";
	priv->hardware_info.product_name = "74xafaf";
	priv->hardware_info.product_version = "ThinkPad X200";

	if (argc < 2) {
		return -1;
	}

	filename = argv[1];

	urf_config_settings_xml_parse (config, filename);

	return 0;
}
