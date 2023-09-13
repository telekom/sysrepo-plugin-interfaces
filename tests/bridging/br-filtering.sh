#!/usr/bin/bash
VETH1_BPORT=$(cat /sys/class/net/veth1_bport/ifindex) envsubst < ../bridging/br-filtering.xml.in > data/bridging/br-filtering.xml
