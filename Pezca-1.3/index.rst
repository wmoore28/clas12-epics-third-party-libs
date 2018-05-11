==================
EPICS Pezca Module
==================

.. This text is RST (ReStructured Text), 
   see also http://docutils.sourceforge.net/rst.html

:Author:
    Goetz Pfeiffer (Goetz.Pfeiffer@helmholtz-berlin.de)

This page is the home of the EPICS Pezca module, which provides channel access
support for the perl scripting language. Pezca is based on the easy channel
access (ezca) library which is part of the EPICS extensions. Pezca implements
interfaces to most of the functions of ezca. 

This site contains links for the source code of the software and information on
how to install and use it on your system.  Please email any comments and bug
reports to Goetz Pfeiffer who is responsible for coordinating development and
releases.

Supported Hardware
------------------

The module supports the following platforms / operating systems: 

* HP-UX HP-Workstations with the HP-UX operating system

* Solaris Sun-Workstations with the Solaris operating system

* Linux PC's with the Linux operating system

* Windows NT, Windows XP, Windows 7, Windows 8

Software Distribution
---------------------

`download Pezca (tarball) <dist>`_

In case you dont' have ezca, the module Pezca is based on:

`download ezca (tarball) <ezca>`_

Pezca is compatible with both versions of ezca you find at the link above.

Repository access
-----------------

You can also access our mercurial repository. 

`browse Pezca repository <http://www-csr.bessy.de/cgi-bin/hgweb.cgi/Pezca>`_.

If you plan to extend or improve Pezca I would recommend that you get a clone
of our repository with this command::

  hg clone http://www-csr.bessy.de/cgi-bin/hgweb.cgi/Pezca

You can then commit changes in your own repository copy. 

If you plan to share these changes you can create a mercurial 
`bundle <http://mercurial.selenic.com/wiki/Bundle>`_ and send it to my e-mail
address.

Required Modules
----------------

Pezca requires the perl-scripting language to be installed on your system.
Perl sources and binaries can be downloaded at the Comprehensive Perl Archive
Network CPAN.  

Your system should also support the compilation of perl modules, which is
usually no problem on a UNIX like systems where perl is installed.

You should have installed the EPICS base.

You should have installed the easy channel access module (ezca), this can also
be downloaded here: `download ezca (tarball) <ezca>`_

Build and install
-----------------

After obtaining a copy of the distribution, it must be installed and built for
use at your site. If you have a UNIX-like operating system, you have to decide
first wether you want a local or global installation. For Windows NT, you
should always select a global installation.

For a global installation you usually have to be "root" (or "administrator" on
Windows NT) on your system. All users on your system can then use Pezca.

For a local installation you do not need to be "root" and your installation
goes into an arbitrary directory. If other users want to use Pezca, you have to
set access-permissions on files and directories accordingly.

You also have to know in which directory your local EPICS base and ezca library
are placed. You either have to put these paths in file "platform.pm" at the
definitions of variables "%epics_base" and "%epics_extensions" or you have to
set the environment variables "EPICS_BASE" and "EPICS_EXTENSIONS" when you call
"perl Makefile.PL" (see below).

Global Installation
+++++++++++++++++++

Select a directory where the distribution can be unpacked.  Unpack the
distribution tar file. This produces a "Pezca-<version>" directory named after
the release number.  Now execute the following commands::

  perl Makefile.PL
  make
  make test
  make install

Local Installation
++++++++++++++++++

Choose a directory where the distribution can be unpacked.  Unpack the
distribution tar file. This produces a "Pezca-<version>" directory named after
the release number.  Select or create a directory where your locally installed
perl modules go: "<perldir>" Now execute the following commands::

  perl Makefile.PL PREFIX=<perldir>
  make
  make test
  make install

Using the Module
----------------

The usage of the installed perl module depends on your type of installation.

Global Installation
+++++++++++++++++++

Add the following near the top of your perl script::

  use Pezca;

Local Installation
++++++++++++++++++

Your can either define environment variables (the following are the shell
commands)::

  export PATH=${PATH}:<perldir>/bin
  export PERL5LIB=<perldir>/lib/site_perl 
  export MANPATH=${MANPATH}:<perldir>/man

and then add near the top of your script::

  use Pezca;

or you use the following statements in your script which are independent on
environment variables::

  use lib "<perldir>/lib/site_perl";
  use Pezca;

Documentation
-------------

The following documentation is available: 

README
  part of the distribution tar-file

Changes
  Release notes, part of the distribution tar-file

Pezca.3
  The Pezca man-page.

Pezca.html
  Here is the current `html documentation <Pezca.html>`_

In Use
------

This software was developed by Goetz Pfeiffer at HZB for BESSY II, a
synchrotron radiation facility in Berlin, Germany.
 
It is used at the following EPICS sites: 

HZB Undulator Control
  Used for for complex movement and reference procedures for maintainance
  purposes of the HZB undulators. 

HZB Beamline Control
  Used for a simple protocoll-less Channel Access Gateway application. This
  enables reading and writing to channels via a "raw socket" connection, no
  EPICS software is needed on the client site. This is basically the tclient
  application that comes as an example with the Pezca distribution.
