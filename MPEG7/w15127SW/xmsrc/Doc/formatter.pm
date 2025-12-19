#!/usr/local/bin/perl -w

# Reads an XM software component documentation file and creates an HTML page.

# Copyright Bill Christmas, CVSSP, University of Surrey, December 2000.

package formatter;
use Exporter ();
@ISA = qw(Exporter);
@EXPORT = qw(create_html);

sub create_html {
  my ($description) = @_;
  open (TXT, "<$description.txt")
    or die "Can't read from file \"$description.txt\"\n";
  open (HTML, ">html/$description.html")
    or die "Can't write to file \"html/$description.html\"\n";

  print HTML "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
    . "<html>\n  <head>\n    <title>$description</title>\n  </head>\n\n"
    . "<body text=\"#000000\" bgcolor=\"#ffffff\">\n"
    . "<h1><font color=\"#8800ff\">"
    . "<center>Documentation for $description</center>"
    . "</font></h1>\n"
    . "<table border=\"1\">\n"
    . "<colgroup><col width=\"20%\"><col width=\"80%\"></colgroup>\n";

  $name = "";         # name of current item
  $description = "";  # description of current item
  while ($line = <TXT>) {
    chomp $line;      # remove line feeds
    $line =~ s/\r//;  # and carriage returns
    @field = split /\t+/, $line, 2;  # split line on tab char
    next if (scalar (@field) == 0)  # ignore completely empty lines
      or ((scalar (@field) == 1) and (!($field[0] =~ /\S/)));
    if ($field[0] =~ /\w+/) { # 1st field is not empty, so start new row
      # in which case 1st o/p old row if it exists
      print HTML "<tr><th>$name\n<td>$description\n" if length ($name) > 0;
      if (scalar (@field) == 1) {  # 2nd field is missing: naughty!
	$name = "<font color=\"888888\">$field[0]</font>";
	$description = "&nbsp;";  # force 2nd cell in table row
      }
      else {  # both fields present
	$name = $field[0];
	$description = $field[1];
      }
    }
    else {  # 1st field empty, so append to current row
      $description = "$description<br>\n$field[1]";
    }
  }
  print HTML "</table>\n</body>\n</html>\n";
}
