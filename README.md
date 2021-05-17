
# EZ Proxy statistics compilation


This is the code which cleans/processes/compiles the raw EZproxy
web logs and produces a single data set (concatenates all logs)
containing pertinent/derived fields.

### software dependencies

<sup>Reminder: this is only necessary if you want to _build_ this project.
If you just want to consume the data product this outputs, you can skip
to the section named "How to read the data product"</sup>

Assumes a fairly recent version of R, and the following R packages
(as of 2021-05-08)

- `colorout`
- `data.table` (version 1.14.1 or above)
- `magrittr`
- `stringr`
- `openssl`
- `libbib` (any version 1.6.2 or above)

In addition to R, step 1 requires a Common Lisp compiler or interpreter.
[SBCL](www.sbcl.org/) is the Lisp compiler this was developed with.
You also have to install the `clix` package included in this project
in the `dependencies` directory. You might have to play around
with `step-1-clean-raw-logs-YEAR.lisp` to get it working for your
system.

Lastly, `OpenSSL` must be installed for the `openssl` R package
to dynamically link to.

Developed on Debian GNU/Linux 10


### build instructions

The first step is to `rsync` the log directory from the EZproxy web
server to the local directory called `logs`.
You can do this by running `./step-0-sync-logs.sh`.
That file also contains comments/reminders that you have to (a)
be connected to the VPN, and (b) add `ezproxy` to your `/etc/hosts`
or (better) `~/.ssh/config`.

If you're just beginning to use this now, the logs going back to the
beginning of 2021 will no longer be available.

Once the logs have finished (one way) syncing, run
`./step-1-clean-raw-logs-2021.lisp`. This produces a single, cleaned,
intermediate data set containing the IP address, patron barcode,
session, datetime of access, a shortened URL, and the full URL.
This script (in addition to concatenating all daily logs into one file)
excludes certain log entries, cleans URLs, and converts the dates
into ISO 8601 format.
This tab-separated file is stored in `./intermediate/cleaned-logs.dat`.

Finally, run the R script `./step-2-compile-ezproxy-stats-2021.R`.
This is where most of the processing takes place. It, among other things:

- (irreversibly) hashes the patron barcode
- categorized barcode type
- joins with patron data to bring in ptype, home branch, and creation date
  (and _only_ those fields)
- joins with a vendor crosswalk to resolve the URL to a particular
  broad vendor


### how to read the data product

The final product is `./target/exproxy_2021-up-to-YYYY-MM-DD.dat.gz`
where `YYYY-MM-DD` is an ISO 8601 date.
This is a gzipped-tab-delimited data file.
You can un-gzip it (`gzip -d DATAFILE`) and then read it with
any software that reads tab-delimited files. Note that
LibreOffice. Gnumeric, or Excel aren't going to cut it for
this one; the data file can have up to over 80 million rows.
The full data set from 2020 contains 65.8 million rows.

In R, you can use `fread` from the `data.table` package to read
it straight from its gzipped form...

```
dat <- fread("./target/exproxy_2021-up-to-YYYY-MM-DD.dat.gz")
```


### fields

The data product contains the following fields
- `session`
  a unique session identifier

- `ptype`
  patron type

- `date_and_time`
  and ISO 8601 date and time (to the second)

- `vendor`
  a broad vendor category

- `url`
  a shortened URL

- `barcode`
  an (irreversibly) hashed patron barcode

- `barcode_category`
  broad categorization of barcode type

- `homebranch`
  the patron's hone branch

- `fullurl`
  the full URL

- `patroncreatedate`
  The date that the patron was created

- `extract`
  An attempt to pull the specific database accessed from the full URL

- `just_date`
  Just the date of the access (no time component) (in ISO 8601, or course)
