#ifndef _SW_SERVICE_#SERVICE#
#define _SW_SERVICE_#SERVICE#

#include <libsocialweb/sw-service.h>

G_BEGIN_DECLS

#define SW_TYPE_SERVICE_#SERVICE# sw_service_#service#_get_type()

#define SW_SERVICE_#SERVICE#(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), SW_TYPE_SERVICE_#SERVICE#, SwService#Service#))

#define SW_SERVICE_#SERVICE#_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), SW_TYPE_SERVICE_#SERVICE#, SwService#Service#Class))

#define SW_IS_SERVICE_#SERVICE#(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SW_TYPE_SERVICE_#SERVICE#))

#define SW_IS_SERVICE_#SERVICE#_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), SW_TYPE_SERVICE_#SERVICE#))

#define SW_SERVICE_#SERVICE#_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), SW_TYPE_SERVICE_#SERVICE#, SwService#Service#Class))

typedef struct _SwService#Service#Private SwService#Service#Private;

typedef struct {
  SwService parent;
  SwService#Service#Private *priv;
} SwService#Service#;

typedef struct {
  SwServiceClass parent_class;
} SwService#Service#Class;

GType sw_service_#service#_get_type (void);

G_END_DECLS

#endif /* _SW_SERVICE_#SERVICE# */
