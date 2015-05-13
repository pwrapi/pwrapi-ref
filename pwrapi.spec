Name:		pwrapi
Version:	1.0
Release:	1%{?dist}
Summary:	Power API prototype

Group:		Development/Libraries
License:	GPLv2+
URL:		powerapi.sandia.gov
Source0:	pwrapi-%{version}.tgz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:	autoconf
BuildRequires:	automake
BuildRequires:	libtool

%description
The Power API prototype is a prototype implementation of the Power API.  The main
purpose for the creation of the prototype was to test the viability of the API
functions during the design of the API specification.  The prototype is also being
used to test implementation details such as how to interface with different types
of hardware and communication between agents.  The prototype is a work in progress
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


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc



%changelog

