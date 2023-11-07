#!/usr/local/bin//Rscript --vanilla


# ------------------------------ #
rm(list=ls())

options(echo=TRUE)
options(width = 80)
options(warn=2)
options(scipen=10)
options(datatable.prettyprint.char=50)
options(datatable.print.class=TRUE)
options(datatable.print.keys=TRUE)
options(datatable.fwrite.sep='\t')
options(datatable.na.strings="")

args <- commandArgs(trailingOnly=TRUE)

library(colorout)
library(data.table)
library(magrittr)
library(stringr)
library(libbib)     # version >- 1.6.2
library(openssl)

# ------------------------------ #


proxy <- fread_plus_date("intermediate/cleaned-logs.dat",
                         colClasses="character")
lb_date <- attr(proxy, "lb.date")

proxy[, ip:=NULL]

proxy[, barcode:=str_replace(barcode, "^%a0x", "")]

categorize_barcode <- function(x){
  fcase(
        str_detect(x, "^2333"), "the two threes",
        str_detect(x, "^2777"), "the two sevens",
        x=="auto", "autos",
        str_detect(x, "\\D"), "alphas",
        str_detect(x, "^2111"), "two one ones",
        str_detect(x, "^1624"), "one six twos",
        default = "unknown")
}
proxy[, barcode_category:=categorize_barcode(barcode)]

setkey(proxy, "barcode")


xlate <- fread("./support/barcode-xlate.csv.gz")
setkey(xlate, "barcode")

proxy %>% merge(xlate, all.x=TRUE) -> proxy

proxy[, barcode:=md5(barcode)]

venx <- fread("./support/vendor-xwalk.dat")
setkey(venx, "url")
setkey(proxy, "url")
proxy %>% merge(venx, all.x=TRUE) -> proxy

setcolorder(proxy, c("session", "ptype", "date_and_time", "vendor",
                     "url", "barcode", "barcode_category",
                     "homebranch", "fullurl"))

# This part needs work
proxy[, extract:=str_replace(str_extract(fullurl,
                                         "([Dd][Bb]|[Pp][Rr][Oo][Dd])=.+"),
                             "&.+$", "")]

setkey(proxy, NULL)
setorder(proxy, "date_and_time")

proxy[, just_date:=as.Date(str_sub(date_and_time, 1, 10))]

set_lb_date(proxy, lb_date)

proxy %>%
  fwrite_plus_date("target/ezproxy_2023-up-to.dat.gz", sep=",")

