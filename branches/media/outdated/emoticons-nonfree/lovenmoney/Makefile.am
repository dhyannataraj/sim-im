nodist_icons_DATA = lovenmoney.jisp

iconsdir=$(kde_datadir)/sim/icons/

all: lovenmoney.jisp

lovenmoney.jisp: $(srcdir)/icondef.xml $(srcdir)/*.png
	$(ZIP) $(ZIP_FLAGS) $@ $^
