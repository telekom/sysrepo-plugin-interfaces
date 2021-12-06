## Development Setup

Setup the development environment using the provided [`setup-dev-sysrepo`](https://github.com/sartura/setup-dev-sysrepo) scripts. This will build all the necessary components.

Subsequent rebuilds of the plugins may be done by navigating to the plugin source directory and executing:

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

Before using the plugins it is necessary to install relevant YANG modules. For these particular plugins, the following commands need to be invoked:

```
$ cd ${SYSREPO_DIR}/repositories/plugins/sysrepo-plugin-interfaces
$ export LD_LIBRARY_PATH="${SYSREPO_DIR}/lib64;${SYSREPO_DIR}/lib"
$ export PATH="${SYSREPO_DIR}/bin:${PATH}"
```

## Running and Examples

### Interfaces plugin

This plugin is installed as the `sysrepo-plugin-interfaces` binary to `${SYSREPO_DIR}/bin/` directory path, if installed with the setup-dev scripts.

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
$ sysrepoctl -i yang/ietf-ip@2018-02-22.yang
$ sysrepoctl -i yang/ietf-if-extensions@2020-07-29.yang
$ sysrepoctl -i yang/ieee802-dot1q-types.yang
$ sysrepoctl -i yang/ietf-if-vlan-encapsulation@2020-07-13.yang
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

### Routing plugin
This plugin is installed as the `sysrepo-plugin-routing` binary to `${SYSREPO_DIR}/bin/` directory path, if installed with the setup-dev scripts.

The `ROUTING_PLUGIN_DATA_DIR` environment variable will be used to save internal plugin information that is used to load and store data of some yang nodes.
Currently it stores data for the `description` of RIB nodes of the `ietf-routing` yang model in the file `ribs_map`.
This ensures that set descriptions are preserved between plugin runs.

Here is an example of creating a directory and setting the `ROUTING_PLUGIN_DATA_DIR` to its path:

```
mkdir -p /usr/local/lib/sysrepo-routing-plugin
export ROUTING_PLUGIN_DATA_DIR=/usr/local/lib/sysrepo-routing-plugin
```

YANG models describing interface related functionality have to be installed:

```
$ sysrepoctl -i ./yang/ietf-ip@2018-02-22.yang
$ sysrepoctl -i ./yang/ietf-routing@2018-03-13.yang
$ sysrepoctl -i ./yang/ietf-ipv4-unicast-routing@2018-03-13.yang
$ sysrepoctl -i ./yang/ietf-ipv6-unicast-routing@2018-03-13.yang
$ sysrepoctl -i ./yang/ietf-ipv4-router-advertisements@2018-03-13.yang
```

After enabling the feature simply invoke this binary:
```
$ sysrepo-plugin-routing
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

Output from the plugin is expected; the plugin has been initialized with `startup` and `running` datastore contents at `${SYSREP
O_DIR}/etc/sysrepo`. We can confirm the contents present in Sysrepo by invoking the following command:

```
$ sysrepocfg -X -d startup -f json -m ietf-routing
{
  "ietf-routing:routing": {
    "control-plane-protocols": {
      "control-plane-protocol": [
        {
          "type": "ietf-routing:direct",
          "name": "unspec",
          "description": "unspecified protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "redirect",
          "description": "redirect protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "kernel",
          "description": "kernel protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "boot",
          "description": "boot protocol"
        },
        {
          "type": "ietf-routing:static",
          "name": "static",
          "description": "static protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "gated",
          "description": "gated protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "ra",
          "description": "ra protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "mrt",
          "description": "mrt protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "zebra",
          "description": "zebra protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "bird",
          "description": "bird protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "dnrouted",
          "description": "dnrouted protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "xorp",
          "description": "xorp protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "ntk",
          "description": "ntk protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "dhcp",
          "description": "dhcp protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "0x11",
          "description": "mrouted protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "babel",
          "description": "babel protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "bgp",
          "description": "bgp protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "isis",
          "description": "isis protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "ospf",
          "description": "ospf protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "rip",
          "description": "rip protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "eigrp",
          "description": "eigrp protocol"
        }
      ]
    },
    "ribs": {
      "rib": [
        {
          "name": "ipv4-main",
          "address-family": "ietf-routing:ipv4",
          "description": "main routing table - normal routing table containing all non-policy routes (ipv4 only)"
        },
        {
          "name": "ipv4-local",
          "address-family": "ietf-routing:ipv4",
          "description": "local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv4 only)"
        },
        {
          "name": "ipv6-main",
          "address-family": "ietf-routing:ipv6",
          "description": "main routing table - normal routing table containing all non-policy routes (ipv6 only)"
        },
        {
          "name": "ipv6-local",
          "address-family": "ietf-routing:ipv6",
          "description": "local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv6 only)"
        }
      ]
    }
  }
}
```

Operational state data, can be accessed with:

```
sysrepocfg -d operational -X -f json -m ietf-routing 
{
  "ietf-routing:routing": {
    "interfaces": {
      "interface": [
        "lo",
        "enp1s0"
      ]
    },
    "control-plane-protocols": {
      "control-plane-protocol": [
        {
          "type": "ietf-routing:direct",
          "name": "unspec",
          "description": "unspecified protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "redirect",
          "description": "redirect protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "kernel",
          "description": "kernel protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "boot",
          "description": "boot protocol"
        },
        {
          "type": "ietf-routing:static",
          "name": "static",
          "description": "static protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "gated",
          "description": "gated protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "ra",
          "description": "ra protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "mrt",
          "description": "mrt protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "zebra",
          "description": "zebra protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "bird",
          "description": "bird protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "dnrouted",
          "description": "dnrouted protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "xorp",
          "description": "xorp protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "ntk",
          "description": "ntk protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "dhcp",
          "description": "dhcp protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "0x11",
          "description": "mrouted protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "babel",
          "description": "babel protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "bgp",
          "description": "bgp protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "isis",
          "description": "isis protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "ospf",
          "description": "ospf protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "rip",
          "description": "rip protocol"
        },
        {
          "type": "ietf-routing:direct",
          "name": "eigrp",
          "description": "eigrp protocol"
        }
      ]
    },
    "ribs": {
      "rib": [
        {
          "name": "ipv4-main",
          "routes": {
            "route": [
              {
                "route-preference": 100,
                "next-hop": {
                  "outgoing-interface": "enp1s0",
                  "ietf-ipv4-unicast-routing:next-hop-address": "10.0.2.2"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "0.0.0.0/0"
              },
              {
                "route-preference": 100,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "10.0.2.0/24"
              },
              {
                "route-preference": 1000,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "169.254.0.0/16"
              }
            ]
          }
        },
        {
          "name": "ipv4-local",
          "routes": {
            "route": [
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "10.0.2.0/32"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "10.0.2.15/32"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "10.0.2.255/32"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "lo"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "127.0.0.0/32"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "lo"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "127.0.0.0/8"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "lo"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "127.0.0.1/32"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "lo"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv4-unicast-routing:destination-prefix": "127.255.255.255/32"
              }
            ]
          }
        },
        {
          "name": "ipv6-main",
          "routes": {
            "route": [
              {
                "route-preference": 256,
                "next-hop": {
                  "outgoing-interface": "lo"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "::1/128"
              },
              {
                "route-preference": 100,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "fe80::/64"
              },
              {
                "route-preference": 100,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "fec0::/64"
              },
              {
                "route-preference": 20100,
                "next-hop": {
                  "outgoing-interface": "enp1s0",
                  "ietf-ipv6-unicast-routing:next-hop-address": "fe80::2"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "::/0"
              }
            ]
          }
        },
        {
          "name": "ipv6-local",
          "routes": {
            "route": [
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "lo"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "::1/128"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "fe80::7987:cb97:60d:e11e/128"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "fec0::51a3:301:b5b7:23bb/128"
              },
              {
                "route-preference": 0,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "fec0::8357:aadf:d46:f571/128"
              },
              {
                "route-preference": 256,
                "next-hop": {
                  "outgoing-interface": "enp1s0"
                },
                "source-protocol": "ietf-routing:direct",
                "active": [null],
                "ietf-ipv6-unicast-routing:destination-prefix": "ff00::/8"
              }
            ]
          }
        }
      ]
    }
  }
}
```

## YANG Overview

### Interfaces plugin

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

### Routing plugin

The `ietf-routing` YANG module with the `rt` prefix consists of the following `container` paths:

* `/ietf-routing:routing` - configuration parameters for the routing subsystem

The following leaves are configuration data:

* `control-planeprotocols`
* `ribs`

Operational state data consists of:
* `ribs`
* `interfaces`

* `routing-state` is obsolete

## Testing
The `tests/interfaces/manual_tests.md` and `tests/routing/manual_tests.md` files
contain a list of sysrepo commands that can be used to test
the currently implemented features of the plugins.

## Nodes that are currently implemented

- DONE - nodes are implemented and a value is provided if such information can be retrieved
- NA - node is not implemented
- IN PROGRESS - implementation in progress

### Interfaces plugin

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

### Routing plugin
```
module: ietf-routing
  +--rw routing
  |  +--rw router-id?                 yang:dotted-quad {router-id}?		NA
  |  +--ro interfaces
  |  |  +--ro interface*   if:interface-ref					DONE
  |  +--rw control-plane-protocols
  |  |  +--rw control-plane-protocol* [type name]
  |  |     +--rw type             identityref					DONE
  |  |     +--rw name             string					DONE
  |  |     +--rw description?     string					DONE
  |  |     +--rw static-routes							IN PROGRESS
  |  +--rw ribs
  |     +--rw rib* [name]
  |        +--rw name              string					DONE
  |        +--rw address-family    identityref					DONE
  |        +--ro default-rib?      boolean {multiple-ribs}?			NA
  |        +--ro routes
  |        |  +--ro route* []
  |        |     +--ro route-preference?   route-preference			DONE
  |        |     +--ro next-hop							DONE
  |        |     |  +--ro (next-hop-options)
  |        |     |     +--:(simple-next-hop)
  |        |     |     |  +--ro outgoing-interface?   if:interface-ref		DONE
  |        |     |     +--:(special-next-hop)
  |        |     |     |  +--ro special-next-hop?     enumeration		DONE
  |        |     |     +--:(next-hop-list)
  |        |     |        +--ro next-hop-list
  |        |     |           +--ro next-hop* []
  |        |     |              +--ro outgoing-interface?   if:interface-ref	DONE
  |        |     +--ro source-protocol     identityref				DONE
  |        |     +--ro active?             empty				DONE
  |        |     +--ro last-updated?       yang:date-and-time			NA
  |        +---x active-route							NA
  |        |  +--ro output
  |        |     +--ro route
  |        |        +--ro next-hop
  |        |        |  +--ro (next-hop-options)
  |        |        |     +--:(simple-next-hop)
  |        |        |     |  +--ro outgoing-interface?   if:interface-ref
  |        |        |     +--:(special-next-hop)
  |        |        |     |  +--ro special-next-hop?     enumeration
  |        |        |     +--:(next-hop-list)
  |        |        |        +--ro next-hop-list
  |        |        |           +--ro next-hop* []
  |        |        |              +--ro outgoing-interface?   if:interface-ref
  |        |        +--ro source-protocol    identityref
  |        |        +--ro active?            empty
  |        |        +--ro last-updated?      yang:date-and-time
  |        +--rw description?      string					DONE

module: ietf-ipv4-unicast-routing

  augment /rt:routing/rt:ribs/rt:rib/rt:routes/rt:route:
    +--ro destination-prefix?   inet:ipv4-prefix				DONE
  augment /rt:routing/rt:ribs/rt:rib/rt:routes/rt:route/rt:next-hop/rt:next-hop-options/rt:simple-next-hop:
    +--ro next-hop-address?   inet:ipv4-address					DONE
  augment /rt:routing/rt:ribs/rt:rib/rt:routes/rt:route/rt:next-hop/rt:next-hop-options/rt:next-hop-list/rt:next-hop-list/rt:next-hop:
    +--ro address?   inet:ipv4-address						DONE
  augment /rt:routing/rt:ribs/rt:rib/rt:active-route/rt:input:
    +---w destination-address?   inet:ipv4-address				NA
  augment /rt:routing/rt:ribs/rt:rib/rt:active-route/rt:output/rt:route:
    +-- destination-prefix?   inet:ipv4-prefix					NA
  augment /rt:routing/rt:ribs/rt:rib/rt:active-route/rt:output/rt:route/rt:next-hop/rt:next-hop-options/rt:simple-next-hop:
    +-- next-hop-address?   inet:ipv4-address					DONE
  augment /rt:routing/rt:ribs/rt:rib/rt:active-route/rt:output/rt:route/rt:next-hop/rt:next-hop-options/rt:next-hop-list/rt:next-hop-list/rt:next-hop:
    +-- next-hop-address?   inet:ipv4-address					DONE
  augment /rt:routing/rt:control-plane-protocols/rt:control-plane-protocol/rt:static-routes:
    +--rw ipv4									IN PROGRESS
       +--rw route* [destination-prefix]
          +--rw destination-prefix    inet:ipv4-prefix
          +--rw description?          string
          +--rw next-hop
             +--rw (next-hop-options)
                +--:(simple-next-hop)
                |  +--rw outgoing-interface?   if:interface-ref
                |  +--rw next-hop-address?     inet:ipv4-address
                +--:(special-next-hop)
                |  +--rw special-next-hop?     enumeration
                +--:(next-hop-list)
                   +--rw next-hop-list
                      +--rw next-hop* [index]
                         +--rw index                 string
                         +--rw outgoing-interface?   if:interface-ref
                         +--rw next-hop-address?     inet:ipv4-address

module: ietf-ipv6-unicast-routing

  augment /rt:routing/rt:ribs/rt:rib/rt:routes/rt:route:
    +--ro destination-prefix?   inet:ipv6-prefix				DONE
  augment /rt:routing/rt:ribs/rt:rib/rt:routes/rt:route/rt:next-hop/rt:next-hop-options/rt:simple-next-hop:
    +--ro next-hop-address?   inet:ipv6-address					DONE
  augment /rt:routing/rt:ribs/rt:rib/rt:routes/rt:route/rt:next-hop/rt:next-hop-options/rt:next-hop-list/rt:next-hop-list/rt:next-hop:
    +--ro address?   inet:ipv6-address						DONE
  augment /rt:routing/rt:ribs/rt:rib/rt:active-route/rt:input:
    +---w destination-address?   inet:ipv6-address				NA
  augment /rt:routing/rt:ribs/rt:rib/rt:active-route/rt:output/rt:route:
    +-- destination-prefix?   inet:ipv6-prefix					NA
  augment /rt:routing/rt:ribs/rt:rib/rt:active-route/rt:output/rt:route/rt:next-hop/rt:next-hop-options/rt:simple-next-hop:
    +-- next-hop-address?   inet:ipv6-address					DONE
  augment /rt:routing/rt:ribs/rt:rib/rt:active-route/rt:output/rt:route/rt:next-hop/rt:next-hop-options/rt:next-hop-list/rt:next-hop-list/rt:next-hop:
    +-- next-hop-address?   inet:ipv6-address					DONE
  augment /rt:routing/rt:control-plane-protocols/rt:control-plane-protocol/rt:static-routes:
    +--rw ipv6									IN PROGRESS
       +--rw route* [destination-prefix]
          +--rw destination-prefix    inet:ipv6-prefix
          +--rw description?          string
          +--rw next-hop
             +--rw (next-hop-options)
                +--:(simple-next-hop)
                |  +--rw outgoing-interface?   if:interface-ref
                |  +--rw next-hop-address?     inet:ipv6-address
                +--:(special-next-hop)
                |  +--rw special-next-hop?     enumeration
                +--:(next-hop-list)
                   +--rw next-hop-list
                      +--rw next-hop* [index]
                         +--rw index                 string
                         +--rw outgoing-interface?   if:interface-ref
                         +--rw next-hop-address?     inet:ipv6-address
  augment /if:interfaces/if:interface/ip:ipv6:					NA
    +--rw ipv6-router-advertisements
       +--rw send-advertisements?    boolean
       +--rw max-rtr-adv-interval?   uint16
       +--rw min-rtr-adv-interval?   uint16
       +--rw managed-flag?           boolean
       +--rw other-config-flag?      boolean
       +--rw link-mtu?               uint32
       +--rw reachable-time?         uint32
       +--rw retrans-timer?          uint32
       +--rw cur-hop-limit?          uint8
       +--rw default-lifetime?       uint16
       +--rw prefix-list
          +--rw prefix* [prefix-spec]
             +--rw prefix-spec                 inet:ipv6-prefix
             +--rw (control-adv-prefixes)?
                +--:(no-advertise)
                |  +--rw no-advertise?         empty
                +--:(advertise)
                   +--rw valid-lifetime?       uint32
                   +--rw on-link-flag?         boolean
                   +--rw preferred-lifetime?   uint32
                   +--rw autonomous-flag?      boolean
```
