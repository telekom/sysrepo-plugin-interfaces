#!/usr/bin/bash
ip link add veth1_host type veth peer veth1_bport
ip link set veth1_host address 3e:85:b9:17:9b:04
ip link set veth1_bport address 86:94:92:0d:4a:3b

ip link add veth2_host type veth peer veth2_bport
ip link set veth2_host address 32:4d:f8:15:8c:7e
ip link set veth2_bport address 46:79:63:ae:39:12
