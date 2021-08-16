# Intro
This document contains a list of sysrepocfg commands that can be used to test currently
implemented plugin functionality.
It can also serve as a reference containing a list of currently implemented leafs in the model.

It is assumed that the datastore has already been initialized, and the data in examples imported.
To import the example data run the following command:
```
sysrepocfg --edit=examples/example_config_data.xml -m "ietf-interfaces"
```

And to import the dhcpv6-client example data run the following command:
```
sysrepocfg --edit=examples/example_dhcpv6_client.xml -m "ietf-dhcpv6-client"
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

The interface description is stored by the plugin, currently in a file (`interface_description`) located in `IF_PLUGIN_DATA_DIR` which ensures that
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
    <description/>
    <oper-status>unknown</oper-status>
    <phys-address>00:00:00:00:00:00</phys-address>
    <speed>0</speed>
    <statistics>
      <discontinuity-time>2021-04-12T09:16:12Z</discontinuity-time>
      <in-octets>1160729</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>1160729</out-octets>
      <out-unicast-pkts>18891</out-unicast-pkts>
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
      <discontinuity-time>2021-04-12T09:16:12Z</discontinuity-time>
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
      <discontinuity-time>2021-04-12T09:16:12Z</discontinuity-time>
      <in-octets>222429196</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>34816525</out-octets>
      <out-unicast-pkts>169315</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>20</out-multicast-pkts>
      <out-discards>0</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
  <interface>
    <name>docker0</name>
    <description/>
    <oper-status>down</oper-status>
    <phys-address>02:42:ed:f9:e8:67</phys-address>
    <speed>0</speed>
    <statistics>
      <discontinuity-time>2021-04-12T09:16:12Z</discontinuity-time>
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
    <name>br-467cc91543b9</name>
    <description/>
    <oper-status>up</oper-status>
    <phys-address>02:42:80:ab:de:52</phys-address>
    <speed>0</speed>
    <lower-layer-if>veth6186adb</lower-layer-if>
    <statistics>
      <discontinuity-time>2021-04-12T09:16:12Z</discontinuity-time>
      <in-octets>113833</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>334734</out-octets>
      <out-unicast-pkts>2348</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>5</out-multicast-pkts>
      <out-discards>0</out-discards>
      <out-errors>0</out-errors>
    </statistics>
  </interface>
  <interface>
    <name>veth6186adb</name>
    <description/>
    <oper-status>up</oper-status>
    <phys-address>3a:0c:eb:cf:1c:2b</phys-address>
    <speed>0</speed>
    <higher-layer-if>br-467cc91543b9</higher-layer-if>
    <statistics>
      <discontinuity-time>2021-04-12T09:16:12Z</discontinuity-time>
      <in-octets>128729</in-octets>
      <in-unicast-pkts>0</in-unicast-pkts>
      <in-broadcast-pkts>0</in-broadcast-pkts>
      <in-multicast-pkts>0</in-multicast-pkts>
      <in-discards>0</in-discards>
      <in-errors>0</in-errors>
      <in-unknown-protos>0</in-unknown-protos>
      <out-octets>336160</out-octets>
      <out-unicast-pkts>2353</out-unicast-pkts>
      <out-broadcast-pkts>0</out-broadcast-pkts>
      <out-multicast-pkts>19</out-multicast-pkts>
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

`lower-layer-if` retrieves the slave interface of the current interface. For example if the interface a virtual bridge,
`lower-layer-if` will contain all the interfaces that are a part of that bridge.


The `statistics` node contains various statistics about the interface.
`discontinuity-time` currently contains the system boot time, as we are currently not aware of an easy
way to track interface discontinuities on Linux.

`last-change` contains the time that the interface last change its operational status.
This time is tracked by the plugin internally, by receiving updates about changes
in the state of the interface.

`in-octets` tracks the number of bytes received on an interface. It is retrieved through `libnl`
using the routing part of netlink.

`in-unicats-packets` ethtool is used to retrieve the rx_packets, rx_broadcast, rx_multicast and then rx_unicast is caluclated by subtracting the broadcast and multicast rx packets with the total number of rx packets (rx_unicast = rx_packets - rx_broadcast - rx_multicast).

`in-broadcast-pkts` ethtool is used to retrieve the rx_broadcast packets.

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

`out-unicast-pkts` ethtool is used to retrieve the tx_packets, tx_broadcast, tx_multicast and then tx_unicast is caluclated by subtracting the broadcast and multicast tx packets with the total number of tx packets (tx_unicast = tx_packets - tx_broadcast - tx_multicast).

`out-broadcast-pkts` ethtool is used to retrieve the tx_broadcast packets.

`out-multicast-pkts` ethtool is used to retrieve the tx_multicast packets.

`out-discards` tracks the number of outwards packets that were dropped due to
not enough space available in Linux. It is retrieved through netlink.

`out-errors` tracks the number of outwards packets that weren't transmitted
due to errors.


## ietf-ip

### Interface IPv6 enabled state

We can enable/disable IPv6 addressing of an interface with the `enabled` node with the following command:
```
$ sysrepocfg -Evim -x '/ietf-interfaces:interfaces/interface[name="wlan0"]/ietf-ip:ipv6/enabled'
```

Add the following to the `wlan0` interface:
```
  <ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <enabled>false</enabled>
  </ipv6>
