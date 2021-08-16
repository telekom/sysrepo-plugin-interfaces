## Development Setup

Setup the development environment using the provided [`setup-dev-sysrepo`](https://github.com/sartura/setup-dev-sysrepo) scripts. This will build all the necessary components.

Subsequent rebuilds of the plugin may be done by navigating to the plugin source directory and executing:

```
$ export SYSREPO_DIR=${HOME}/code/sysrepofs
$ cd ${SYSREPO_DIR}/repositories/plugins/sysrepo-plugin-interfaces

$ rm -rf ./build && mkdir ./build && cd ./build
$ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_PREFIX_PATH=${SYSREPO_DIR} \
		-DCMAKE_INSTALL_PREFIX=${SYSREPO_DIR} \
		-DCMAKE_BUILD_TYPE=Debug \
		..
-- Configuring done
-- Generating done
-- Build files have been written to: ${SYSREPO_DIR}/repositories/plugins/sysrepo-plugin-interfaces/build
Scanning dependencies of target sysrepo-plugin-interfaces
[ 33%] Building C object CMakeFiles/sysrepo-plugin-interfaces.dir/src/interfaces.c.o
[ 66%] Linking C executable sysrepo-plugin-interfaces
[100%] Built target sysrepo-plugin-interfaces
```

For building both the dhcpv6-client and interfaces plugin add the `-DDHCPv6_CLIENT` flag to cmake:

```
$ cmake -DDHCPv6_CLIENT=ON \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_PREFIX_PATH=${SYSREPO_DIR} \
		-DCMAKE_INSTALL_PREFIX=${SYSREPO_DIR} \
		-DCMAKE_BUILD_TYPE=Debug \
		..
```

Lastly, invoke the build and install using `make`:

```
$ make && make install
[...]
[100%] Built target sysrepo-plugin-interfaces
[100%] Built target sysrepo-plugin-interfaces
Install the project...
-- Install configuration: "Debug"
-- Installing: ${SYSREPO_DIR}/bin/sysrepo-plugin-interfaces
-- Set runtime path of "${SYSREPO_DIR}/bin/sysrepo-plugin-interfaces" to ""

-$ cd ..
```

Before using the plugin it is necessary to install relevant YANG modules. For this particular plugin, the following commands need to be invoked:

```
$ cd ${SYSREPO_DIR}/repositories/plugins/sysrepo-plugin-interfaces
$ export LD_LIBRARY_PATH="${SYSREPO_DIR}/lib64;${SYSREPO_DIR}/lib"
$ export PATH="${SYSREPO_DIR}/bin:${PATH}"
```

## Running and Examples

This plugin is installed as the `sysrepo-plugin-interfaces` binary to `${SYSREPO_DIR}/bin/` directory path, if installed with the setup-dev scripts.
If `-DDHCPv6_CLIENT` cmake flag was used the plugin will additionally install the `sysrepo-plugin-dhcpv6-client` binary.

The `IF_PLUGIN_DATA_DIR` environment variable will be used to save internal plugin information that is used to load and store data of some yang nodes.
Currently it only stores data for the `description` node of the `ietf-interfaces` yang model in the file `interface_description`.
This ensures that set descriptions are preserved between plugin runs.

Setting the environment variable `IF_PLUGIN_DATA_DIR` is optional, if not set the default location will be:

```
/usr/local/lib/sysrepo-interfaces-plugin
```

Here is an example of creating a directory and setting the `IF_PLUGIN_DATA_DIR` to its path:

```
mkdir -p /usr/local/lib/sysrepo-interfaces-plugin
export IF_PLUGIN_DATA_DIR=/usr/local/lib/sysrepo-interfaces-plugin
```

YANG models describing interface related functionality have to be installed:

```
$ sysrepoctl -i yang/iana-if-type@2017-01-19.yang
$ sysrepoctl -i yang/ietf-interfaces@2018-02-20.yang
$ sysrepoctl -i yang/ietf-ip@2014-06-16.yang
$ sysrepoctl -i yang/ietf-if-extensions@2020-07-29.yang
$ sysrepoctl -i yang/ieee802-dot1q-types.yang
$ sysrepoctl -i yang/ietf-if-vlan-encapsulation@2020-07-13.yang
```

If dhcpv6-client is used the following YANG models have to be installed:

```
$ sysrepoctl -i ./yang/dhcpv6-client/ietf-dhcpv6-common.yang
$ sysrepoctl -i ./yang/dhcpv6-client/ietf-dhcpv6-client.yang
```

`sub-interfaces` feature has to be enabled:
```
$ sysrepoctl --change ietf-if-extensions --enable-feature sub-interfaces
```

After enabling the feature simply invoke this binary:
```
$ sysrepo-plugin-interfaces
[...]
[INF]: Applying scheduled changes.
[INF]: No scheduled changes.
[INF]: Connection "..." created.
[INF]: Session "..." (user "...", CID "...") created.
[INF]: plugin: start session to startup datastore
[INF]: Session "..." (user "...", CID "...") created.
[INF]: plugin: plugin init done
[...]
```

If the project was built in plugin mode, run the following command:

```
$ sysrepo-plugind -v 4 -d
```

Output from the plugin is expected; the plugin has been initialized with `startup` and `running` datastore contents at `${SYSREPO_DIR}/etc/sysrepo`. We can confirm the contents present in Sysrepo by invoking the following command:

```
$ sysrepocfg -X -d startup -f json -m 'ietf-interfaces'
{
  "ietf-interfaces:interfaces": {
    "interface": [
      {
        "name": "lo",
        "description": "",
        "type": "iana-if-type:softwareLoopback",
        "enabled": false
      },
      {
        "name": "wlan0",
        "description": "",
        "type": "iana-if-type:ethernetCsmacd",
        "enabled": true
      },
      {
        "name": "enp0s31f6",
        "description": "",
        "type": "iana-if-type:ethernetCsmacd",
        "enabled": true
      }
    ]
  }
}

```

Operational state data, as defined by the `ietf-interfaces` module can be accessed with:

```
$ sysrepocfg -X -d operational -f json -x '/ietf-interfaces:interfaces'
{
  "ietf-interfaces:interfaces": {
    "interface": [
      {
        "name": "lo",
        "description": "",
        "oper-status": "unknown",
        "phys-address": "00:00:00:00:00:00",
        "speed": "0",
        "statistics": {
          "discontinuity-time": "2021-03-26T10:14:06Z",
          "in-octets": "78480",
          "in-unicast-pkts": "0",
          "in-broadcast-pkts": "0",
          "in-multicast-pkts": "0",
          "in-discards": 0,
          "in-errors": 0,
          "in-unknown-protos": 0,
          "out-octets": "78480",
          "out-unicast-pkts": "1308",
          "out-broadcast-pkts": "0",
          "out-multicast-pkts": "0",
          "out-discards": 0,
          "out-errors": 0
        }
      },
      {
        "name": "wlan0",
        "description": "",
        "oper-status": "up",
        "phys-address": "68:3e:26:b9:c6:02",
        "speed": "0",
        "statistics": {
          "discontinuity-time": "2021-03-26T10:14:06Z",
          "in-octets": "352955",
          "in-unicast-pkts": "18446744073709548532",
          "in-broadcast-pkts": "0",
          "in-multicast-pkts": "0",
          "in-discards": 0,
          "in-errors": 0,
          "in-unknown-protos": 0,
          "out-octets": "10994",
          "out-unicast-pkts": "96",
          "out-broadcast-pkts": "0",
          "out-multicast-pkts": "7",
          "out-discards": 0,
          "out-errors": 0
        }
      },
      {
        "name": "enp0s31f6",
        "description": "",
        "oper-status": "up",
        "phys-address": "8c:8c:aa:43:cc:f8",
        "speed": "0",
        "statistics": {
          "discontinuity-time": "2021-03-26T10:14:06Z",
          "in-octets": "730093500",
          "in-unicast-pkts": "18446744073709551445",
          "in-broadcast-pkts": "0",
          "in-multicast-pkts": "7069",
          "in-discards": 6809,
          "in-errors": 0,
          "in-unknown-protos": 0,
          "out-octets": "25237778",
          "out-unicast-pkts": "144804",
          "out-broadcast-pkts": "0",
          "out-multicast-pkts": "75",
          "out-discards": 0,
          "out-errors": 0
        }
      }
    ]
  }
}
```

## YANG Overview

The `ietf-interfaces` YANG module with the `if` prefix consists of the following `container` paths:

* `/ietf-interfaces:interfaces` — configuration and operational data for the system

The following leaves are for configuration:

* `name`
* `description`
* `type`
* `enabled`
* `link-up-down-trap-enable`

    note: currently only the following network interface types (`type` node) from IANA Interface Type YANG Module are supported:

    * ethernetCsmacd
    * softwareLoopback
    * l2vlan
    * other - dummy interface type

The following items are operational state data:

* `admin-status`
* `oper-status`
* `last-change?`
* `if-index`
* `phys-address?`
* `higher-layer-if*`
* `lower-layer-if*`
* `speed`
* `statistics`:


   -  `discontinuity-time`
   -  `in-octets`
   -  `in-unicast-pkts`
   -  `in-broadcast-pkts`
   -  `in-multicast-pkts`
   -  `in-discards`
   -  `in-errors`
   -  `in-unknown-protos`
   -  `out-octets`
   -  `out-unicast-pkts`
   -  `out-broadcast-pkts`
   -  `out-multicast-pkts`
   -  `out-discards`
   -  `out-errors`



* `interfaces-state` is obsolete

The `ietf-dhcpv6-client` YANG module consists of the following `container` paths:

* `/ietf-dhcpv6-client:dhcpv6-client` — configuration and operational data for the system

The following leaves are for configuration:

* `if-name`
* `enabled`
* `duid`
* `enabled`

The following groupings are also used for configuration (`dhc6` prefix is used to denote `ietf-dhcpv6-common` YANG model):

* `option-request-option-group`
* `dhc6:rapid-commit-option-group`
* `user-class-option-group`
* `vendor-class-option-group`
* `dhc6:vendor-specific-information-option-group`
* `dhc6:reconfigure-accept-option-group`

## Testing
The `tests/manual_tests.md` file contains a list of sysrepo commands that can be used to test
the currently implemented features of the plugin.


## Nodes that are currently implemented

- DONE - nodes are implemented and a value is provided if such information can be retrieved
- NA - node is not implemented
- IN PROGRESS - implementation in progress
```
module: ietf-interfaces
   +--rw interfaces
      +--rw interface* [name]
         +--rw name                        string                     DONE
         +--rw description?                string                     DONE
         +--rw type                        identityref                DONE
         +--rw enabled?                    boolean                    DONE
         +--rw link-up-down-trap-enable?   enumeration {if-mib}?      NA
         +--ro admin-status                enumeration {if-mib}?      NA
         +--ro oper-status                 enumeration                DONE
         +--ro last-change?                yang:date-and-time         DONE
         +--ro if-index                    int32 {if-mib}?            DONE
         +--ro phys-address?               yang:phys-address          DONE
         +--ro higher-layer-if*            interface-ref              DONE
         +--ro lower-layer-if*             interface-ref              DONE
         +--ro speed?                      yang:gauge64               DONE
         +--ro statistics
            +--ro discontinuity-time    yang:date-and-time            IN PROGRESS
            +--ro in-octets?            yang:counter64                DONE
            +--ro in-unicast-pkts?      yang:counter64                DONE
            +--ro in-broadcast-pkts?    yang:counter64                DONE
            +--ro in-multicast-pkts?    yang:counter64                DONE
            +--ro in-discards?          yang:counter32                DONE
            +--ro in-errors?            yang:counter32                DONE
            +--ro in-unknown-protos?    yang:counter32                DONE
            +--ro out-octets?           yang:counter64                DONE
            +--ro out-unicast-pkts?     yang:counter64                DONE
            +--ro out-broadcast-pkts?   yang:counter64                DONE
            +--ro out-multicast-pkts?   yang:counter64                DONE
            +--ro out-discards?         yang:counter32                DONE
            +--ro out-errors?           yang:counter32                DONE


module: ietf-if-extensions
   augment /if:interfaces/if:interface:
      +--rw carrier-delay {carrier-delay}?
      |  +--rw down?                  uint32                          NA
      |  +--rw up?                    uint32                          NA
      |  +--ro carrier-transitions?   yang:counter64                  NA
      |  +--ro timer-running?         enumeration                     NA
      +--rw dampening! {dampening}?
      |  +--rw half-life?           uint32                            NA
      |  +--rw reuse?               uint32                            NA
      |  +--rw suppress?            uint32                            NA
      |  +--rw max-suppress-time?   uint32                            NA
      |  +--ro penalty?             uint32                            NA
      |  +--ro suppressed?          boolean                           NA
      |  +--ro time-remaining?      uint32                            NA
      +--rw encapsulation
      |  +--rw (encaps-type)?                                         DONE
      +--rw loopback?          identityref {loopback}?                NA
      +--rw max-frame-size?    uint32 {max-frame-size}?               NA
      +--ro forwarding-mode?   identityref                            NA
   augment /if:interfaces/if:interface:
      +--rw parent-interface    if:interface-ref {sub-interfaces}?    DONE
   augment /if:interfaces/if:interface/if:statistics:
      +--ro in-discard-unknown-encaps?   yang:counter64               NA
            {sub-interfaces}?


module: ietf-if-vlan-encapsulation
   augment /if:interfaces/if:interface/if-ext:encapsulation
            /if-ext:encaps-type:
      +--:(dot1q-vlan)
         +--rw dot1q-vlan                                             QinQ not yet supported, second-tag is ignored and normal vlan is created
            +--rw outer-tag
            |  +--rw tag-type    dot1q-tag-type                       IN PROGRESS (currently only normal vlan supported)
            |  +--rw vlan-id     vlanid                               IN PROGRESS (currently only normal vlan supported)
            +--rw second-tag!
               +--rw tag-type    dot1q-tag-type                       IN PROGRESS (currently only normal vlan supported)
               +--rw vlan-id     vlanid                               IN PROGRESS (currently only normal vlan supported)

module: ietf-ip
   augment /if:interfaces/if:interface:
      +--rw ipv4!
      |  +--rw enabled?      boolean                                  IN PROGRESS
      |  +--rw forwarding?   boolean                                  DONE
      |  +--rw mtu?          uint16                                   DONE
      |  +--rw address* [ip]
      |  |  +--rw ip               inet:ipv4-address-no-zone          DONE
      |  |  +--rw (subnet)
      |  |  |  +--:(prefix-length)
      |  |  |  |  +--rw prefix-length?   uint8                        DONE
      |  |  |  +--:(netmask)
      |  |  |     +--rw netmask?         yang:dotted-quad
      |  |  |             {ipv4-non-contiguous-netmasks}?             DONE
      |  |  +--ro origin?          ip-address-origin                  NA
      |  +--rw neighbor* [ip]
      |     +--rw ip                    inet:ipv4-address-no-zone     DONE
      |     +--rw link-layer-address    yang:phys-address             DONE
      |     +--ro origin?               neighbor-origin               NA
      +--rw ipv6!
         +--rw enabled?                     boolean                   DONE
         +--rw forwarding?                  boolean                   DONE
         +--rw mtu?                         uint32                    DONE
         +--rw address* [ip]
         |  +--rw ip               inet:ipv6-address-no-zone          DONE
         |  +--rw prefix-length    uint8                              DONE
         |  +--ro origin?          ip-address-origin                  NA
         |  +--ro status?          enumeration                        NA
         +--rw neighbor* [ip]
         |  +--rw ip                    inet:ipv6-address-no-zone     DONE
         |  +--rw link-layer-address    yang:phys-address             DONE
         |  +--ro origin?               neighbor-origin               NA
         |  +--ro is-router?            empty                         NA
         |  +--ro state?                enumeration                   NA
         +--rw dup-addr-detect-transmits?   uint32                    NA
         +--rw autoconf
            +--rw create-global-addresses?        boolean             NA
            +--rw create-temporary-addresses?     boolean             NA
            |       {ipv6-privacy-autoconf}?
            +--rw temporary-valid-lifetime?       uint32              NA
            |       {ipv6-privacy-autoconf}?
            +--rw temporary-preferred-lifetime?   uint32
                  {ipv6-privacy-autoconf}?                            NA
```

DHCPv6 client

```
module: ietf-dhcpv6-client
  +--rw dhcpv6-client
        +--rw enabled?                     boolean                      IN PROGRESS
        +--rw duid?                        dhc6:duid                    DONE
        +--rw client-configured-options
        |  +--rw option-request-option
        |  |  +--rw oro-option*   uint16                                DONE
        |  +--rw user-class-option!
        |  |  +--rw user-class-data-instance* [user-class-data-id]
        |  |     +--rw user-class-data-id    uint8                      NA (only string value is used)
        |  |     +--rw user-class-data?      string                     DONE
        |  +--rw vendor-class-option                                    NA (not supported by dhclient)
        |     +--rw vendor-class-option-instances* [enterprise-number]
        |        +--rw enterprise-number            uint32
        |        +--rw vendor-class-data-element* [vendor-class-data-id]
        |           +--rw vendor-class-data-id    uint8
        |           +--rw vendor-class-data?      string
        +--rw ia-na* [ia-id] {non-temporary-address}?                   IN PROGRESS
        |  +--rw ia-id            uint32
        |  +--rw ia-na-options
        |  +--ro lease-state
        |     +--ro ia-na-address?        inet:ipv6-address
        |     +--ro lease-t1?             dhc6:timer-seconds32
        |     +--ro lease-t2?             dhc6:timer-seconds32
        |     +--ro preferred-lifetime?   dhc6:timer-seconds32
        |     +--ro valid-lifetime?       dhc6:timer-seconds32
        |     +--ro allocation-time?      yang:date-and-time
        |     +--ro last-renew-rebind?    yang:date-and-time
        |     +--ro server-duid?          dhc6:duid
        +--rw ia-ta* [ia-id] {temporary-address}?                       IN PROGRESS
        |  +--rw ia-id            uint32
        |  +--rw ia-ta-options
        |  +--ro lease-state
        |     +--ro ia-ta-address?        inet:ipv6-address
        |     +--ro preferred-lifetime?   dhc6:timer-seconds32
        |     +--ro valid-lifetime?       dhc6:timer-seconds32
        |     +--ro allocation-time?      yang:date-and-time
        |     +--ro last-renew-rebind?    yang:date-and-time
        |     +--ro server-duid?          dhc6:duid
        +--rw ia-pd* [ia-id] {prefix-delegation}?                       IN PROGRESS
        |  +--rw ia-id            uint32
        |  +--rw ia-pd-options
        |  +--ro lease-state
        |     +--ro ia-pd-prefix?         inet:ipv6-prefix
        |     +--ro lease-t1?             dhc6:timer-seconds32
        |     +--ro lease-t2?             dhc6:timer-seconds32
        |     +--ro preferred-lifetime?   dhc6:timer-seconds32
        |     +--ro valid-lifetime?       dhc6:timer-seconds32
        |     +--ro allocation-time?      yang:date-and-time
        |     +--ro last-renew-rebind?    yang:date-and-time
        |     +--ro server-duid?          dhc6:duid
        +--ro solicit-count?               uint32                       IN PROGRESS
        +--ro advertise-count?             uint32                       IN PROGRESS
        +--ro request-count?               uint32                       IN PROGRESS
        +--ro confirm-count?               uint32                       IN PROGRESS
        +--ro renew-count?                 uint32                       IN PROGRESS
        +--ro rebind-count?                uint32                       IN PROGRESS
        +--ro reply-count?                 uint32                       IN PROGRESS
        +--ro release-count?               uint32                       IN PROGRESS
        +--ro decline-count?               uint32                       IN PROGRESS
        +--ro reconfigure-count?           uint32                       IN PROGRESS
        +--ro information-request-count?   uint32                       IN PROGRESS

  notifications:                                                        NA (currently not in progress, planned for future)
    +---n invalid-ia-address-detected {non-temporary-address or temporary-address}?
    |  +--ro ia-id                 uint32
    |  +--ro ia-na-t1-timer?       uint32
    |  +--ro ia-na-t2-timer?       uint32
    |  +--ro invalid-address?      inet:ipv6-address
    |  +--ro preferred-lifetime?   uint32
    |  +--ro valid-lifetime?       uint32
    |  +--ro ia-options?           binary
    |  +--ro description?          string
    +---n transmission-failed
    |  +--ro failure-type    enumeration
    |  +--ro description?    string
    +---n unsuccessful-status-code
    |  +--ro server-duid    dhc6:duid
    +---n server-duid-changed {non-temporary-address or prefix-delegation or temporary-address}?
       +--ro new-server-duid         dhc6:duid
       +--ro previous-server-duid    dhc6:duid
       +--ro lease-ia-na?            -> ../../dhcpv6-client/client-if/ia-na/ia-id {non-temporary-address}?
       +--ro lease-ia-ta?            -> ../../dhcpv6-client/client-if/ia-ta/ia-id {temporary-address}?
       +--ro lease-ia-pd?            -> ../../dhcpv6-client/client-if/ia-pd/ia-id {prefix-delegation}?
```