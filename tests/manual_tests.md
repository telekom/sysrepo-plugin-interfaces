# Intro
This document contains a list of sysrepocfg commands that can be used to test currently
implemented plugin functionality.
It can also serve as a reference containing a list of currently implemented leafs in the model.

It is assumed that the datastore has already been initialized, and the data in examples imported.
To import the example data run the following command:
```
sysrepocfg --edit=examples/example_config_data.xml -m "ietf-interfaces"
```

## ietf-interfaces
First we can confirm that the datastore contains any data at all.

```
$ sysrepocfg -X -m ietf-interfaces
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <description/>
    <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:softwareLoopback</type>
    <enabled>false</enabled>
  </interface>
  <interface>
    <name>enp0s31f6</name>
    <description/>
    <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:ethernetCsmacd</type>
    <enabled>false</enabled>
  </interface>
  <interface>
    <name>wlp0s20f3</name>
    <description/>
    <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:ethernetCsmacd</type>
    <enabled>true</enabled>
  </interface>
  <interface>
    <name>test_interface</name>
    <description>this is a test interface</description>
    <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:other</type>
    <enabled>false</enabled>
  </interface>
  <interface>
    <name>eth0.2</name>
    <description>this is a test interface</description>
    <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:l2vlan</type>
    <enabled>false</enabled>
  </interface>
</interfaces>
```

### Interface name
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/name'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
  </interface>
</interfaces>
```

Changing the name of a system interface like `lo` isn't supported, and the plugin won't allow it.

### Interface description
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/description'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <description/>
  </interface>
</interfaces>
```

Since the description is empty, we can set it with the following command:
```
sysrepocfg -Evim -fjson -x '/ietf-interfaces:interfaces/interface[name="lo"]/description'
```

After setting the description to the value "test", the output should look something like this.
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/description'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <description>test</description>
  </interface>
</interfaces>
```

The interface description is stored by the plugin, currently in a file which sould ensure that
set descriptions are preserved between plugin runs.

### Interface type

Next, we can get the type of an interface.
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/type'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:softwareLoopback</type>
  </interface>
</interfaces>
```

Changing the type of a system interface like `lo` also won't work.
Types have to be set based on the `iana-if-type` model.
Currently supported interfaces include `ethernetCsmacd`, `other`, `softwareLoopback` and `l2vlan`.
Any other unsupported types, for example bridges, shouldn't appear in the datastore.

### Interface enabled state

We can retrieve information whether the interface is currently enabled with the following command:
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/enabled'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <enabled>false</enabled>
  </interface>
</interfaces>
```

Check that it is really disabled on the system:

```
$ ip link show lo
1: lo: <LOOPBACK> mtu 65536 qdisc noqueue state DOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
```

After enabling it through sysrepocfg, the outputs should look like this:

```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/enabled'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <enabled>true</enabled>
  </interface>
</interfaces>

