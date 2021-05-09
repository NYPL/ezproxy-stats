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
library(lubridate)

# ------------------------------ #


proxy <- fread_plus_date("intermediate/cleaned-logs.dat",
                         colClasses="character")

proxy[, ip:=NULL]

proxy[, barcode:=str_replace(barcode, "^%a0x", "")]
# STOP AND WAIT

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
# STOP AND WAIT

setkey(proxy, "barcode")
xlate <- readRDS("./support/barcode-xlate.datatable")
setkey(xlate, "barcode")

xlate[proxy] -> proxy
# STOP AND WAIT


library(openssl)
proxy[, barcode:=md5(barcode)]
# STOP AND WAIT

venx <- fread("./support/vendor-xwalk.dat")
setkey(venx, "url")
setkey(proxy, "url")
venx[proxy] -> proxy
# STOP AND WAIT

names(proxy)

setcolorder(proxy, c("session", "ptype", "date_and_time", "vendor",
                     "url", "barcode", "barcode_category",
                     "homebranch", "fullurl"))


# This part needs work
proxy[, extract:=str_replace(str_extract(fullurl,
                                         "([Dd][Bb]|[Pp][Rr][Oo][Dd])=.+"),
                             "&.+$", "")]
# STOP AND WAIT

setkey(proxy, NULL)
setorder(proxy, "date_and_time")


proxy[, just_date:=ymd(str_sub(date_and_time, 1, 10))]
# STOP AND WAIT


last_valid_date <- proxy[, max(just_date)-1]

set_lb_date(proxy, as.character(last_valid_date))

proxy <- proxy[just_date>=ymd("2021-01-01") & just_date<=last_valid_date, ]

proxy %>%
  fwrite_plus_date("target/exproxy_2021-up-to.dat.gz")
  saveRDS("ezproxy_2021-01-01_2021-04-30.datatable")



# --------------------------------------------------------------- #
# --------------------------------------------------------------- #
# --------------------------------------------------------------- #

# checkpoint
# proxy <- readRDS("fixed-01-01_05-12.datatable")




proxy[, uniqueN(session), .(just_date, vendor)] -> short


setnames(short, "V1", "unique_sessions")



short[vendor!="ezproxy" & vendor!="google"] -> short

short[, sum(unique_sessions), vendor][order(-V1)][1:10][, vendor] -> top10
short[, sum(unique_sessions), vendor][order(-V1)][1:6][, vendor] -> top6

short[vendor %chin% top6] -> kontos

library(lubridate)
kontos[, thedate:=ymd(thedate)]


library(ggplot2)
ggplot(kontos, aes(x=thedate, y=unique_sessions,
                   group=vendor, color=vendor, fill=vendor)) +
  geom_smooth(method="gam", se=FALSE, size=2) + # geom_line() +
  ggtitle("number of unique sessions per vendor (top 6) since january 1st to april 28th") +
  xlab("date") + ylab("number of unique sessions") +
  ggsave("since-jan.png")



# all
short[, sum(unique_sessions), thedate] -> tmp
setnames(tmp, "V1", "total_unique_sessions")
tmp[, thedate:=ymd(thedate)]
ggplot(tmp, aes(x=thedate, y=total_unique_sessions)) +
  geom_smooth() +
              # method="gam", se=false, size=2) + # geom_line() +
  # ggtitle("number of unique sessions per vendor (top 6) since january 1st to april 28th") +
  xlab("date") + ylab("number of unique sessions")





short[vendor %chin% c("oxford", "jstor", "cambridge",
                      "muse", "torrossa", "cairn"), ] -> bajo
bajo[, thedate:=ymd(thedate)]
ggplot(bajo, aes(x=thedate, y=unique_sessions,
                   group=vendor, color=vendor, fill=vendor)) +
  geom_smooth(method="gam", se=FALSE, size=2) + # geom_line() +
  ggtitle("number of unique sessions per academic e-book vendor since january 1st") +
  xlab("date") + ylab("number of unique sessions") +
  ggsave("ebooks.png")

# --------------------------------------------------------------- #
# --------------------------------------------------------------- #
# --------------------------------------------------------------- #
# --------------------------------------------------------------- #





uniqueN(proxy[, session])

proxy[vendor!="(not yet categorized)"][, uniqueN(session), vendor][order(-V1)]

#                     vendor    V1
#                     <char> <int>
#        1:         Proquest 25018
#        2:            EBSCO 11827
#        3:             Gale 11099
#        4:             Muse  4100
#        5:      Morningstar  3958
#        6:           Oxford  3149
#        7:              OED  2502
#        8:            JStor  2324
#        9: University Press  1547
#       10:          Fulcrum   878
#       11:         Newsbank   853
#       12: Learning Express   656
#       13:        Cambridge   573
#       14:            Brill   412
#       15:             Sage   332
#       16:             Emis    21
#       17:         Torrossa    13


# proxy[!duplicated(session)] -> tmp

proxy[vendor %chin% c("Proquest", "EBSCO", "Gale", "Muse")] -> short

library(lubridate)
short[, themonth:=month(ymd(str_sub(date, 0, 10)))]
short[themonth %in% c(1, 2, 3)] -> short

short[, uniqueN(session), .(themonth, vendor)] -> tmp

library(ggplot2)
setnames(tmp, "V1", "unique_sessions")
ggplot(tmp, aes(x=themonth, y=unique_sessions, fill=vendor, group=vendor)) +
  geom_line()

