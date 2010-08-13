#include <libsocialweb/sw-module.h>
#include "#service#.h"

const gchar *
sw_module_get_name (void)
{
  return "#service#";
}

const GType
sw_module_get_type (void)
{
  return SW_TYPE_SERVICE_#SERVICE#;
}
