#!/usr/bin/perl

use Data::Dumper;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.

use platform
  qw(pl_includes pl_defines pl_dir pl_libdirs
     pl_libs pl_objects
     pl_dump);

# *****************************************************************

my $debug=0;

my %extra_info= 
  {
   'meta-spec' => { version => 2 },
   'resources' => 
     {
       'repository' => 
         {
           'type' => 'hg',
           'url'  => 'http://www-csr.bessy.de/cgi-bin/hgweb.cgi/Pezca',
           'web'  => 'http://www-csr.bessy.de/control/SoftDist/Pezca',
         }
     }
  };

my %options= 
  (
    'NAME'         => 'Pezca',
    'AUTHOR'       => ['Goetz Pfeiffer <Goetz.Pfeiffer@helmholtz-berlin.de>'],
    'VERSION_FROM' => 'Pezca.pm', 	# finds $VERSION
    'INC'    	   => pl_includes(),
    'DEFINE' 	   => pl_defines(),
    'LIBS'         => join(" ",pl_libdirs(),pl_libs()),
    'OBJECT' 	   => pl_objects(),
    'PM'           => { 'Pezca.pm' => '$(INST_LIBDIR)/Pezca.pm' },
    'DIR' => [],
    'META_MERGE'   => \%extra_info
  );
  
my $dirs= pl_dir();
$options{'DIR'}= $dirs if (defined $dirs);

if ($debug)
  { pl_dump(\%options);
    die "\n";
  }

WriteMakefile(%options);
