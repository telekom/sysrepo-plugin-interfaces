# syntax=docker/dockerfile:1.0.0-experimental
FROM sysrepo/sysrepo-netopeer2:latest

MAINTAINER juraj.vijtiuk@sartura.hr

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y\
	ssh \
	libnl-3-dev \
	libnl-route-3-dev \
	libnl-nf-3-dev \
	libnl-genl-3-dev

WORKDIR /opt/dev

RUN \
	mkdir -m 0600 -p /root/.ssh && ssh-keyscan lab.sartura.hr >> ~/.ssh/known_hosts

RUN \
	--mount=type=secret,id=sshkey,dst=/root/.ssh/id_ed25519 git clone git@lab.sartura.hr:sysrepo/sysrepo-plugin-interfaces.git

Run \
	cd sysrepo-plugin-interfaces && mkdir build && cd build && \
	git checkout master && \
	cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
	make -j2

RUN \
	sysrepoctl -i /opt/dev/sysrepo-plugin-interfaces/yang/ietf-interfaces@2018-02-20.yang && \
	sysrepoctl -i /opt/dev/sysrepo-plugin-interfaces/yang/iana-if-type@2017-01-19.yang && \
	sysrepoctl -i /opt/dev/sysrepo-plugin-interfaces/yang/ietf-interfaces-common@2019-03-05.yang && \
	sysrepoctl -i /opt/dev/sysrepo-plugin-interfaces/yang/ieee802-dot1q-types.yang && \
	sysrepoctl -i /opt/dev/sysrepo-plugin-interfaces/yang/ietf-if-vlan-encapsulation@2020-07-13.yang
