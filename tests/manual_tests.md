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

Then we can retreive the name of an interface:
```
$ sysrepocfg -X -x '/ietf-interfaces:interfaces/interface[name="lo"]/name'
<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">
  <interface>
    <name>lo</name>
  </interface>
</interfaces>
```

Description:
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

Changing the name of a system interface like `lo` isn't supported, and the plugin won't allow it.

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

We can retreive information whether the interface is currently enabled with the following command:
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
