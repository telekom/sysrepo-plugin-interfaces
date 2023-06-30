# bridging-plugin manual testing

## Intro
This document contains a list of commands that can be used to test currently
implemented bridging-plugin functionality. All commands should be executed while the plugin is
running. Running the interfaces plugin is not necessary for testing.

This is a tree view of `ieee802-dot1q-bridge` YANG module nodes currently implemented in the plugin:
```
module: ieee802-dot1q-bridge
  +--rw bridges
     +--rw bridge* [name]
        +--rw name           dot1qtypes:name-type
        +--rw address        ieee:mac-address
        +--rw bridge-type    identityref (partial - only the customer-vlan-bridge type)
        +--ro ports?         uint16
        +--rw component* [name]
           +--rw name                     string
           +--rw id?                      uint32
           +--rw type                     identityref (partial - d-bridge-component and edge-relay-component missing)
           +--rw address?                 ieee:mac-address (note: used as
           +--ro ports?                   uint16
           +--ro bridge-port*             if:interface-ref
           +--rw filtering-database
           |  +--rw aging-time?                          uint32
           |  +--rw filtering-entry* [database-id vids address]
           |  |  +--rw database-id    uint32 (value ignored)
           |  |  +--rw address        ieee:mac-address
           |  |  +--rw vids           dot1qtypes:vid-range-type
           |  |  +--rw entry-type?    enumeration (partial - static entries only)
           |  |  +--rw port-map* [port-ref]
           |  |  |  +--rw port-ref                                   port-number-type
           |  |  |  +--rw (map-type)?
           |  |  |     +--:(static-filtering-entries)
           |  |  |     |  +--rw static-filtering-entries
           |  |  |     |     +--rw control-element?         enumeration (partial - 'forward' only)
           |  +--rw vlan-registration-entry* [database-id vids]
           |     +--rw database-id    uint32 (value ignored)
           |     +--rw vids           dot1qtypes:vid-range-type
           |     +--rw entry-type?    enumeration (partial - 'static' only)
           |     +--rw port-map* [port-ref]
           |        +--rw port-ref                                   port-number-type
           |        +--rw (map-type)?
           |           +--:(static-vlan-registration-entries)
           |           |  +--rw static-vlan-registration-entries
           |           |     +--rw vlan-transmitted?          enumeration
           +--rw bridge-vlan
           |  +--ro version?                   uint16
           |  +--ro max-vids?                  uint16
           |  +--ro override-default-pvid?     boolean
           |  +--rw vlan* [vid]
           |  |  +--rw vid               dot1qtypes:vlan-index-type
           |  |  +--rw name?             dot1qtypes:name-type
           |  |  +--ro untagged-ports*   if:interface-ref
           |  |  +--ro egress-ports*     if:interface-ref

  augment /if:interfaces/if:interface:
    +--rw bridge-port
       +--rw component-name?                        string
       +--rw port-type?                             identityref
       +--rw pvid?                                  dot1qtypes:vlan-index-type
       +--ro port-number?                           dot1qtypes:port-number-type (currently set to the interface index)
```

## Create bridge & bridge component

First, create a 802.1Q VLAN aware bridge (type: customer-vlan-bridge) and component.
A customer-vlan-bridge is associated with a single c-vlan-component.

Config data:
```xml
<bridges xmlns="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">
  <bridge>
    <name>br0</name>
    <address>42-50-a1-c4-23-78</address>
    <bridge-type xmlns:dot1q="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">dot1q:customer-vlan-bridge</bridge-type>
  <component>
    <name>br0</name>
    <address>42-50-a1-c4-23-78</address>
    <type xmlns:dot1q="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">dot1q:c-vlan-component</type>
  </component>
  </bridge>
</bridges>
```

Command:
```bash
sysrepocfg --edit=bridge-component.xml
```

Creating a bridge component in the datastore should create a Linux bridge
interface with the same name.

```bash
$ ip link show br0
14: br0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 42:50:a1:c4:23:78 brd ff:ff:ff:ff:ff:ff
```

## Create bridge component ports

To test bridging functionality (frame filtering and forwarding based on MAC address and VLAN ID),
we need to add ports to the bridge. Testing without physical interfaces can be done
by using veth interfaces on Linux. veth interfaces are always created in pairs, and when
a frame is transmitted on a veth interface it will always be received on its veth peer, similar
to a physical connection between two devices. For testing purposes, one veth interface
can represent a VM or a host in a local network and its veth peer can be used as a bridge port
to which this 'host' is connected to. By creating two veth pairs, we can test the functionality
necessary to forward and filter frames sent by two machines which are connected to a 
VLAN aware bridge.

To create the necessary interfaces run the [setup_veth.sh](setup_veth.sh) script or execute the following commands:

