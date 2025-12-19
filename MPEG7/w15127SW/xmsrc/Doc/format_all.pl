#!/usr/local/bin/perl -w
#
# Create an HTML tree from the XM software component documentation .txt files.
#
# Can be run from any directory.
# No special options for running for the first time.
# The line beginning "@ahgs =" should be edited if more categories are wanted.
#
# Synopsis:
#   format_all <src> <doc_root>
# where:
#  <src> is the source code directory
#  <doc_root> is the directory containing the directories of .txt files
# E.g.:
#   format_all . /buf/mpeg7/NewSrc/Doc
#
# Directory names can be relative or absolute.
#
# URL of resulting HTML tree is <doc_root>/html
#
# Copyright Bill Christmas, CVSSP, University of Surrey, December 2000.

use formatter;
use Cwd;
use File::Copy;

@ahgs = (Audio, Video, MDS); # Description categories

# Check command-line args are present & valid
$program = $0;
$program =~ s|/.*/||;  # just want prog name, not full path.
($src = $ARGV[0] and $root = $ARGV[1])
  or die "Synopsis: " . $program . " <src> <doc_root>\n";
-d $src or die "Source code directory \"$src\" not a valid directory\n";
-d $root
  or die "Documentation root directory \"$root\" not a valid directory\n";

# install any already-created HTML files needed
system ("mkdir -p $root/html");
-d "$root/html"  or die "Cannot create html directory\n";
copy ("$src/index.html", "$root/html/index.html")
  or die "File \"$src/ahg_index.html\" does not exist\n";
copy ("$src/introduction.html", "$root/html")
  or die "File \"$src/introduction.html\" does not exist\n";
chdir $root;

# Create top-level contents page
open INDEX, ">html/contents.html"
  or die "Can't open \"html/contents.html\"\n";
print INDEX "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
  ."<html>\n  <head>\n    <title>MPEG-7 Descriptions</title>\n  </head>\n\n"
  ."<body text=\"#000000\" bgcolor=\"#ffffff\">"
  ."<h1><font color=\"#8800ff\"><center>"
  ."Documentation of MPEG-7 XM Software Components"
  ."</center></font></h1>\n\n"
  ."<a href=\"introduction.html\" target=\"dynamic\">Introduction</a><br>\n";

# Create separate directories of HTML pages for each component category and
# add items to index page
foreach $ahg (@ahgs) {
  # create corresponding HTML directory
  system "mkdir -p html/$ahg";
  -d "html/$ahg" or die "Cannot create directory \"html/$ahg\".\n";
  # add entry for category to index page
  print INDEX "  <a href=\"$ahg/contents.html\">$ahg</a><br>\n";
  # create contents frame
  open CONTENTS, ">html/$ahg/contents.html"
    or die "Can't open contents file for \"$ahg\"\n";
  print CONTENTS "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
    ."<html>\n  <head>\n    <title>$ahg</title>\n  </head>\n\n"
    ."<body text=\"#000000\" bgcolor=\"#eeeeff\">"
    ."<h1><font color=\"#8800ff\"><center>$ahg components"
      ."</center></font></h1>\n\n";
  # convert the .txt files if the directory exists
  if (opendir THISDIR, $ahg) {
    @files = sort readdir THISDIR;
    # for each component, create HTML file & add to contents
    foreach $file (@files) {
      next unless $file =~ s/.txt$//; # only process .txt files; strip off .txt
      print CONTENTS
	"<a href=\"$file.html\" target=\"dynamic\">$file</a><br>\n";
      create_html ("$ahg/$file");  # create the HTML file from the .txt file
    }
  }
  else {
    print "No valid \"$ahg\" directory of .txt files\n";
  }
  # tidy up for category
  print CONTENTS "<hr><a href=\"../index.html\" target=\"_top\">Home</a><br>"
    ."</body></html>\n";
  close CONTENTS;
}
# and tidy up the rest
print INDEX "</body></html>\n";
close INDEX;
