#include <config.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "#service#.h"
#include <libsocialweb/sw-item.h>
#include <libsocialweb/sw-set.h>
#include <libsocialweb/sw-online.h>
#include <libsocialweb/sw-utils.h>
#include <libsocialweb/sw-web.h>
#include <libsocialweb/sw-debug.h>
#include <libsocialweb-keyfob/sw-keyfob.h>
#include <libsocialweb-keystore/sw-keystore.h>
#include <gconf/gconf-client.h>
#include <rest/oauth-proxy.h>
#include <rest/oauth-proxy-call.h>
#include <rest/rest-xml-parser.h>
#include <libsoup/soup.h>

#include <interfaces/sw-query-ginterface.h>
#include <interfaces/sw-avatar-ginterface.h>
#include <interfaces/sw-status-update-ginterface.h>

#include "#service#-item-view.h"

static void initable_iface_init (gpointer g_iface, gpointer iface_data);
static void query_iface_init (gpointer g_iface, gpointer iface_data);
static void avatar_iface_init (gpointer g_iface, gpointer iface_data);
static void status_update_iface_init (gpointer g_iface, gpointer iface_data);

G_DEFINE_TYPE_WITH_CODE (SwService#Service#,
                         sw_service_#service#,
                         SW_TYPE_SERVICE,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
                                                initable_iface_init)
                         G_IMPLEMENT_INTERFACE (SW_TYPE_QUERY_IFACE,
                                                query_iface_init)
                         G_IMPLEMENT_INTERFACE (SW_TYPE_AVATAR_IFACE,
                                                avatar_iface_init)
                         G_IMPLEMENT_INTERFACE (SW_TYPE_STATUS_UPDATE_IFACE,
                                                status_update_iface_init));

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SW_TYPE_SERVICE_#SERVICE#, SwService#Service#Private))

struct _SwService#Service#Private {
  gboolean inited;
  enum {
    OWN,
    FRIENDS,
    BOTH
  } type;
  gboolean running;
  RestProxy *proxy;
  char *user_id;
  char *image_url;
  /* What else? */
};

static void online_notify (gboolean online, gpointer user_data);
static void credentials_updated (SwService *service);

RestXmlNode *
node_from_call (RestProxyCall *call)
{
  static RestXmlParser *parser = NULL;
  RestXmlNode *root;

  if (call == NULL)
    return NULL;

  if (parser == NULL)
    parser = rest_xml_parser_new ();

  if (!SOUP_STATUS_IS_SUCCESSFUL (rest_proxy_call_get_status_code (call))) {
    g_message ("Error from #Service#: %s (%d)",
               rest_proxy_call_get_status_message (call),
               rest_proxy_call_get_status_code (call));
    return NULL;
  }

  root = rest_xml_parser_parse_from_data (parser,
                                          rest_proxy_call_get_payload (call),
                                          rest_proxy_call_get_payload_length (call));

  if (root == NULL) {
    g_message ("Error from #Service#: %s",
               rest_proxy_call_get_payload (call));
    return NULL;
  }

  return root;
}

static const char **
get_static_caps (SwService *service)
{
  static const char * caps[] = {
    CAN_VERIFY_CREDENTIALS,
    CAN_UPDATE_STATUS,
    CAN_REQUEST_AVATAR,
    CAN_GEOTAG,
    NULL
  };

  return caps;
}

static const char **
get_dynamic_caps (SwService *service)
{
  SwService#Service#Private *priv = GET_PRIVATE (service);
  static const char *no_caps[] = { NULL };
  static const char *configured_caps[] = {
    IS_CONFIGURED,
    NULL
  };
  static const char *invalid_caps[] = {
    IS_CONFIGURED,
    CREDENTIALS_INVALID,
    NULL
  };
  static const char *full_caps[] = {
    IS_CONFIGURED,
    CREDENTIALS_VALID,
    CAN_UPDATE_STATUS,
    CAN_REQUEST_AVATAR,
    NULL
  };
  static const char *full_caps_with_geotag[] = {
    IS_CONFIGURED,
    CREDENTIALS_VALID,
    CAN_UPDATE_STATUS,
    CAN_REQUEST_AVATAR,
    CAN_GEOTAG,
    NULL
  };

  /* Check the conditions and determine which caps array to return */

  /* Just in case we fell through that switch */
  g_warning ("Unhandled credential state %d", priv->credentials);
  return no_caps;
}

