#!/usr/bin/perl

#-----------------------------------------------------------------------
# Submit jobs to generate requested samples for the NuINT09 `Confronting 
# theory, models & data' session organized by S.Dytman
#
# Syntax:
#   perl submit-nuint09_jobs.pl <options>
#
# Options:
#  --run           : Comma separated list of run numbers
#  --version       : GENIE version number
# [--production]   :
# [--cycle]        :
# [--use-valgrind] :
#
# Examples:
#  perl submit-nuint09_jobs.pl --production nuint09 --cycle 01 --version v2.5.1 --run 1001
#  perl submit-nuint09_jobs.pl --production nuint09 --cycle 01 --version v2.5.1 --run all
#
# Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
# STFC, Rutherford Appleton Lab
#----------------------------------------------------------------------
#
# SAMPLES:
#......................................................................
#  run   | nev  |  init state      | energy  | processes
#  nu.   |      |                  | (GeV)   | enabled
#......................................................................
#
# 1001   | 500k | numu    + C12    | 0.5     | COH-CC
# 1002   | 500k | numu    + C12    | 1.0     | COH-CC
# 1003   | 500k | numu    + C12    | 1.5     | COH-CC
# 1011   | 500k | numu    + C12    | 0.5     | COH-NC
# 1012   | 500k | numu    + C12    | 1.0     | COH-NC
# 1013   | 500k | numu    + C12    | 1.5     | COH-NC
# 2001   | 500k | numu    + C12    | 0.5     | QEL-CC
# 2002   | 500k | numu    + C12    | 1.0     | QEL-CC
# 3001   | 500k | numu    + C12    | 0.5     | RES-CC
# 3002   | 500k | numu    + C12    | 1.0     | RES-CC
#
# 1101   | 500k | numu    + O16    | 0.5     | COH-CC
# 1102   | 500k | numu    + O16    | 1.0     | COH-CC
# 1103   | 500k | numu    + O16    | 1.5     | COH-CC
# 1111   | 500k | numu    + O16    | 0.5     | COH-NC
# 1112   | 500k | numu    + O16    | 1.0     | COH-NC
# 1113   | 500k | numu    + O16    | 1.5     | COH-NC
# 2101   | 500k | numu    + O16    | 0.5     | QEL-CC
# 2102   | 500k | numu    + O16    | 1.0     | QEL-CC
# 3101   | 500k | numu    + O16    | 0.5     | RES-CC
# 3102   | 500k | numu    + O16    | 1.0     | RES-CC
#
# 1201   | 500k | numu    + Fe56   | 0.5     | COH-CC
# 1202   | 500k | numu    + Fe56   | 1.0     | COH-CC
# 1203   | 500k | numu    + Fe56   | 1.5     | COH-CC
# 1211   | 500k | numu    + Fe56   | 0.5     | COH-NC
# 1212   | 500k | numu    + Fe56   | 1.0     | COH-NC
# 1213   | 500k | numu    + Fe56   | 1.5     | COH-NC
# 2201   | 500k | numu    + Fe56   | 0.5     | QEL-CC
# 2202   | 500k | numu    + Fe56   | 1.0     | QEL-CC
# 3201   | 500k | numu    + Fe56   | 0.5     | RES-CC
# 3202   | 500k | numu    + Fe56   | 1.0     | RES-CC
#......................................................................
#

use File::Path;

# inputs
#
$iarg=0;
foreach (@ARGV) {
  if($_ eq '--run')           { $runnu         = $ARGV[$iarg+1]; }
  if($_ eq '--version')       { $genie_version = $ARGV[$iarg+1]; }
  if($_ eq '--production')    { $production    = $ARGV[$iarg+1]; }
  if($_ eq '--cycle')         { $cycle         = $ARGV[$iarg+1]; }
  if($_ eq '--use-valgrind')  { $use_valgrind  = $ARGV[$iarg+1]; }
  $iarg++;
}
die("** Aborting [Undefined benchmark runs #. Use the --run option]")
unless defined $runnu;
die("** Aborting [Undefined GENIE version. Use the --version option]")
unless defined $genie_version;

$use_valgrind   = 0                         unless defined $use_valgrind;
$production     = "nuint09\_$genie_version" unless defined $production;
$cycle          = "01"                      unless defined $cycle;