```bash
$ ip link add veth1_host type veth peer veth1_bport
$ ip link set veth1_host address 3e:85:b9:17:9b:04
$ ip link set veth1_bport address 86:94:92:0d:4a:3b

$ ip link add veth2_host type veth peer veth2_bport
$ ip link set veth2_host address 32:4d:f8:15:8c:7e
$ ip link set veth2_bport address 46:79:63:ae:39:12
```

After creating the interfaces, `veth1_port` and `veth2_port` should be set as ports of the `br0` bridge component.

Config:
```xml
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>veth1_bport</name>
    <description>test port for br0 bridge</description>
    <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:ethernetCsmacd</type>
    <bridge-port xmlns="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">
      <component-name>br0</component-name>
    </bridge-port>
  </interface>
  <interface>
    <name>veth2_bport</name>
    <description>test port for br0 bridge</description>
    <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:ethernetCsmacd</type>
    <bridge-port xmlns="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">
      <component-name>br0</component-name>
    </bridge-port>
  </interface>
</interfaces>
```

Command:
```bash
sysrepocfg --edit=veth-ports.xml
```

Now the output for the following commands should include `master br0` for both ports:

```bash
$ ip link show veth1_bport
10: veth1_bport@veth1_host: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue master br0 state UP mode DEFAULT group default qlen 1000
    link/ether 86:94:92:0d:4a:3b brd ff:ff:ff:ff:ff:ff
$ ip link show veth2_bport
12: veth2_bport@veth2_host: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue master br0 state UP mode DEFAULT group default qlen 1000
    link/ether 46:79:63:ae:39:12 brd ff:ff:ff:ff:ff:ff
```

The next step is to enable the bridge all the veth interfaces (TODO: do this in the plugin):

```bash
ip link set br0 up
ip link set veth1_host up
ip link set veth1_bport up
ip link set veth2_host up
ip link set veth2_bport up
```

## Create port VLAN entries

To test VLAN functionality, `veth1_bport` and `veth2_bport` should both be added to VLAN 20.
A port can also be added to a range of VLANs.

The following config data adds `veth1_bport` and `veth2_bport` to VLAN 20, and adds `veth2_bport` to VLANs
8-10:

```xml
<bridges xmlns="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">
  <bridge>
    <name>br0</name>
    <bridge-type xmlns:dot1q="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">dot1q:customer-vlan-bridge</bridge-type>
  <component>
    <name>br0</name>
    <type xmlns:dot1q="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">dot1q:c-vlan-component</type>
    <filtering-database>
      <vlan-registration-entry>
        <database-id>0</database-id>
        <vids>20</vids>
        <port-map xmlns:dot1qtypes="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
          <port-ref>${VETH1_BPORT}</port-ref>
          <static-vlan-registration-entries>
            <vlan-transmitted>untagged</vlan-transmitted>
          </static-vlan-registration-entries>
        </port-map>
        <port-map xmlns:dot1qtypes="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
          <port-ref>${VETH2_BPORT}</port-ref>
          <static-vlan-registration-entries>
            <vlan-transmitted>tagged</vlan-transmitted>
          </static-vlan-registration-entries>
        </port-map>
      </vlan-registration-entry>
      <vlan-registration-entry>
        <database-id>0</database-id>
        <vids>8-10</vids>
        <port-map xmlns:dot1qtypes="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
          <port-ref>${VETH2_BPORT}</port-ref>
          <static-vlan-registration-entries>
            <vlan-transmitted>tagged</vlan-transmitted>
          </static-vlan-registration-entries>
        </port-map>
      </vlan-registration-entry>
    </filtering-database>
    <bridge-vlan>
      <vlan>
        <vid>20</vid>
        <name>veth-test-vlan</name>
      </vlan>
    </bridge-vlan>
  </component>
  </bridge>
</bridges>
```

In the `ieee-802dot1q-bridge` YANG module,
ports are referenced by `port-number` (a read-only value in the datastore) when creating VLAN or MAC filtering entries. Currently, the plugin uses the interface ifindex as the `port-number` value.
To add VLAN entries, first substitute the variables in the configuration file
with the correct ifindex values:

```bash
$ VETH1_BPORT=$(cat /sys/class/net/veth1_bport/ifindex) VETH2_BPORT=$(cat /sys/class/net/veth2_bport/ifindex) envsubst < br-vlan.xml.in > br-vlan.xml
```

Then run the command:
```bash
$ sysrepocfg --edit=br-vlan.xml
```

The `bridge vlan show` command output should now contain the port-VLAN combinations
which were added to the datastore. By default, Linux bridges with VLAN filtering enabled
use VID 1 as the primary VLAN ID (PVID) on all ports:
```bash
$ bridge vlan show
port              vlan-id
br0               1 PVID Egress Untagged
veth1_bport       1 PVID Egress Untagged
                  20 Egress Untagged
veth2_bport       1 PVID Egress Untagged
                  8
                  9
                  10
                  20
```

