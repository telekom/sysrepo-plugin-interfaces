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

Test Bridging Filtering Database
    [Documentation]    Set filtering database entries for vids
    Run Process    ../bridging/interfaces_up.sh    shell=yes
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

Test Mac Address Filtering
    [Documentation]    Set MAC address filtering
    Run Process    ../bridging/br-filtering.sh    shell=yes
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/bridging/br-filtering.xml    xml

    ${Bridge Fdb}=    Run Process        bridge fdb show | grep 3e:85:b9:17:9b:04        shell=yes
    Should Contain    ${Bridge Fdb.stdout}    3e:85:b9:17:9b:04 dev veth1_bport vlan 20 master br0

Test Aging Time
    [Documentation]    Set aging time
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/bridging/aging.xml    xml

    ${Ageing Time}=    Run Process        ip -d link show br0 | grep -Po 'ageing_time.*'        shell=yes
    Should Contain    ${Ageing Time.stdout}    ageing_time 10000

Test Cleanup
    [Documentation]    Cleanup created veth interfaces
    Run Process    ../bridging/delete_veth.sh    shell=yes