$queue          = "prod";
$time_limit     = "30:00:00";
$topdir         = "/opt/ppd/t2k";
$genie_inst_dir = "$topdir/GENIE/";
$genie_setup    = "$genie_inst_dir/$genie_version-setup";
$jobs_dir       = "$topdir/GENIE/scratch/$production\_$cycle";
$xspl_file      = "$topdir/GENIE/data/job_inputs/xspl/gxspl-t2k-$genie_version.xml";
$mcseed         = 210921029;

%nevents_hash = ( 
  '1001' =>  '500000',
  '1002' =>  '500000',
  '1003' =>  '500000',
  '1011' =>  '500000',
  '1012' =>  '500000',
  '1013' =>  '500000',
  '2001' =>  '500000',
  '2002' =>  '500000',
  '3001' =>  '500000',
  '3002' =>  '500000',
  '1101' =>  '500000',
  '1102' =>  '500000',
  '1103' =>  '500000',
  '1111' =>  '500000',
  '1112' =>  '500000',
  '1113' =>  '500000',
  '2101' =>  '500000',
  '2102' =>  '500000',
  '3101' =>  '500000',
  '3102' =>  '500000',
  '1201' =>  '500000',
  '1202' =>  '500000',
  '1203' =>  '500000',
  '1211' =>  '500000',
  '1212' =>  '500000',
  '1213' =>  '500000',
  '2201' =>  '500000',
  '2202' =>  '500000',
  '3201' =>  '500000',
  '3202' =>  '500000'
);

%nupdg_hash = ( 
  '1001' =>  '14',
  '1002' =>  '14',
  '1003' =>  '14',
  '1011' =>  '14',
  '1012' =>  '14',
  '1013' =>  '14',
  '2001' =>  '14',
  '2002' =>  '14',
  '3001' =>  '14',
  '3002' =>  '14',
  '1101' =>  '14',
  '1102' =>  '14',
  '1103' =>  '14',
  '1111' =>  '14',
  '1112' =>  '14',
  '1113' =>  '14',
  '2101' =>  '14',
  '2102' =>  '14',
  '3101' =>  '14',
  '3102' =>  '14',
  '1201' =>  '14',
  '1202' =>  '14',
  '1203' =>  '14',
  '1211' =>  '14',
  '1212' =>  '14',
  '1213' =>  '14',
  '2201' =>  '14',
  '2202' =>  '14',
  '3201' =>  '14',
  '3202' =>  '14'
);

%tgtpdg_hash = ( 
  '1001' =>  '1000060120',
  '1002' =>  '1000060120',
  '1003' =>  '1000060120',
  '1011' =>  '1000060120',
  '1012' =>  '1000060120',
  '1013' =>  '1000060120',
  '2001' =>  '1000060120',
  '2002' =>  '1000060120',
  '3001' =>  '1000060120',
  '3002' =>  '1000060120',
  '1101' =>  '1000080160',
  '1102' =>  '1000080160',
  '1103' =>  '1000080160',
  '1111' =>  '1000080160',
  '1112' =>  '1000080160',
  '1113' =>  '1000080160',
  '2101' =>  '1000080160',
  '2102' =>  '1000080160',
  '3101' =>  '1000080160',
  '3102' =>  '1000080160',
  '1201' =>  '1000260560',
  '1202' =>  '1000260560',
  '1203' =>  '1000260560',
  '1211' =>  '1000260560',
  '1212' =>  '1000260560',
  '1213' =>  '1000260560',
  '2201' =>  '1000260560',
  '2202' =>  '1000260560',
  '3201' =>  '1000260560',
  '3202' =>  '1000260560'
);

%energy_hash = ( 
  '1001' =>  '0.5',
  '1002' =>  '1.0',
  '1003' =>  '1.5',
  '1011' =>  '0.5',
  '1012' =>  '1.0',
  '1013' =>  '1.5',
  '2001' =>  '0.5',
  '2002' =>  '1.0',
  '3001' =>  '0.5',
  '3002' =>  '1.0',
  '1101' =>  '0.5',
  '1102' =>  '1.0',
  '1103' =>  '1.5',
  '1111' =>  '0.5',
  '1112' =>  '1.0',
  '1113' =>  '1.5',
  '2101' =>  '0.5',
  '2102' =>  '1.0',
  '3101' =>  '0.5',
  '3102' =>  '1.0',
  '1201' =>  '0.5',
  '1202' =>  '1.0',
  '1203' =>  '1.5',
  '1211' =>  '0.5',
  '1212' =>  '1.0',
  '1213' =>  '1.5',
  '2201' =>  '0.5',
  '2202' =>  '1.0',
  '3201' =>  '0.5',
  '3202' =>  '1.0'
);

