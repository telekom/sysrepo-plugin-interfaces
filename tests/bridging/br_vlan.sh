#!/usr/bin/bash
VETH1_BPORT=$(cat /sys/class/net/veth1_bport/ifindex) VETH2_BPORT=$(cat /sys/class/net/veth2_bport/ifindex) envsubst < ../bridging/br-vlan.xml.in > data/bridging/br-vlan.xml
