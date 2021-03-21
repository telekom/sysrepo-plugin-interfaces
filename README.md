# Sysrepo interfaces plugin

## Introduction

This Sysrepo plugin is responsible for bridging a Linux system and Sysrepo/YANG datastore network interfaces configuration.

## Dependencies

Other than libyang and Sysrepo, there are some other dependencies required to build and use this plugin.
These include pthreads and the libnl (https://www.infradead.org/~tgr/libnl/) netlink library.

## Build

This section describes how to build the plugin on hosts that have Sysrepo installed. This
includes standard Linux machines and docker images with Sysrepo, Netopeer and other required dependencies.

First, clone the repo:
```
$ git clone git@lab.sartura.hr:sysrepo/sysrepo-plugin-interfaces.git
```

Next, make a build directory and prepare the build scripts:

```
$ mkdir build && cd build
$ cmake ..
```

This will build the plugin as a standalone executable which can be run without `sysrepo-plugind`

To build the project in plugin mode, run the following instead:

```
$ cmake -DPLUGIN=ON ..
```

After that, run `make` and `make install`

Finally to run the plugin, YANG models describing interface related functionality have to be installed.

```
$ sysrepoctl -i ../yang/iana-if-type@2017-01-19.yang
$ sysrepoctl -i ../yang/ietf-interfaces@2018-02-20.yang
$ sysrepoctl -i ../yang/ietf-interfaces-common@2019-03-05.yang
$ sysrepoctl -i ../yang/ieee802-dot1q-types.yang 
$ sysrepoctl -i ../yang/ietf-if-vlan-encapsulation@2020-07-13.yang
```

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

$ sysrepoctl -i ./yang/ietf-interfaces@2018-02-20.yang

$ sysrepoctl -i ./yang/iana-if-type@2017-01-19.yang
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

## Running and Examples

This plugin is installed as the `sysrepo-plugin-interfaces` binary to `${SYSREPO_DIR}/bin/` directory path.
After loading the example simply invoke this binary, making sure that the environment variables are set correctly:

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

Output from the plugin is expected; the plugin has been initialized with `startup` and `running` datastore contents at `${SYSREPO_DIR}/etc/sysrepo`. We can confirm the contents present in Sysrepo by invoking the following command:

```
$ sysrepocfg -X -d startup -f json -m 'ietf-interfaces'
{
	# TODO
}

```

Operational state data, as defined by the `ietf-interfaces` module can be accessed with:

```
sysrepocfg -X -d operational -f json -x '/ietf-interfaces:interfaces'
{
	# TODO
}
```