static void
start (SwService *service)
{
  SwService#Service# *#service# = (SwService#Service#*)service;

  #service#->priv->running = TRUE;
}

static void
refresh (SwService *service)
{
  /* Update contents */
}

static void
avatar_downloaded_cb (const gchar *uri,
                      gchar       *local_path,
                      gpointer     userdata)
{
  SwService *service = SW_SERVICE (userdata);

  sw_service_emit_avatar_retrieved (service, local_path);
  g_free (local_path);
}

static void
request_avatar (SwService *service)
{
  SwService#Service#Private *priv = GET_PRIVATE (service);

  if (priv->image_url) {
    sw_web_download_image_async (priv->image_url,
                                     avatar_downloaded_cb,
                                     service);
  }
}

static void
online_notify (gboolean online, gpointer user_data)
{
  if (online) {
    /* Whatever have to do while online */
  } else {
    /* Whatever have to do while offline */
  }
}

static void
credentials_updated (SwService *service)
{
  /* If we're online, force a reconnect to fetch new credentials */
  if (sw_is_online ()) {
    online_notify (FALSE, service);
    online_notify (TRUE, service);
  }

  sw_service_emit_user_changed (service);
  sw_service_emit_capabilities_changed ((SwService *)service,
                                        get_dynamic_caps (service));
}

static const char *
sw_service_#service#_get_name (SwService *service)
{
  return "#service#";
}

static void
sw_service_#service#_dispose (GObject *object)
{
  sw_online_remove_notify (online_notify, object);

  /* unref private variables */

  G_OBJECT_CLASS (sw_service_#service#_parent_class)->dispose (object);
}

static void
sw_service_#service#_finalize (GObject *object)
{
  /* Free private variables*/

  G_OBJECT_CLASS (sw_service_#service#_parent_class)->finalize (object);
}

static void
sw_service_#service#_class_init (SwService#Service#Class *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  SwServiceClass *service_class = SW_SERVICE_CLASS (klass);

  g_type_class_add_private (klass, sizeof (SwService#Servuce#Private));

  object_class->dispose = sw_service_#service#_dispose;
  object_class->finalize = sw_service_#service#_finalize;

  service_class->get_name = sw_service_#service#_get_name;
  service_class->start = start;
  service_class->refresh = refresh;
  service_class->get_static_caps = get_static_caps;
  service_class->get_dynamic_caps = get_dynamic_caps;
  service_class->request_avatar = request_avatar;
  service_class->credentials_updated = credentials_updated;
}

static void
sw_service_#service#_init (SwService#Service# *self)
{
  self->priv = GET_PRIVATE (self);
  self->priv->inited = FALSE;
}

/* Initable interface */

static gboolean
sw_service_#service#_initable (GInitable    *initable,
                               GCancellable *cancellable,
                               GError      **error)
{
  /* Initialize the service and return TRUE if everything is OK.
     Otherwise, return FALSE */
}

static void
initable_iface_init (gpointer g_iface, gpointer iface_data)
{
  GInitableIface *klass = (GInitableIface *)g_iface;

  klass->init = sw_service_#service#_initable;
}

/* Query interface */

static void
query_iface_init (gpointer g_iface,
                  gpointer iface_data)
{
  SwQueryIfaceClass *klass = (SwQueryIfaceClass*)g_iface;

  sw_query_iface_implement_open_view (klass,
                                      /*QUERY_OPEN_VIEW_CALLBACK*/);
}

/* Avatar interface */

static void
avatar_iface_init (gpointer g_iface,
                   gpointer iface_data)
{
  SwAvatarIfaceClass *klass = (SwAvatarIfaceClass*)g_iface;

  sw_avatar_iface_implement_request_avatar (klass,
                                            /*AVATAR_REQUEST_CALLBACK*/);
}

static void
status_update_iface_init (gpointer g_iface,
                          gpointer iface_data)
{
  SwStatusUpdateIfaceClass *klass = (SwStatusUpdateIfaceClass*)g_iface;

  sw_status_update_iface_implement_update_status (klass,
                                                  /*STATUS_UPDATE_CALLBACK*/);
}

