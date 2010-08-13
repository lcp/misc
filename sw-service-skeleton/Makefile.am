services_LTLIBRARIES = lib#service#.la
lib#service#_la_SOURCES = module.c #service#.c #service#.h \
			#service#-item-view.h #service#-item-view.c
lib#service#_la_CFLAGS = -I$(top_srcdir) $(REST_CFLAGS) $(GCONF_CFLAGS) $(DBUS_GLIB_CFLAGS) -DG_LOG_DOMAIN=\"#Service#\"
lib#service#_la_LIBADD = $(top_builddir)/libsocialweb/libsocialweb.la $(top_builddir)/libsocialweb-keystore/libsocialweb-keystore.la $(top_builddir)/libsocialweb-keyfob/libsocialweb-keyfob.la $(REST_LIBS) $(GCONF_LIBS) $(DBUS_GLIB_LIBS)
lib#service#_la_LDFLAGS = -module -avoid-version

dist_servicesdata_DATA = #service#.png

servicesdata_DATA = #service#.keys
CLEANFILES = #service#.keys
EXTRA_DIST = #serivce#.keys.in
@INTLTOOL_SOCIALWEB_KEYS@
