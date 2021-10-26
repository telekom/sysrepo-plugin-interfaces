# Intro
This document contains a list of sysrepocfg commands that can be used to test currently
implemented plugin functionality.
It can also serve as a reference containing a list of currently implemented leafs in the model.

It is assumed that the datastore has already been initialized. As all of the data is currently
automatically populated by the plugin, no import of example data is needed.

## ietf-routing
First we can confirm that the datastore contains any data at all.

```
$ sysrepocfg -X -m ietf-routing
<routing xmlns="urn:ietf:params:xml:ns:yang:ietf-routing">
  <control-plane-protocols>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>unspec</name>
      <description>unspecified protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>redirect</name>
      <description>redirect protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>kernel</name>
      <description>kernel protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>boot</name>
      <description>boot protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:static</type>
      <name>static</name>
      <description>static protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>gated</name>
      <description>gated protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>ra</name>
      <description>ra protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>mrt</name>
      <description>mrt protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>zebra</name>
      <description>zebra protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>bird</name>
      <description>bird protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>dnrouted</name>
      <description>dnrouted protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>xorp</name>
      <description>xorp protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>ntk</name>
      <description>ntk protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>dhcp</name>
      <description>dhcp protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>0x11</name>
      <description>mrouted protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>babel</name>
      <description>babel protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>bgp</name>
      <description>bgp protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>isis</name>
      <description>isis protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>ospf</name>
      <description>ospf protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>rip</name>
      <description>rip protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>eigrp</name>
      <description>eigrp protocol</description>
    </control-plane-protocol>
  </control-plane-protocols>
  <ribs>
    <rib>
      <name>ipv4-main</name>
      <address-family xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:ipv4</address-family>
      <description>main routing table - normal routing table containing all non-policy routes (ipv4 only)</description>
    </rib>
    <rib>
      <name>ipv4-local</name>
      <address-family xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:ipv4</address-family>
      <description>local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv4 only)</description>
    </rib>
    <rib>
      <name>ipv6-main</name>
      <address-family xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:ipv6</address-family>
      <description>main routing table - normal routing table containing all non-policy routes (ipv6 only)</description>
    </rib>
    <rib>
      <name>ipv6-local</name>
      <address-family xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:ipv6</address-family>
      <description>local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv6 only)</description>
    </rib>
  </ribs>
</routing>
```

### control-plane-protocols
```
sysrepocfg -X -x '/ietf-routing:routing/control-plane-protocols'
<routing xmlns="urn:ietf:params:xml:ns:yang:ietf-routing">
  <control-plane-protocols>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>unspec</name>
      <description>unspecified protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>redirect</name>
      <description>redirect protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>kernel</name>
      <description>kernel protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>boot</name>
      <description>boot protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:static</type>
      <name>static</name>
      <description>static protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>gated</name>
      <description>gated protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>ra</name>
      <description>ra protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>mrt</name>
      <description>mrt protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>zebra</name>
      <description>zebra protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>bird</name>
      <description>bird protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>dnrouted</name>
      <description>dnrouted protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>xorp</name>
      <description>xorp protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>ntk</name>
      <description>ntk protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>dhcp</name>
      <description>dhcp protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>0x11</name>
      <description>mrouted protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>babel</name>
      <description>babel protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>bgp</name>
      <description>bgp protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>isis</name>
      <description>isis protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>ospf</name>
      <description>ospf protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>rip</name>
      <description>rip protocol</description>
    </control-plane-protocol>
    <control-plane-protocol>
      <type xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</type>
      <name>eigrp</name>
      <description>eigrp protocol</description>
    </control-plane-protocol>
  </control-plane-protocols>
</routing>
```

This will list all the routing protocols that are known to Linux. This is retrieved from `linux/rtnetlink.h`

