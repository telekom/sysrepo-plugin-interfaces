#!/usr/bin/env python3

# Send ethernet frame with VLAN ID (vid) 20.
#
# Usage: send_frame.py source_interface dest_addr message
# Eg.:   send_frame.py eth0 ff:ff:ff:ff:ff:ff 'Hello everybody!'
#        send_frame.py eth0 06:e5:f0:20:af:7a 'Hello 06:e5:f0:20:af:7a!'
#
# Note: CAP_NET_RAW capability is required to use SOCK_RAW

import fcntl
import socket
import struct
import sys

def send_frame(ifname, dstmac, eth_type, payload):
    # Open raw socket and bind it to network interface.
    s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)
    s.bind((ifname, 0))

    # Get source interface's MAC address.
    info = fcntl.ioctl(
        s.fileno(), 0x8927, struct.pack('256s', bytes(ifname, 'utf-8')[:15])
    )
    srcmac = ':'.join('%02x' % b for b in info[18:24])
    print(ifname, "mac address:", srcmac)	

    # Build Ethernet frame
    payload_bytes = payload.encode('utf-8')
    assert len(payload_bytes) <= 1500  # Ethernet MTU
    tpid = b'\x81\x00' # dot1q
    vid = b'\x00\x14' # vid 20 (0x14), pcp & dei set to 0
    frame = human_mac_to_bytes(dstmac) + human_mac_to_bytes(srcmac) + \
            tpid + vid + eth_type + payload_bytes
    print("frame bytes:")
    print(frame)

    # Send Ethernet frame
    return s.send(frame)

def human_mac_to_bytes(addr):
    return bytes.fromhex(addr.replace(':', ''))

def main():
    ifname = sys.argv[1]
    dstmac = sys.argv[2]
    payload = sys.argv[3]
    ethtype = b'\x7A\x05'  # arbitrary, non-reserved
    send_frame(ifname, dstmac, ethtype, payload)

if __name__ == "__main__":
    main()
