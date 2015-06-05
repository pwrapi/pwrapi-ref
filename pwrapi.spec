Name:		pwrapi
Version:	1.0.1
Release:	1%{?dist}
Summary:	Power API reference implementation

Group:		Development/Libraries
License:	GPLv2+
URL:		powerapi.sandia.gov
Source0:	pwrapi-%{version}.tgz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:	autoconf
BuildRequires:	automake
BuildRequires:	libtool

%description
The Power API reference is a reference implementation of the Power API.  The main
purpose for the creation of the reference was to test the viability of the API
functions during the design of the API specification.  The reference is also being
used to test implementation details such as how to interface with different types
of hardware and communication between agents.  The reference is a work in progress
and only a subset of the functions listed in the specifications have been implemented.

%prep
%setup -q
./autogen.sh


%build
%configure
make %{?_smp_mflags}


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
# remove unpackaged files from the buildroot
rm -f $RPM_BUILD_ROOT%{_libdir}/*.la

%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%{_libdir}/*
/usr/lib/python2.6/site-packages/*
%{_bindir}/*

# devel
%package devel
Summary: Power API devel package
Group: Development/Libraries
Requires:  pwrapi
%description devel
This is a development package of the Power API.
Users who want to implement their own plugins must install this
package.

%files devel
%defattr(-,root,root)
%{_includedir}/*.h
#end devel


%package doc
Summary: Documentation files for %{name}
Group: Development/Libraries
%description doc
Examples and man for Power API.
%files doc
%defattr(-,root,root)
%{_mandir}/*/*
%{_datadir}/doc/*/*/*
%docdir %{_defaultdocdir}

%changelog

