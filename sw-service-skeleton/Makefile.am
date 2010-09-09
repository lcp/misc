services_LTLIBRARIES = lib#service#.la
lib#service#_la_SOURCES = module.c #service#.c #service#.h \
			#service#-item-view.h #service#-item-view.c
lib#service#_la_CFLAGS = $(LIBSOCIWEB_MODULE_CFLAGS) $(LIBSOCIWEB_KEYFOB_CFLAGS) $(LIBSOCIWEB_KEYSTORE_CFLAGS) $(REST_CFLAGS) $(GCONF_CFLAGS) $(DBUS_GLIB_CFLAGS) -DG_LOG_DOMAIN=\"#Service#\"
lib#service#_la_LIBADD = $(LIBSOCIWEB_MODULE_LIBS) $(LIBSOCIWEB_KEYFOB_LIBS) $(LIBSOCIWEB_KEYSTORE_LIBS) $(REST_LIBS) $(GCONF_LIBS) $(DBUS_GLIB_LIBS)
lib#service#_la_LDFLAGS = -module -avoid-version

dist_servicesdata_DATA = #service#.png

servicesdata_DATA = #service#.keys
CLEANFILES = #service#.keys
EXTRA_DIST = #service#.keys.in
@INTLTOOL_SOCIALWEB_KEYS@
