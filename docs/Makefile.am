EXTRA_DIST = \
	Doxyfile \
	CMakeLists.txt

if WITH_OPENXCOM_DOCS

install-data-am:
	mkdir -p $(DESTDIR)$(docdir)/html/search
	$(INSTALL_DATA) html/*.html $(DESTDIR)$(docdir)/html/
	$(INSTALL_DATA) html/*.png $(DESTDIR)$(docdir)/html/
	$(INSTALL_DATA) html/*.css $(DESTDIR)$(docdir)/html/
	$(INSTALL_DATA) html/*.js $(DESTDIR)$(docdir)/html/
	$(INSTALL_DATA) html/search/*.html $(DESTDIR)$(docdir)/html/search/
	$(INSTALL_DATA) html/search/*.png $(DESTDIR)$(docdir)/html/search/
	$(INSTALL_DATA) html/search/*.css $(DESTDIR)$(docdir)/html/search/
	$(INSTALL_DATA) html/search/*.js $(DESTDIR)$(docdir)/html/search/


uninstall-am:
	-rm -rf $(DESTDIR)$(docdir)/html

all:
	$(DOXYGEN) Doxyfile

else

all:

endif

distclean-local: clean-local

clean-local:
	rm -rf html
