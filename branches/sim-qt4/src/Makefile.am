bin_PROGRAMS = sim 

sim_SOURCES = sim.cpp sim-resource.cpp

sim_LDADD = ../lib/libsim.la

sim-resource.cpp: $(RCC) sim.qrc
	$(RCC) -o sim-resource.cpp -name sim $(srcdir)/sim.qrc

INCLUDES=-I../lib -Iapi $(all_includes) 

METASOURCES = AUTO

sim_LDFLAGS = -L../lib $(QT_LDFLAGS) $(KDE_RPATH) $(SSL_LDFLAGS) $(all_libraries)