### ribs
```
sysrepocfg -X -x '/ietf-routing:routing/ribs'
<routing xmlns="urn:ietf:params:xml:ns:yang:ietf-routing">
  <ribs>
    <rib>
      <name>ipv4-main</name>
      <address-family xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:ipv4</address-family>
      <description>main routing table - normal routing table containing all non-policy routes (ipv4 only)</description>
    </rib>
    <rib>
      <name>ipv4-local</name>
      <address-family xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:ipv4</address-family>
      <description>local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv4 only)</description>
    </rib>
    <rib>
      <name>ipv6-main</name>
      <address-family xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:ipv6</address-family>
      <description>main routing table - normal routing table containing all non-policy routes (ipv6 only)</description>
    </rib>
    <rib>
      <name>ipv6-local</name>
      <address-family xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:ipv6</address-family>
      <description>local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv6 only)</description>
    </rib>
  </ribs>
</routing>
```

The `ribs` container is populated from available Linux routing tables usually visible in `/etc/iproute2/rt_tables`.
In this example, the IPv4 and IPv6 master and local Linux tables are available.

To see actual routes inside the RIBs, we have to query the operational datastore:
```
sysrepocfg -X -d operational -x '/ietf-routing:routing/ribs'
<routing xmlns="urn:ietf:params:xml:ns:yang:ietf-routing">
  <ribs>
    <rib>
      <name>ipv4-main</name>
      <routes>
        <route>
          <route-preference>100</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
            <next-hop-address xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">10.0.2.2</next-hop-address>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">0.0.0.0/0</destination-prefix>
        </route>
        <route>
          <route-preference>100</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">10.0.2.0/24</destination-prefix>
        </route>
        <route>
          <route-preference>1000</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">169.254.0.0/16</destination-prefix>
        </route>
      </routes>
    </rib>
    <rib>
      <name>ipv4-local</name>
      <routes>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">10.0.2.0/32</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">10.0.2.15/32</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">10.0.2.255/32</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>lo</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">127.0.0.0/32</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>lo</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">127.0.0.0/8</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>lo</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">127.0.0.1/32</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>lo</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv4-unicast-routing">127.255.255.255/32</destination-prefix>
        </route>
      </routes>
    </rib>
    <rib>
      <name>ipv6-main</name>
      <routes>
        <route>
          <route-preference>256</route-preference>
          <next-hop>
            <outgoing-interface>lo</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">::1/128</destination-prefix>
        </route>
        <route>
          <route-preference>100</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">fe80::/64</destination-prefix>
        </route>
        <route>
          <route-preference>100</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">fec0::/64</destination-prefix>
        </route>
        <route>
          <route-preference>20100</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
            <next-hop-address xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">fe80::2</next-hop-address>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">::/0</destination-prefix>
        </route>
      </routes>
    </rib>
    <rib>
      <name>ipv6-local</name>
      <routes>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>lo</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">::1/128</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">fe80::7987:cb97:60d:e11e/128</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">fec0::51a3:301:b5b7:23bb/128</destination-prefix>
        </route>
        <route>
          <route-preference>0</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">fec0::8357:aadf:d46:f571/128</destination-prefix>
        </route>
        <route>
          <route-preference>256</route-preference>
          <next-hop>
            <outgoing-interface>enp1s0</outgoing-interface>
          </next-hop>
          <source-protocol xmlns:rt="urn:ietf:params:xml:ns:yang:ietf-routing">rt:direct</source-protocol>
          <active/>
          <destination-prefix xmlns="urn:ietf:params:xml:ns:yang:ietf-ipv6-unicast-routing">ff00::/8</destination-prefix>
        </route>
      </routes>
    </rib>
  </ribs>
</routing>
```

### interfaces

The `interfaces` container lists network-layer interfaces used for routing.

```
sysrepocfg -X -d operational -x '/ietf-routing:routing/interfaces'
<routing xmlns="urn:ietf:params:xml:ns:yang:ietf-routing">
  <interfaces>
    <interface>lo</interface>
    <interface>enp1s0</interface>
  </interfaces>
</routing>
```