```

Check that the IPv6 addressing has been disabled for the `wlan0` interface:
```
$ ip addr show wlan0
4: wlan0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000
```

If previous IPv6 addresses existed, now they don't.

After setting it through sysrepocfg, the outputs should look like this:
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="wlan0"]/ietf-ip:ipv6/enabled'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>wlan0</name>
    <ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <forwarding>false</forwarding>
    </ipv6>
  </interface>
</interfaces>
```

The IPv4 `enabled` node has not yet been implemented, since not all distribution have the disable_ipv4 file in:
```/proc/sys/net/ipv6/conf/_interface-name_/disable_ipv4```

### Interface address forwarding

We can enable/disable IPv4 and IPv6 address forwarding of an interface with the `forwarding` node with the following command:
```
$ sysrepocfg -Evim -x '/ietf-interfaces:interfaces/interface[name="wlan0"]/ietf-ip:ipv6/forwarding'
```

Add the following to the `wlan0` interface:
```
  <ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <forwarding>false</forwarding>
  </ipv6>
```

Check that the IPv6 forwarding has been disabled for the `wlan0` interface:
```
$  cat /proc/sys/net/ipv6/conf/wlan0/forwarding 
0
```

After setting it through sysrepocfg, the outputs should look like this:
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="wlan0"]/ietf-ip:ipv6/forwarding'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>wlan0</name>
    <ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <enabled>false</enabled>
    </ipv6>
  </interface>
</interfaces>
```

The same can be done for IPv4, just replace ipv6 with ipv4 in the examples above.

### Interface MTU (maximum transmission unit)

We can set the IPv4 MTU of an interface with the following command:
```
$ sysrepocfg -Evim -x '/ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv4/mtu'
```

Add the following to the `lo` an interface:
```
  <ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <mtu>1300</mtu>
  </ipv4>
```

Check that the MTU is really set for the `lo` interface:
```
$ ip link show lo
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 1300 qdisc noqueue state UP mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00

```

After setting it through sysrepocfg, the outputs should look like this:
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv4/mtu'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <mtu>1300</mtu>
    </ipv4>
  </interface>
</interfaces>
```

The same can be done for IPv6, just replace ipv4 with ipv6 in the examples above.

### Interface IP address and subnet

We can set the IPv4 address and subnet of an interface with the following command:
```
$ sysrepocfg -Evim -x '/ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv4/address'
```

Add the following to the `lo` interface:
```
  <ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <address>
        <ip>127.0.0.0</ip>
        <prefix-length>8</prefix-length>
      </address>
  </ipv4>
```

Check that the ip and subnet really are set for the `lo` interface:
```
$ ip addr show lo
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 1300 qdisc noqueue state UP group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.0/8 scope host secondary lo
       valid_lft forever preferred_lft forever
```

After setting it through sysrepocfg, the outputs should look like this:
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv4/address'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
    <ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <address>
        <ip>127.0.0.0</ip>
        <prefix-length>8</prefix-length>
      </address>
    </ipv4>
  </interface>
</interfaces
```

The same can be done for IPv6, just replace ipv4 with ipv6 in the examples above and for the ip node add an ipv6 address.

We can also use a netmask instead of prefix length but the following feature has to be enabled first:
```
 sysrepoctl --change ietf-ip --enable-feature ipv4-non-contiguous-netmasks
```

After that instead of `prefix-length` node use `netmask`:
```
<netmask>255.0.0.0</netmask>
```

### Interface neighbors

We can set the IPv4 and IPv6 ip address and link layer address of an interfaces neighbor with the following command:
```
$ sysrepocfg -Evim -x '/ietf-interfaces:interfaces/interface[name="wlan0"]/ietf-ip:ipv4/neighbor'
```

Add the following to the `wlan0` interface:
```
  <ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <neighbor>
        <ip>192.0.2.2</ip>
        <link-layer-address>00:00:5E:00:53:AB</link-layer-address>
      </neighbor>
  </ipv4>
