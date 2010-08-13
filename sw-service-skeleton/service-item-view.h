#ifndef _SW_#SERVICE#_ITEM_VIEW
#define _SW_#SERVICE#_ITEM_VIEW

#include <glib-object.h>
#include <libsocialweb/sw-item-view.h>

G_BEGIN_DECLS

#define SW_TYPE_#SERVICE#_ITEM_VIEW sw_#service#_item_view_get_type()

#define SW_#SERVICE#_ITEM_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), SW_TYPE_#SERVICE#_ITEM_VIEW, Sw#Service#ItemView))

#define SW_#SERVICE#_ITEM_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), SW_TYPE_#SERVICE#_ITEM_VIEW, Sw#Service#ItemViewClass))

#define SW_IS_#SERVICE#_ITEM_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SW_TYPE_#SERVICE#_ITEM_VIEW))

#define SW_IS_#SERVICE#_ITEM_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), SW_TYPE_#SERVICE#_ITEM_VIEW))

#define SW_#SERVICE#_ITEM_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), SW_TYPE_#SERVICE#_ITEM_VIEW, Sw#Service#ItemViewClass))

typedef struct {
  SwItemView parent;
} Sw#Service#ItemView;

typedef struct {
  SwItemViewClass parent_class;
} Sw#Service#ItemViewClass;

GType sw_#service#_item_view_get_type (void);

G_END_DECLS

#endif /* _SW_#SERVICE#_ITEM_VIEW */

