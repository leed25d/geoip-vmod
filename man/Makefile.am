#

dist_man_MANS = vmod_geoip.3
MAINTAINERCLEANFILES = $(dist_man_MANS)

vmod_geoip.3: vmod_geoip.rst
if HAVE_RST2MAN
	${RST2MAN} vmod_geoip.rst $@
else
	@echo "========================================"
	@echo "You need rst2man installed to make dist"
	@echo "========================================"
	@false
endif
