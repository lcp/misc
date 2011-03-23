#include <glib.h>
#include <string.h>
#include <expat.h>

/* XXX Defined in utils */
typedef struct {
	char *sys_vendor;
	char *bios_date;
	char *bios_vendor;
	char *bios_version;
	char *product_name;
	char *product_version;
} DmiInfo;
/* XXX */

enum
{
	OPT_NONE,
	OPT_KEY_CONTROL,
	OPT_MASTER_KEY,
	OPT_FORCE_SYNC,
	OPT_UNKNOWN,
};

enum
{
	OPT_TYPE_NONE,
	OPT_TYPE_BOOLEAN,
	OPT_TYPE_UNKNOWN,
};

enum
{
	OPER_STRING,
	OPER_STRING_OUTOF,
	OPER_CONTAINS,
	OPER_CONTAINS_NCASE,
	OPER_CONTAINS_NOT,
	OPER_CONTAINS_OUTOF,
	OPER_PREFIX,
	OPER_PREFIX_NCASE,
	OPER_PREFIX_OUTOF,
	OPER_SUFFIX,
	OPER_SUFFIX_NCASE,
	OPER_UNKNOWN,
};

typedef struct {
	DmiInfo *hardware_info;

	int xml_depth;
	int xml_bound;

	int opt;
	int opt_type;

	/* result */
	gboolean key_control;
	gboolean master_key;
	gboolean force_sync;
} ParseInfo;

#define URF_CONFIG_GET_PRIVATE(obj) (obj)->priv

typedef struct{
	char *user;
	gboolean key_control;
	gboolean master_key;
	gboolean force_sync;
} UrfConfigPrivate;

typedef struct {
	UrfConfigPrivate *priv;
} UrfConfig;

/* XXX defined in utils */
DmiInfo *
get_dmi_info ()
{
	DmiInfo *hardware_info;

	hardware_info = g_new0 (DmiInfo, 1);

	hardware_info->sys_vendor = "LENOVO";
	hardware_info->bios_date = "03/17/2009";
	hardware_info->bios_vendor = "LENOVO";
	hardware_info->bios_version = "xxxxxxx";
	hardware_info->product_name = "74xafaf";
	hardware_info->product_version = "ThinkPad X200";
}
/* XXX */

static int
get_option (const char *option)
{
	if (g_strcmp0 (option, "key_control") == 0)
		return OPT_KEY_CONTROL;
	else if (g_strcmp0 (option, "master_key") == 0)
		return OPT_MASTER_KEY;
	else if (g_strcmp0 (option, "force_sync") == 0)
		return OPT_FORCE_SYNC;
	return OPT_UNKNOWN;
}

static int
get_option_type (const char *type)
{
	if (g_strcmp0 (type, "bool") == 0)
		return OPT_TYPE_BOOLEAN;
	return OPT_TYPE_UNKNOWN;
}

static char *
get_match_key (DmiInfo    *hardware_info,
	       const char *key)
{
	if (hardware_info == NULL)
		return NULL;

	if (g_strcmp0 (key, "sys_vendor") == 0)
		return hardware_info->sys_vendor;
	else if (g_strcmp0 (key, "bios_date") == 0)
		return hardware_info->bios_date;
	else if (g_strcmp0 (key, "bios_vendor") == 0)
		return hardware_info->bios_vendor;
	else if (g_strcmp0 (key, "bios_version") == 0)
		return hardware_info->bios_version;
	else if (g_strcmp0 (key, "product_name") == 0)
		return hardware_info->product_name;
	else if (g_strcmp0 (key, "product_version") == 0)
		return hardware_info->product_version;
	return NULL;
}