%gevgl_hash = ( 
  '1001' =>  'COH-CC',
  '1002' =>  'COH-CC',
  '1003' =>  'COH-CC',
  '1011' =>  'COH-NC',
  '1012' =>  'COH-NC',
  '1013' =>  'COH-NC',
  '2001' =>  'QEL-CC',
  '2002' =>  'QEL-CC',
  '3001' =>  'RES-CC',
  '3002' =>  'RES-CC',
  '1101' =>  'COH-CC',
  '1102' =>  'COH-CC',
  '1103' =>  'COH-CC',
  '1111' =>  'COH-NC',
  '1112' =>  'COH-NC',
  '1113' =>  'COH-NC',
  '2101' =>  'QEL-CC',
  '2102' =>  'QEL-CC',
  '3101' =>  'RES-CC',
  '3102' =>  'RES-CC',
  '1201' =>  'COH-CC',
  '1202' =>  'COH-CC',
  '1203' =>  'COH-CC',
  '1211' =>  'COH-NC',
  '1212' =>  'COH-NC',
  '1213' =>  'COH-NC',
  '2201' =>  'QEL-CC',
  '2202' =>  'QEL-CC',
  '3201' =>  'RES-CC',
  '3202' =>  'RES-CC'
);

# make the jobs directory
#
mkpath ($jobs_dir, {verbose => 1, mode=>0777});

print "Input runs: $runnu \n";

for my $curr_runnu (keys %gevgl_hash)  {
  print "Checking benchmark run: ...... $curr_runnu \n";

  if($runnu=~m/$curr_runnu/ || $runnu eq "all") {
    print "** matched -> submitting job \n";

    #
    # get runnu-dependent info
    #
    $nev   = $nevents_hash {$curr_runnu};
    $nu    = $nupdg_hash   {$curr_runnu};
    $tgt   = $tgtpdg_hash  {$curr_runnu};
    $en    = $energy_hash  {$curr_runnu};
    $gevgl = $gevgl_hash   {$curr_runnu};

    $batch_script  = "$jobs_dir/nuint09job-$curr_runnu.pbs";
    $logfile_evgen = "$jobs_dir/nuint09job-$curr_runnu.evgen.log";
    $logfile_conv  = "$jobs_dir/nuint09job-$curr_runnu.conv.log";
    $logfile_pbse  = "$jobs_dir/nuint09job-$curr_runnu.pbs_e.log";
    $logfile_pbso  = "$jobs_dir/nuint09job-$curr_runnu.pbs_o.log";

    $grep_pipe     = "grep -B 20 -A 30 -i \"warn\\|error\\|fatal\"";
    $valgrind_cmd  = "valgrind --tool=memcheck --error-limit=no --leak-check=yes --show-reachable=yes";
    $evgen_cmd     = "gevgen -n $nev -s -e $en -p $nu -t $tgt -r $curr_runnu | grep_pipe &> $logfile_evgen";
    $conv_cmd      = "gntpc -f gst -i gntp.$curr_runnu.ghep.root | grep -B 100 -A 30 -i \"warn\\|error\\|fatal\" &> $logfile_conv";

    # create the PBS script
    #
    open(PBS, ">$batch_script") or die("Can not create the PBS batch script");
    print PBS "#!/bin/bash \n";
    print PBS "#PBS -l cput=$time_limit \n";
    print PBS "#PBS -o $logfile_pbso \n";
    print PBS "#PBS -e $logfile_pbse \n";
    print PBS "source $genie_setup \n"; 
    print PBS "cd $jobs_dir \n";
    print PBS "export GSPLOAD=$xspl_file \n";
    print PBS "export GEVGL=$gevgl \n";
    print PBS "export GSEED=$mcseed  \n";
    print PBS "$evgen_cmd \n";
    print PBS "$conv_cmd \n";

    print "EXEC: $evgen_cmd \n";

    # submit job
    #
    `qsub -q $queue $batch_script`;
  }
}
