<h1 align="center">
    Sysrepo Plugin Interfaces
</h1>

<p align="center">
    <a href="/../../commits/" title="Last Commit"><img src="https://img.shields.io/github/last-commit/telekom/sysrepo-plugin-interfaces?style=flat"></a>
    <a href="https://github.com/andreiosg/sysrepo-plugin-interfaces/actions/workflows/ci.yml" title="sysrepo-plugin-interfaces CI"><img src="https://github.com/andreiosg/sysrepo-plugin-interfaces/workflows/sysrepo-plugin-interfaces%20CI/badge.svg"></a>
    <a href="https://github.com/andreiosg/sysrepo-plugin-interfaces/actions/workflows/ci.yml" title="clang-format all"><img src="https://github.com/andreiosg/sysrepo-plugin-interfaces/workflows/clang-format%20all/badge.svg"></a>
    <a href="/../../issues" title="Open Issues"><img src="https://img.shields.io/github/issues/telekom/sysrepo-plugin-interfaces?style=flat"></a>
    <a href="./LICENSE" title="License"><img src="https://img.shields.io/badge/License-BSD%203--Clause-blue.svg?style=flat"></a>
</p>

<p align="center">
  <a href="#development">Development</a> •
  <a href="#documentation">Documentation</a> •
  <a href="#support-and-feedback">Support</a> •
  <a href="#how-to-contribute">Contribute</a> •
  <a href="#contributors">Contributors</a> •
  <a href="#licensing">Licensing</a>
</p>

The goal of this project is to provide a method of configuring networking on generic Linux systems using the Sysrepo NETCONF server implementation. 

## About this component

This repository contains several Sysrepo plugins related to newtork management.

The first, sysrepo-plugin-interfaces is based on the `ietf-interfaces` YANG module which contains
"a collection of YANG definitions for managing network interfaces".
More information about the specific YANG module can be found in
[RFC 7223: A YANG Data Model for Interface Management](https://datatracker.ietf.org/doc/html/rfc7223).

The second, sysrepo-plugin-routing is based on the `ietf-routing` YANG module which contains
"generic components of a routing data model" and `ietf-ipv4-unicast-routing` and `ietf-ipv6-unicast-routing`
which augment `ietf-routing` with IPv4 and IPv6 specific data.
More information about `ietf-routing` is available in [RFC 8022: A YANG Data Model for Routing Management](https://datatracker.ietf.org/doc/html/rfc8022)

## Development

Besides the usual C development environment, the following additional dependencies are required:

* libyang
* sysrepo
* pthreads
* netlink
* [sysrepo-plugins-common library](https://github.com/telekom/sysrepo-plugins-common)

#### Build

First clone the repository:

```
$ git clone https://github.com/telekom/sysrepo-plugin-interfaces
$ git submodule init
$ git submodule update
```

Next, create a build directory and generate the build recipes using CMake:

```
$ mkdir build
$ cd build
$ cmake ..
```

By default both plugins will be built, but either of them can be disabled by setting the appropriate CMake plugin option to `OFF`.
For example, to build only the routing plugin the following command should be executed instead:
```
$ cmake -DINTERFACES_PLUGIN=OFF ..
```

Lastly, invoke the build and install using `make`:

```
$ make -j$(nproc)
```

### Build artifacts

Plugin will be built as a standalone application and also as a `sysrepo-plugind` module. For example, for the ietf-interfaces plugin there are two build artifacts:
- **ietf-interfaces-plugin**: standalone application
- **libsrplg-ietf-interfaces.so**: `sysrepo-plugind` module which exposes the plugin init and cleanup callbacks and can be installed by invoking the following command: `sysrepo-plugind -P libsrplg-ietf-interfaces.so`

### Sysrepo/YANG requirements

The plugins require several YANG modules to be loaded into the Sysrepo datastore and several features need to be enabled.
For the interfaces plugin this can be achieved by invoking the following commands:

```
$ sysrepoctl -i ./yang/ietf-interfaces@2018-02-20.yang
$ sysrepoctl -i ./yang/iana-if-type@2017-01-19.yang
$ sysrepoctl -i ./yang/ietf-ip@2018-02-22.yang
$ sysrepoctl -i ./yang/ietf-if-extensions@2020-07-29.yang
$ sysrepoctl -i ./yang/ieee802-dot1q-types.yang
$ sysrepoctl -i ./yang/ietf-if-vlan-encapsulation@2020-07-13.yang
$ sysrepoctl --change ietf-interfaces --enable-feature if-mib
$ sysrepoctl --change ietf-if-extensions --enable-feature sub-interfaces
```

For the routing plugin, the following models have to be installed:
```
$ sysrepoctl -i ./yang/ietf-interfaces@2018-02-20.yang
$ sysrepoctl -i ./yang/iana-if-type@2017-01-19.yang
$ sysrepoctl -i ./yang/ietf-ip@2018-02-22.yang
$ sysrepoctl -i ./yang/ietf-routing@2018-03-13.yang
$ sysrepoctl -i ./yang/ietf-ipv4-unicast-routing@2018-03-13.yang
$ sysrepoctl -i ./yang/ietf-ipv6-unicast-routing@2018-03-13.yang -s ./yang
```

## Code of Conduct

This project has adopted the [Contributor Covenant](https://www.contributor-covenant.org/) in version 2.0 as our code of conduct. Please see the details in our [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md). All contributors must abide by the code of conduct.

## Working Language

We decided to apply _English_ as the primary project language.  

Consequently, all content will be made available primarily in English. We also ask all interested people to use English as language to create issues, in their code (comments, documentation etc.) and when you send requests to us. The application itself and all end-user facing content will be made available in other languages as needed.

## Documentation

The full documentation for the Sysrepo interfaces and routing plugins can be found in the [documentation directory](../docs).

## Support and Feedback

The following channels are available for discussions, feedback, and support requests:

| Type               | Channel                                                                                                                                                                                            |
| ------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Issues**         | <a href="/../../issues/new/choose" title="General Discussion"><img src="https://img.shields.io/github/issues/telekom/sysrepo-plugin-interfaces?style=flat-square"></a> </a>                        |
| **Other Requests** | <a href="mailto:opensource@telekom.de" title="Email Open Source Team"><img src="https://img.shields.io/badge/email-Open%20Source%20Team-green?logo=mail.ru&style=flat-square&logoColor=white"></a> |

## How to Contribute

Contribution and feedback is encouraged and always welcome. For more information about how to contribute, the project structure, as well as additional contribution information, see our [Contribution Guidelines](./CONTRIBUTING.md). By participating in this project, you agree to abide by its [Code of Conduct](./CODE_OF_CONDUCT.md) at all times.

## Contributors

Our commitment to open source means that we are enabling -in fact encouraging- all interested parties to contribute and become part of its developer community.

## Licensing

Copyright (C) 2021 Deutsche Telekom AG.

Licensed under the **BSD 3-Clause License** (the "License"); you may not use this file except in compliance with the License.

You may obtain a copy of the License by reviewing the file [LICENSE](./LICENSE) in the repository.

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the [LICENSE](./LICENSE) for the specific language governing permissions and limitations under the License.