static int
get_operator (const char *operator)
{
	if (g_strcmp0 (operator, "string") == 0)
		return OPER_STRING;
	else if (g_strcmp0 (operator, "string_outof") == 0)
		return OPER_STRING_OUTOF;
	else if (g_strcmp0 (operator, "contains") == 0)
		return OPER_CONTAINS;
	else if (g_strcmp0 (operator, "contains_ncase") == 0)
		return OPER_CONTAINS_NCASE;
	else if (g_strcmp0 (operator, "contains_not") == 0)
		return OPER_CONTAINS_NOT;
	else if (g_strcmp0 (operator, "contains_outof") == 0)
		return OPER_CONTAINS_OUTOF;
	else if (g_strcmp0 (operator, "prefix") == 0)
		return OPER_PREFIX;
	else if (g_strcmp0 (operator, "prefix_ncase") == 0)
		return OPER_PREFIX_NCASE;
	else if (g_strcmp0 (operator, "prefix_outof") == 0)
		return OPER_PREFIX_OUTOF;
	else if (g_strcmp0 (operator, "suffix") == 0)
		return OPER_SUFFIX;
	else if (g_strcmp0 (operator, "suffix_ncase") == 0)
		return OPER_SUFFIX_NCASE;
	return OPER_UNKNOWN;
}

static gboolean
match_rule (const char *str1,
	    int operator,
	    const char *str2)
{
	gboolean match = FALSE;

	switch (operator) {
	case OPER_STRING:
		if (g_strcmp0 (str1, str2) == 0)
			match = TRUE;
		break;
	/* TODO complete other operators */
	case OPER_STRING_OUTOF:
	case OPER_CONTAINS:
	case OPER_CONTAINS_NCASE:
	case OPER_CONTAINS_NOT:
	case OPER_CONTAINS_OUTOF:
	case OPER_PREFIX:
	case OPER_PREFIX_NCASE:
	case OPER_PREFIX_OUTOF:
	case OPER_SUFFIX:
	case OPER_SUFFIX_NCASE:
	case OPER_UNKNOWN:
		match = FALSE;
		break;
	default:
		break;
	}

	return match;
}

static void
parse_xml_cdata_handler (void       *data,
			 const char *cdata,
			 int         len)
{
	ParseInfo *info = (ParseInfo *)data;
	char *str = g_strndup (cdata, len);

	if (info->opt == OPT_NONE ||
	    info->opt == OPT_UNKNOWN) {
		return;
	}

	switch (info->opt) {
	case OPT_KEY_CONTROL:
		if (g_ascii_strcasecmp (str, "TRUE") == 0)
			info->key_control = TRUE;
		else if (g_ascii_strcasecmp (str, "FALSE") == 0)
			info->key_control = FALSE;
g_debug ("** assign key_control to %d", info->key_control);
		break;
	case OPT_MASTER_KEY:
		if (g_ascii_strcasecmp (str, "TRUE") == 0)
			info->master_key = TRUE;
		else if (g_ascii_strcasecmp (str, "FALSE") == 0)
			info->master_key = FALSE;
g_debug ("** assign master_key to %d", info->master_key);
		break;
	case OPT_FORCE_SYNC:
		if (g_ascii_strcasecmp (str, "TRUE") == 0)
			info->force_sync = TRUE;
		else if (g_ascii_strcasecmp (str, "FALSE") == 0)
			info->force_sync = FALSE;
g_debug ("** assign force_sync to %d", info->force_sync);
		break;
	default:
		break;
	}
}

static void
parse_xml_start_element (void       *data,
			 const char *name,
			 const char **atts)
{
	ParseInfo *info = (ParseInfo *)data;
	const char *key = NULL;
	const char *type = NULL;
	const char *match_key = NULL;
	const char *operator = NULL;
	const char *match_body = NULL;
	int i;

	info->xml_depth++;

	if (info->xml_depth > info->xml_bound)
		return;
	else if (info->xml_depth < info->xml_bound)
		info->xml_bound = info->xml_depth + 1;

	info->opt = OPT_NONE;
	info->opt_type = OPT_TYPE_NONE;
/**
 * TODO parse the file
 *  if a rule is matched, go on. Otherwise, return immediately.
 **/
g_debug ("== Start == tag = %s , depth = %d, bound = %d", name, info->xml_depth, info->xml_bound);
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
				operator = atts[i];
				match_body = atts[i+1];
				i++;
			}
		}

		match_key = get_match_key (info->hardware_info, key);
		if (!match_rule (match_key, get_operator (operator), match_body))
			return;