$ ip link show lo
1: lo: <LOOPBACK> mtu 65536 qdisc noqueue state DOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
```

## ietf-interfaces operational data
Retrieving operational data from the datastore after importing the example
data should look something like this:

```
$ sysrepocfg -X -d operational -m ietf-interfaces
sysrepocfg -X -d operational -m ietf-interfaces
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <description>test</description>
    <oper-status>unknown</oper-status>
    <last-change>2021-03-23T12:21:23Z</last-change>
    <phys-address>00:00:00:00:00:00</phys-address>
    <speed>0</speed>
    <statistics>
      <discontinuity-time>2021-03-22T19:38:35Z</discontinuity-time>
      <in-octets>0</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>0</out-octets>
      <out-unicast-pkts>2776</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>0</out-multicast-pkts>
      <out-discards>0</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
  <interface>
    <name>enp0s31f6</name>
    <description/>
    <oper-status>down</oper-status>
    <phys-address>98:fa:9b:78:03:6f</phys-address>
    <speed>0</speed>
    <statistics>
      <discontinuity-time>2021-03-22T19:38:35Z</discontinuity-time>
      <in-octets>0</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>0</out-octets>
      <out-unicast-pkts>0</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>0</out-multicast-pkts>
      <out-discards>0</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
  <interface>
    <name>wlp0s20f3</name>
    <description/>
    <oper-status>up</oper-status>
    <phys-address>d0:ab:d5:33:62:c9</phys-address>
    <speed>0</speed>
    <statistics>
      <discontinuity-time>2021-03-22T19:38:35Z</discontinuity-time>
      <in-octets>0</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>0</out-octets>
      <out-unicast-pkts>18016</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>23</out-multicast-pkts>
      <out-discards>1</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
  <interface>
    <name>br-467cc91543b9</name>
    <description/>
    <oper-status>up</oper-status>
    <phys-address>02:42:57:2b:23:af</phys-address>
    <speed>0</speed>
    <statistics>
      <discontinuity-time>2021-03-22T19:38:35Z</discontinuity-time>
      <in-octets>0</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>0</out-octets>
      <out-unicast-pkts>251</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>5</out-multicast-pkts>
      <out-discards>0</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
  <interface>
    <name>docker0</name>
    <description/>
    <oper-status>up</oper-status>
    <phys-address>02:42:7c:69:df:d7</phys-address>
    <speed>0</speed>
    <statistics>
      <discontinuity-time>2021-03-22T19:38:35Z</discontinuity-time>
      <in-octets>0</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>0</out-octets>
      <out-unicast-pkts>9994</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>21</out-multicast-pkts>
      <out-discards>0</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
  <interface>
    <name>vethcb51a04</name>
    <description/>
    <oper-status>up</oper-status>
    <phys-address>5a:9d:4c:be:19:d4</phys-address>
    <speed>0</speed>
    <higher-layer-if>br-467cc91543b9</higher-layer-if>
    <statistics>
      <discontinuity-time>2021-03-22T19:38:35Z</discontinuity-time>
      <in-octets>0</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>0</out-octets>
      <out-unicast-pkts>256</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>15</out-multicast-pkts>
      <out-discards>0</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
  <interface>
    <name>veth44c145f</name>
    <description/>
    <oper-status>up</oper-status>
    <phys-address>42:7a:06:90:46:87</phys-address>
    <speed>0</speed>
    <higher-layer-if>br-467cc91543b9</higher-layer-if>
    <higher-layer-if>docker0</higher-layer-if>
    <statistics>
      <discontinuity-time>2021-03-22T19:38:35Z</discontinuity-time>
      <in-octets>0</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>0</out-octets>
      <out-unicast-pkts>56</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>14</out-multicast-pkts>
      <out-discards>0</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
</interfaces>
```

Obviously, the name, description and oper-status fields match the name, description and enabled nodes
in the running datastore.

The `phys-address` node should contain the physical address (usually MAC) of the interface.

`speed` contains an approximation of the bits per second rate estimation retrieved through the Linux `tc` subsystem.

`higher-layer-if` retrieves the master interface of the current interface. For example if the interface is part of a virtual bridge,
`higher-layer-if` will contain the name of the virtual bridge.

The `statistics` node contains various statistics about the interface.
`discontinuity-time` currently contains the system boot time, as we are currently not aware of an easy
way to track interface discontinuities on Linux.

`last-change` contains the time that the interface last change its operational status.
This time is tracked by the plugin internally, by receiving updates about changes
in the state of the interface.

`in-octets` tracks the number of bytes received on an interface. It is retrieved through `libnl`
using the routing part of netlink.

`in-unicats-packets` won't be correct and will be mostly 0 for now,
as there doesn't seme to be a way to retrieve or calculate
just the number of unicast packets with just netlink. ethtool uses ioctl for example, which
will be device and driver specific.

`in-broadcast-pkts` won't be correct and will mostly be 0 as netlink doesn't have a way to retrieve
the number of broadcast packets received on an interface.

`in-multicast-pkts` is set to the number of received multicast packets, retrieved through netlink.

`in-discards` is set to the number of packets dropped due to insufficient buffer space in Linux.
This is retrieved through netlink.

`in-errors` is set to the number of received packets with errors. The data
is retrieved through netlink.

`in-unknown-protos` is set to the number of packets with unknown protocols received on
the interface. The data is retrieved from netlink, through the IP6 SNMP data. 
In most cases it will be 0.

`out-octets` is the number of packets transmitted on the interface. It is retrieved
through netlink.

`out-unicast-pkts` will mostly be 0 and incorrect, as netlink doesn't have
a way to retrieve the number of transmitted unicast packets.

`out-broadcast-pkts` will mostly be 0 and incorrect, as netlink doesn't have
a way to retrieve the number of transimtted broadcast packets.

`out-multicast-pkts` will be incorrect as netlink doesn't have a way
to retrieve the number of transmitted multicast packets.

`out-discards` tracks the number of outwards packets that were dropped due to
not enough space available in Linux. It is retrieved through netlink.

`out-errors` tracks the number of outwards packets that weren't transmitted
due to errors.