```

Check that the ip and link layer address really are set for the `wlan0` interface:
```
$ ip neigh show dev wlan0
192.0.2.2 lladdr 00:00:5e:00:53:ab PERMANENT
```

After setting it through sysrepocfg, the outputs should look like this:
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="wlan0"]/ietf-ip:ipv4/neighbor'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>wlan0</name>
    <ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">
      <neighbor>
        <ip>192.0.2.2</ip>
        <link-layer-address>00:00:5E:00:53:AB</link-layer-address>
      </neighbor>
    </ipv4>
  </interface>
</interfaces
```

The same can be done for IPv6, just replace ipv4 with ipv6 in the examples above and for the ip node add an ipv6 address.



## ietf-dhcpv6-client

First we can confirm that the datastore contains any data at all.

```
$ sysrepocfg -X -m ietf-dhcpv6-client
<dhcpv6-client xmlns="urn:ietf:params:xml:ns:yang:ietf-dhcpv6-client">
  <enabled>true</enabled>
  <client-if>
    <if-name>enp0s31f6</if-name>
    <enabled>true</enabled>
    <duid>0004550e8400e29b11d4a716446655440002</duid>
    <client-configured-options>
      <rapid-commit-option/>
      <user-class-option>
        <user-class-data-instance>
          <user-class-data-id>137</user-class-data-id>
          <user-class-data>HR</user-class-data>
        </user-class-data-instance>
        <user-class-data-instance>
          <user-class-data-id>140</user-class-data-id>
          <user-class-data>MANAGMENT</user-class-data>
        </user-class-data-instance>
      </user-class-option>
      <reconfigure-accept-option/>
    </client-configured-options>
  </client-if>
</dhcpv6-client>
```

We can also check the `/etc/dhclient.conf` file to see if it's been configured properly:
```
$ cat /etc/dhclient.conf
interface "enp0s31f6" {
        send dhcp6.client-id "0004550e8400e29b11d4a716446655440002";
        request dhcp6.oro;
        request dhcp6.rapid-commit;
        send user-class "HR";
        send user-class "MANAGMENT";
        send dhcp6.reconf-accept;
        request dhcp6.vendor-opts;
}
```

### dhcpv6-client duid

```
$ sysrepocfg -X -x  "/ietf-dhcpv6-client:dhcpv6-client/client-if[if-name='enp0s31f6']/duid"
<dhcpv6-client xmlns="urn:ietf:params:xml:ns:yang:ietf-dhcpv6-client">
  <client-if>
    <if-name>enp0s31f6</if-name>
    <duid>0004550e8400e29b11d4a716446655440002</duid>
  </client-if>
</dhcpv6-client>
```

### dhcpv6-client rapid-commit-option

```
$ sysrepocfg -X -x  "/ietf-dhcpv6-client:dhcpv6-client/client-if[if-name='enp0s31f6']/client-configured-options/rapid-commit-option"
<dhcpv6-client xmlns="urn:ietf:params:xml:ns:yang:ietf-dhcpv6-client">
  <client-if>
    <if-name>enp0s31f6</if-name>
    <client-configured-options>
      <rapid-commit-option/>
    </client-configured-options>
  </client-if>
</dhcpv6-client>
```

The presence of this empty container indicates that rapid commit option should be requested by the client.

### dhcpv6-client user-class

```
$ sysrepocfg -X -x  " /ietf-dhcpv6-client:dhcpv6-client/client-if[if-name='enp0s31f6']/client-configured-options/user-class-option/user-class-data-instance[user-class-data-id='137']/user-class-data"
<dhcpv6-client xmlns="urn:ietf:params:xml:ns:yang:ietf-dhcpv6-client">
  <client-if>
    <if-name>enp0s31f6</if-name>
    <client-configured-options>
      <user-class-option>
        <user-class-data-instance>
          <user-class-data-id>137</user-class-data-id>
          <user-class-data>HR</user-class-data>
        </user-class-data-instance>
      </user-class-option>
    </client-configured-options>
  </client-if>
</dhcpv6-client>
```

The user-class-data-id is ignored by the plugin because it is not supported by dhclient.

### dhcpv6-client reconfigure-accept-option

```
$ sysrepocfg -X -x " /ietf-dhcpv6-client:dhcpv6-client/client-if[if-name='enp0s31f6']/client-configured-options/reconfigure-accept-option"
<dhcpv6-client xmlns="urn:ietf:params:xml:ns:yang:ietf-dhcpv6-client">
  <client-if>
    <if-name>enp0s31f6</if-name>
    <client-configured-options>
      <reconfigure-accept-option/>
    </client-configured-options>
  </client-if>
</dhcpv6-client>
```

The presence of this empty container indicates that reconfigure accept option should be requested by the client.
