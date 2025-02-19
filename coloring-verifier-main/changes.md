## Change History

- 11 June 2024. Refactoring. v3.1

- 06 Sept 2022. Moved to git. v3.0

- 20 May 2020. Contribution by Willem van Hoeve. Updated test_sol.c to scan for "c "
  and "p " (with a space) to avoid that when parsing the dimension of
  DIMACS instances, it scans for a "c" in the line, even if this "c" may
  appear anywhere.  Since some instances have "p col" for the dimension,
  the program was skipping that line as it contains a "c".  Moreover, it
  has been added the cases "edge", "edges", and "col" to identify the
  possible edge dimension. 



## Previous versions 

Available at: https://imada.sdu.dk/~marco/gcp/gcp_check_sol

| Version               | Date               | Note                                                                                                  |
| --------------------- | ------------------ | ----------------------------------------------------------------------------------------------------- |
| gcp_check_sol-2.8.tgz | 2018-10-29 19:50   | Bug fix: fixed the reporting of the number of edges from an ascii instance. (Thanks to Marko Bukal)   |
| gcp_check_sol-2.7.tgz | 2013-10-23 19:04   | Handling of empty or incomplete colorings in solution file                                            |
| gcp_check_sol-2.6.tgz | 2013-10-14 09:50   | Added reader for bin instance file format                                                             |
| test_sol-2.5.tgz      | 2013-08-06 19:03   | Added sum coloring calculation                                                                        |
| test_sol-2.4.tgz      | 2012-10-04 17:27   | Added interval coloring checker                                                                       |
| test_sol: V2.3        | 2010-10-06 16:22   | corrected a bug in memory allocation                                                                  |
| test_sol:  V2.2       | 2004-02-10 16:50   |                                                                                                       |

