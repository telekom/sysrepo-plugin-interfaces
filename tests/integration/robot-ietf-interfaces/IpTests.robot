*** Settings ***
Library         Collections
Library         OperatingSystem
Library         String
Library		    Process
Library		    BuiltIn
Library         SysrepoLibrary
Library         RPA.JSON
Library         Utils.py
Variables       InterfaceVariables.py
Resource        InterfaceKeywords.resource
Resource        InterfaceInit.resource


*** Test Cases ***
Test Ip Addr Prefix IPv4
    [Documentation]    Attempt to set loopback interface IPv4 address and subnet via prefix 
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_addr_prefix_ipv4.xml   xml
    ${Ip Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv4/address[ip="127.0.0.0"]
    ...    xml
    Should Be Equal As Strings    ${Ip Str}    ${Expected Ip Prefix Address}
    ${Proc Res}=    Run Process    ip   addr   show   lo
    Should Contain    ${Proc Res.stdout}    inet 127.0.0.1/8
    
Test Ip Addr Netmask IPv4
    [Documentation]    Attempt to set loopback interface IPv4 address and subnet via netmask
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_addr_netmask_ipv4.xml   xml
    ${Ip Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv4/address[ip="127.0.0.0"]
    ...    xml
    Should Be Equal As Strings    ${Ip Str}    ${Expected Ip Netmask Address}
    ${Proc Res}=    Run Process    ip   addr   show   lo
    Should Contain    ${Proc Res.stdout}    inet 127.0.0.1/8
    
Test Ip Addr Sub IPv6
    [Documentation]    Attempt to set loopback interface IPv6 address and subnet via prefix
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_addr_prefix_ipv6.xml   xml
    ${Ip Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv6/address
    ...    xml
    Should Be Equal As Strings    ${Ip Str}    ${Expected Ip Sub Address}
    ${Proc Res}=    Run Process    ip   addr   show   lo
    Should Contain    ${Proc Res.stdout}    inet6 ::1/128

Test Interface IPv4 Mtu
    [Documentation]    Attempt to change loopback interface mtu
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_mtu.xml   xml
    ${Mtu Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv4/mtu
    ...    xml
    Should Be Equal As Strings    ${Mtu Str}    ${Expected IPv4 Mtu}
    ${Sys Mtu Path}=    Set Variable    /sys/class/net/lo/mtu
    File Should Exist    ${Sys Mtu Path}
    ${Sys Mtu Str}=     Get File   ${Sys Mtu Path} 
    Should Be Equal As Strings    ${Sys Mtu Str.strip()}    12345

Test Interface IPv6 Mtu
    [Documentation]    Attempt to change loopback interface IPv6 mtu
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_ipv6_mtu.xml   xml
    ${Mtu Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv6/mtu
    ...    xml
    Should Be Equal As Strings    ${Mtu Str}    ${Expected IPv6 Mtu}
    ${Sys Mtu Path}=    Set Variable    /proc/sys/net/ipv6/conf/lo/mtu
    File Should Exist    ${Sys Mtu Path}
    ${Sys Mtu Str}=     Get File   ${Sys Mtu Path} 
    Should Be Equal As Strings    ${Sys Mtu Str.strip()}    1300

Test Interface IPv4 Forwarding
    [Documentation]    Attempt to enable loopback ipv4 address forwarding
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_ipv4_forwarding.xml   xml
    ${Forwarding Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv4/forwarding
    ...    xml
    Should Be Equal As Strings    ${Forwarding Str}    ${Expected IPv4 Forwarding}
    ${Sys Forwarding Path}=    Set Variable    /proc/sys/net/ipv4/conf/lo/forwarding
    File Should Exist    ${Sys Forwarding Path}
    ${Sys Forwarding Str}=     Get File   ${Sys Forwarding Path} 
    Should Be Equal As Strings    ${Sys Forwarding Str.strip()}    1

Test Interface IPv6 Forwarding
    [Documentation]    Attempt to enable loopback ipv6 address forwarding 
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_ipv6_forwarding.xml   xml
    ${Forwarding Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="lo"]/ietf-ip:ipv6/forwarding
    ...    xml
    Should Be Equal As Strings    ${Forwarding Str}    ${Expected IPv6 Forwarding}
    ${Sys Forwarding Path}=    Set Variable    /proc/sys/net/ipv6/conf/lo/forwarding
    File Should Exist    ${Sys Forwarding Path}
    ${Sys Forwarding Str}=     Get File   ${Sys Forwarding Path} 
    Should Be Equal As Strings    ${Sys Forwarding Str.strip()}    1

Test Interface IPv4 Neighbor
    [Documentation]     Attempt to add neighbor IPv4 address
    @{Sys Names}=    List Directory    /sys/class/net
    Remove Values From List    ${Sys Names}    lo    docker0
    ${Chosen If}=    Set Variable    ${Sys Names}[0]
    ${Neighbor Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="${Chosen If}"]/type
    ...    json
    &{Neighbor JSON}=    Convert String To JSON    ${Neighbor Str}
    @{If Type}=    Get Values From JSON    ${Neighbor JSON}    $..type
    ${If Type}=    Fetch From Right    ${If Type}[0]    :
    ${Test XML}=    Catenate    SEPARATOR=
    ...   <interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>
    ...   <name>${Chosen If}</name><type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">
    ...   ianaift:${If Type}</type><ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip"><neighbor>
    ...   <ip>192.0.2.2</ip><link-layer-address>00:00:5e:00:53:ab</link-layer-address>
    ...   </neighbor></ipv4></interface></interfaces>
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${Test XML}   xml
    ${Neighbor Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="${Chosen If}"]/ietf-ip:ipv4/neighbor
    ...    xml
    Should Contain    ${Neighbor Str}    <ip>192.0.2.2</ip><link-layer-address>00:00:5e:00:53:ab</link-layer-address>
    ${Proc Res}=    Run Process    ip   neigh   show    dev    ${Chosen If}
    Should Contain    ${Proc Res.stdout}    192.0.2.2 lladdr 00:00:5e:00:53:ab
    
Test Interface IPv6 Neighbor
    [Documentation]     Attempt to add neighbor IPv6 address
    @{Sys Names}=    List Directory    /sys/class/net
    Remove Values From List    ${Sys Names}    lo    docker0
    ${Chosen If}=    Set Variable    ${Sys Names}[0]
    ${Neighbor Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="${Chosen If}"]/type
    ...    json
    &{Neighbor JSON}=    Convert String To JSON    ${Neighbor Str}
    @{If Type}=    Get Values From JSON    ${Neighbor JSON}    $..type
    ${If Type}=    Fetch From Right    ${If Type}[0]    :
    ${Test XML}=    Catenate    SEPARATOR=
    ...   <interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>
    ...   <name>${Chosen If}</name><type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">
    ...   ianaift:${If Type}</type><ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip"><neighbor>
    ...   <ip>::ffff:c000:203</ip><link-layer-address>00:00:5e:00:53:ab</link-layer-address>
    ...   </neighbor></ipv6></interface></interfaces>
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${Test XML}   xml
    ${Neighbor Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="${Chosen If}"]/ietf-ip:ipv6/neighbor
    ...    xml
    Should Contain    ${Neighbor Str}    <ip>::ffff:192.0.2.3</ip><link-layer-address>00:00:5e:00:53:ab</link-layer-address>
    ${Proc Res}=    Run Process    ip   neigh   show    dev    ${Chosen If}
    Should Contain    ${Proc Res.stdout}    ::ffff:192.0.2.3 lladdr 00:00:5e:00:53:ab
    
Test Interface Subinterface
    [Documentation]    Attempt to create a vlan qinq subinterface 
    @{Sys Names}=    List Directory    /sys/class/net
    Remove Values From List    ${Sys Names}    lo
    # Select a parent interface
    FOR    ${Interface}    IN    @{Sys Names}
        ${Proc Res}=    Run Process    ip   link   show    ${Interface}
        IF    "link/ether" in """${Proc Res.stdout}"""
            ${Parent Interface}=    Set Variable    ${Interface}
            BREAK
        END
    END
    # Modify datastore
    ${Test Unformatted XML}=    Get File    data/subinterface.xml
    ${Test XML}=    Utils.Format String    ${Test Unformatted XML}    PARENT_INTERFACE=${Parent Interface}
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${Test XML}   xml
    ${Expected XML}=    Catenate    SEPARATOR=
    ...   <interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>
    ...   <name>${Parent Interface}.sub1</name>
    ...   <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:l2vlan</type>
    ...   <dot1q-vlan xmlns="urn:ietf:params:xml:ns:yang:ietf-if-vlan-encapsulation">
    ...   <outer-tag><tag-type xmlns:dot1q-types="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
    ...   dot1q-types:s-vlan</tag-type><vlan-id>10</vlan-id></outer-tag><second-tag>
    ...   <tag-type xmlns:dot1q-types="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
    ...   dot1q-types:c-vlan</tag-type><vlan-id>20</vlan-id></second-tag></dot1q-vlan></encapsulation>
    ...   <ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip"><address><ip>2001:db8:10::1</ip>
    ...   <prefix-length>48</prefix-length></address>
    ...   <dup-addr-detect-transmits>0</dup-addr-detect-transmits></ipv6>
    ...   <parent-interface xmlns="urn:ietf:params:xml:ns:yang:ietf-if-extensions">
    ...   ${Parent Interface}</parent-interface></interface></interfaces>
    ${Path}=    Set Variable   /ietf-interfaces:interfaces/interface[name="${Parent Interface}.sub1"]
    ${Subinterface Data}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Path}
    ...    xml
    Should Be Equal As Strings    ${Expected XML}    ${Subinterface Data}
    @{Sys Names}=    List Directory    /sys/class/net
    ${Subinterface Name}=    Set Variable    ${Parent Interface}.sub1
    Should Contain    ${Sys Names}    ${Subinterface Name}
    Should Contain    ${Sys Names}    ${Subinterface Name}.20
    ${Proc Res}=    Run Process    ip   addr   show   dev   ${Subinterface Name}
    Should Contain    ${Proc Res.stdout}    inet6 2001:db8:10::1/48
    ${Proc Res}=    Run Process    ip   -d    link    show    ${Subinterface Name}
    Should Contain    ${Proc Res.stdout}    vlan protocol 802.1ad id 10 
    ${Proc Res}=    Run Process    ip   -d    link    show    ${Subinterface Name}.20
    Should Contain    ${Proc Res.stdout}    vlan protocol 802.1Q id 20
