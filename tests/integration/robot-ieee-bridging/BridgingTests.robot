*** Settings ***
Library         Collections
Library         OperatingSystem
Library         String
Library         Process
Library         BuiltIn
Library         SysrepoLibrary
Library         RPA.JSON
Library         Utils.py
Variables       BridgingVariables.py
Resource        BridgingKeywords.resource
Resource        BridgingInit.resource


*** Test Cases ***
Test Bridging Check Bridges
    [Documentation]    Load and check if datastore bridges match system network bridges
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/bridging/initial-bridge.xml    xml

    ${Bridge Init Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Bridging}
    ...    json
    &{Bridge Init JSON}=    Convert String To JSON    ${Bridge Init Str}

    @{Components Names}=    Get Values From JSON    ${Bridge Init JSON}    $..bridge[*].name
    Should Not Be Empty    ${Components Names}
    FOR    ${Component}    IN    @{Components Names}
        Bridging Is Bridge    ${Component}
    END

Test Bridging Veth
    [Documentation]    Add veth ports to bridge
    Run Process    ../bridging/setup_veth.sh    shell=yes
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    ../bridging/veth-ports.xml    xml

    ${Interfaces Init Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Interfaces}
    ...    json
    &{Interfaces Init JSON}=    Convert String To JSON    ${Interfaces Init Str}

    @{Veths Names}=    Get Values From JSON    ${Interfaces Init JSON}    $..interface[*].name
    Should Not Be Empty    ${Veths Names}
    FOR    ${Veth}    IN    @{Veths Names}
        IF    "veth" in """${Veth}"""
            Should Exist    /sys/class/net/${Veth}
        END
    END
    Run Process    ../bridging/interfaces_up.sh    shell=yes

Test Bridging Filtering Database
    [Documentation]    Set filtering database entries for vids
    Run Process    ../bridging/br_vlan.sh    shell=yes
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/bridging/br-vlan.xml    xml

    ${Vlan show}=    Run Process        bridge vlan show        shell=yes
    ${Vlan File}=    Get File    data/bridging/vlan_show.txt

    Should Be Equal As Strings    ${Vlan File}    ${Vlan show.stdout}

Test Bridging Vlan Operational
    [Documentation]    Check the operational datastore
    ${Session Operational}=    Open Datastore Session    ${Connection Default}    ${Operational Datastore}

    ${Vlan Init Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Operational}
    ...    ${Xpath Vlan Operational}
    ...    xml

    ${Vlan Operational File}=    Get File    data/bridging/vlan-operational.xml

    Should Be Equal As Strings    ${Vlan Operational File}    ${Vlan Init Str}

Test Bridging Vlan Transmitted
    [Documentation]    Invert vlan-transmitted values
    ${Veth1 Bport}=    Get File    /sys/class/net/veth1_bport/ifindex
    ${Veth2 Bport}=    Get File    /sys/class/net/veth2_bport/ifindex

    ${Veth1 Bport}=    Replace String    ${Veth1 Bport}    \n    ${EMPTY}
    ${Veth2 Bport}=    Replace String    ${Veth2 Bport}    \n    ${EMPTY}

    ${Tagged XML}=    Catenate    SEPARATOR=
    ...   <bridges xmlns="urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge"><bridge>
    ...   <name>br0</name><address>42-50-a1-c4-23-78</address>
    ...   <bridge-type>customer-vlan-bridge</bridge-type><component><name>br0</name>
    ...   <type>c-vlan-component</type><address>42-50-a1-c4-23-78</address><filtering-database>
    ...   <vlan-registration-entry><database-id>0</database-id><vids>20</vids>
    ...   <port-map xmlns:dot1qtypes="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
    ...   <port-ref>${Veth1 Bport}</port-ref><static-vlan-registration-entries>
    ...   <vlan-transmitted>tagged</vlan-transmitted></static-vlan-registration-entries></port-map>
    ...   <port-map xmlns:dot1qtypes="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
    ...   <port-ref>${Veth2 Bport}</port-ref><static-vlan-registration-entries>
    ...   <vlan-transmitted>untagged</vlan-transmitted>
    ...   </static-vlan-registration-entries></port-map></vlan-registration-entry>
    ...   <vlan-registration-entry><database-id>0</database-id><vids>8-10</vids>
    ...   <port-map xmlns:dot1qtypes="urn:ieee:std:802.1Q:yang:ieee802-dot1q-types">
    ...   <port-ref>${Veth2 Bport}</port-ref><static-vlan-registration-entries>
    ...   <vlan-transmitted>untagged</vlan-transmitted>
    ...   </static-vlan-registration-entries></port-map></vlan-registration-entry>
    ...   </filtering-database></component></bridge></bridges>

    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${Tagged XML}   xml

    ${Vlan show}=    Run Process        bridge vlan show        shell=yes
    ${Vlan File}=    Get File    data/bridging/vlan_transmitted_inverted_show.txt

    Should Be Equal As Strings    ${Vlan File}    ${Vlan show.stdout}

Test Mac Address Filtering
    [Documentation]    Set MAC address filtering
    Run Process    ../bridging/br-filtering.sh    shell=yes
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/bridging/br-filtering.xml    xml

    ${Bridge Fdb}=    Run Process        bridge fdb show | grep 3e:85:b9:17:9b:04        shell=yes
    Should Contain    ${Bridge Fdb.stdout}    3e:85:b9:17:9b:04 dev veth1_bport vlan 20 master br0 permanent

Test Aging Time
    [Documentation]    Set aging time
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/bridging/aging.xml    xml

    ${Ageing Time}=    Run Process        ip -d link show br0 | grep -Po 'ageing_time.*'        shell=yes
    Should Contain    ${Ageing Time.stdout}    ageing_time 10000