TODO: `bridge-vlan` container operational ds output

## Add a MAC address filtering entry & configure filtering database aging time

The following config data adds a permanent MAC address filtering entry to ensure that the bridge forwards frames intended
for veth1_host on veth1_bport, as long as the frame was received from VLAN 20, and sets the aging time of
learned MAC entries to 10000 seconds:

```xml
<bridges xmlns="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">
  <bridge>
    <name>br0</name>
    <bridge-type xmlns:dot1q="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">dot1q:customer-vlan-bridge</bridge-type>
  <component>
    <name>br0</name>
    <type xmlns:dot1q="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge">dot1q:c-vlan-component</type>
    <filtering-database>
      <aging-time>10000</aging-time>
      <filtering-entry>
        <database-id>0</database-id>
        <vids>20</vids>
        <address>3e-85-b9-17-9b-04</address>
        <entry-type>static</entry-type>
        <port-map xmlns:dot1qtypes="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
          <port-ref>${VETH1_BPORT}</port-ref>
          <static-filtering-entries>
            <control-element>forward</control-element>
          </static-filtering-entries>
        </port-map>
      </filtering-entry>
    </filtering-database>
  </component>
  </bridge>
</bridges>
```

Substitute the variables in the configuration file with the correct ifindex values:
```bash
$ VETH1_BPORT=$(cat /sys/class/net/veth1_bport/ifindex) envsubst < br-filtering.xml.in > br-filtering.xml
```

Then add the filtering configuration to the datastore:
```bash
$ sysrepocfg --edit=br-filtering.xml
```

The bridge filtering database should now contain the added entry:
```bash
$ bridge fdb show | grep 3e:85:b9:17:9b:04
3e:85:b9:17:9b:04 dev veth1_bport vlan 20 master br0 permanent
```

Ageing time should also be updated for br0:
```bash
$ ip -d  link show br0 | grep -Po 'ageing_time \d*'
ageing_time 10000
```

## Frame filtering and forwarding

To test VLAN and MAC address filtering on the bridge, we can bind a raw socket on an interface, construct an ethernet
frame with an appropriate source/destionation MAC addresses and VLAN ID (VID), and send it to a bridge port.
The [send_frame.py](send_frame.py) script (adapted from [this blog post](https://iximiuz.com/en/posts/networking-lab-simple-vlan/))
sends a frame with a VID 20 802.1Q tag from a chosen interface to a chosen destination
MAC address.

The following test sends a frame from `veth2_host` to `veth1_host`. The `br0` bridge should receive the frame
over `veth2_bport` and forward it to `veth1_host` over `veth1_bport`.

Frames received on `veth1_host` should be monitored with tcpdump in a different terminal session:
```bash
$ tcpdump -i veth1_host -ne
tcpdump: verbose output suppressed, use -v[v]... for full protocol decode
listening on veth1_host, link-type EN10MB (Ethernet), snapshot length 262144 bytes
```

Command for sending the frame:
```bash
$ python3 send_frame.py veth2_host 3e:85:b9:17:9b:04  "message for veth1_host"
```

Now the frame show up in the tcpdump output:

```bash
$ tcpdump -i veth1_host -ne
tcpdump: verbose output suppressed, use -v[v]... for full protocol decode
listening on veth1_host, link-type EN10MB (Ethernet), snapshot length 262144 bytes
11:47:31.489628 32:4d:f8:15:8c:7e > 3e:85:b9:17:9b:04, ethertype Unknown (0x7a05), length 36:
        0x0000:  6d65 7373 6167 6520 666f 7220 7665 7468  message.for.veth
        0x0010:  315f 686f 7374                           1_host
```

###  Frame filtering - variations and expected results

* If you change the frame VID in the `send_frame.py` script, the frame should be dropped by the bridge because `veth1_bport`
uses only VIDs 1 and 20.

* The frame should also be dropped if you remove `veth2_bport` from VLAN 20 (by deleting the `port-map` container for `veth2_bport` for VID 20).

* If you change the `<vlan-transmitted>untagged</vlan-transmitted>` node to `tagged` for `veth1_bport` and try
to send the frame again, the VLAN tag should stay in the frame and show up in the tcpdump output:

```bash
$ tcpdump -i veth1_host -ne
tcpdump: verbose output suppressed, use -v[v]... for full protocol decode
listening on veth1_host, link-type EN10MB (Ethernet), snapshot length 262144 bytes
17:11:15.776157 32:4d:f8:15:8c:7e > 3e:85:b9:17:9b:04, ethertype 802.1Q (0x8100), length 40: vlan 20, p 0, ethertype Unknown (0x7a05),
        0x0000:  6d65 7373 6167 6520 666f 7220 7665 7468  message.for.veth
        0x0010:  315f 686f 7374                           1_host
```
