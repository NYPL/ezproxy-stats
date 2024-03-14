#!/bin/bash

THIS_YEAR=$(date +%Y)

# make sure you're connected to the VPN

# add add a entry in /etc/hosts pointing to the ezproxy
# log server
# OR (even better) add it to ~/.ssh/config

rsync -Phaz ezproxy:/usr/local/ezproxy/logs/i.ezproxy.nypl.org.$THIS_YEAR* ./logs
