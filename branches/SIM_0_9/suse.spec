# spec created by Crissi
%define suse_release %(suse_release="`cat /etc/SuSE-release | tail -n1 | awk {'print $3'}`" ;; echo "$suse_release")
%define release %(release="`echo "%{suse_release} * 10" | bc 2>/dev/null`" ; if test $? != 0 ; then release="" ; fi ; echo "$release")

Name:           sim
Version:        0.9.0
Release:	1.suse%{release}
Vendor:		Vladimir Shutoff <shutoff@mail.ru>
Packager:	Christoph Thielecke <crissi99@gmx.de>
Summary:	SIM - Simple Instant Messenger
Copyright:	GPL
Group:		X11/KDE/Network
URL: 		http://sim-icq.sourceforge.net/
Source0: 	http://osdn.dl.sourceforge.net/sourceforge/sim-icq/%{name}-%{version}.tar.gz
# Source1: myfind-requires.sh
# %define __find_requires %{SOURCE1}
BuildRoot:	%{_tmppath}/sim-buildroot
Distribution:	SuSE Linux %{suse_release}

# neededforbuild  kde3-devel-packages
# usedforbuild    aaa_base aaa_dir aaa_version arts arts-devel autoconf automake base bash bindutil binutils bison bzip compat cpio cpp cracklib cyrus-sasl db devs diffutils docbook-dsssl-stylesheets docbook_3 e2fsprogs fam file fileutils findutils flex freetype2 freetype2-devel gawk gcc gcc-c++ gdbm gdbm-devel gettext glibc glibc-devel glibc-locale gpm grep groff gzip iso_ent jade_dsl kbd kdelibs3 kdelibs3-devel less libgcc libjpeg liblcms libmng libmng-devel libpng libpng-devel libstdc++ libstdc++-devel libtiff libtool libxcrypt libxml2 libxml2-devel libxslt libxslt-devel libz m4 make man mesa mesa-devel mesaglu mesaglu-devel mesaglut mesaglut-devel mesasoft mktemp modutils ncurses ncurses-devel net-tools netcfg openssl openssl-devel pam pam-devel pam-modules patch perl ps qt3 qt3-devel rcs readline rpm sed sendmail sh-utils shadow sp sp-devel strace syslogd sysvinit tar texinfo textutils timezone unzip util-linux vim xdevel xf86 xshared

%description
SIM - Simple Instant Messenger

SIM (Simple Instant Messenger) is an unpretentious
open-source ICQ client which supports many of the
features of Version 8 of the ICQ protocol (ICQ 2001).

SIM has a lot of features, many of them are listed
at: http://sim-icq.sourceforge.net/

%prep
export KDEDIR=/opt/kde3
export QTDIR=/usr/lib/qt3

%setup -q
make -f admin/Makefile.common
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=/opt/kde3 $LOCALFLAGS

%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j $numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.sim
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.sim
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.sim

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/sim*
rm -rf ../file.list.sim

%files -f ../file.list.sim