g_debug ("  %s, %s, %s", key, operator, match_body);
	} else if (g_strcmp0 (name, "option") == 0) {
		for (i = 0; atts[i]; i++) {
			if (g_strcmp0 (atts[i], "key") == 0) {
				if (!atts[i+1])
					continue;
				key = atts[i+1];
				i++;
			} else if (g_strcmp0 (atts[i], "type") == 0) {
				if (!atts[i+1])
					continue;
				type = atts[i+1];
				i++;
			}
		}

		info->opt = get_option (key);
		info->opt_type = get_option_type (type);
g_debug ("  option: %s type %s", key, type);
	}

/* STOP HERE */

	info->xml_bound++;
}

static void
parse_xml_end_element (void       *data,
		       const char *name)
{
	ParseInfo *info = (ParseInfo *)data;

	if (info->xml_bound > info->xml_depth)
		info->xml_bound = info->xml_depth;

g_debug ("== END == tag = %s , depth = %d, bound = %d", name, info->xml_depth, info->xml_bound);

	info->opt = OPT_NONE;
	info->opt_type = OPT_TYPE_NONE;

	info->xml_depth--;
}

void
urf_config_profile_xml_parse (UrfConfig  *config,
			      const char *filename)
{
	UrfConfigPrivate *priv = config->priv;
	DmiInfo *hardware_info = NULL;
	ParseInfo *info;
	XML_Parser parser;
	char *content;
	gsize length;
	int len;

	if (!g_file_get_contents (filename, &content, &length, NULL)) {
		g_debug ("Failed to read %s\n", filename);
		return;
	}

	hardware_info = get_dmi_info ();
	if (hardware_info == NULL)
		return;

	info = g_new0 (ParseInfo, 1);
	info->hardware_info = hardware_info;
	info->key_control = priv->key_control;
	info->master_key = priv->master_key;
	info->force_sync = priv->force_sync;
	info->xml_depth = 0;
	info->xml_bound = 1;
	info->opt = OPT_NONE;
	info->opt_type = OPT_TYPE_NONE;

	parser = XML_ParserCreate (NULL);
	XML_SetUserData (parser, (void *)info);
	XML_SetElementHandler (parser,
			       parse_xml_start_element,
			       parse_xml_end_element);
	XML_SetCharacterDataHandler (parser,
				     parse_xml_cdata_handler);
	len = strlen (content);

	if (XML_Parse (parser, content, len, 1) == XML_STATUS_ERROR) {
		g_warning ("Settings File Parse error: %s\n", filename);
		XML_ParserFree (parser);
		g_free (info);
		return;
	}

	XML_ParserFree (parser);

	priv->key_control = info->key_control;
	priv->master_key = info->master_key;
	priv->force_sync = info->force_sync;

	g_free (info);
	return;
}

int
main (int argc, char *argv[])
{
	char *filename;
	gboolean ret;
	UrfConfig *config;
	UrfConfigPrivate *priv;

	config = g_new0 (UrfConfig, 1);
	config->priv = g_new0 (UrfConfigPrivate, 1);
	priv = URF_CONFIG_GET_PRIVATE (config);

	priv->user = "root";
	priv->key_control = TRUE;
	priv->master_key = FALSE;
	priv->force_sync = FALSE;

	if (argc < 2) {
		return -1;
	}

	filename = argv[1];

	urf_config_profile_xml_parse (config, filename);

	g_debug ("key_control = %d, master_key = %d, force_sync = %d",
		 priv->key_control,
		 priv->master_key,
		 priv->force_sync);

	return 0;
}
