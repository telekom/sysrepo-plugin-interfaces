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
Resource            InterfaceInit.resource


*** Test Cases ***
Test interfaces name get
    [Documentation]    Check if datastore names match system network interface names
    @{If Names}=    Get Values From JSON    ${If Init JSON}    $..name
    @{Sys Names}=    List Directory    /sys/class/net
    Lists Should Be Equal    ${If Names}    ${Sys names}    ignore_order=True

Test interfaces description
    [Documentation]    Check if datastore descriptions are empty
    @{If Descriptions}=    Get Values From JSON    ${If Init JSON}    $..description
    List Elements Should Be Empty    ${If Descriptions}

Test interfaces types
    [Documentation]    Check if datastore contains only supported types
    @{If Types}=    Get Values From JSON    ${If Init JSON}    $..type
    List Should Contain Sub List    ${Supported If Types}    ${If Types}

Test interfaces network status
    [Documentation]    Check if datastore interface state matches system state
    @{If Names}=    Get Values From JSON    ${If Init JSON}    $..name
    @{If Enabled}=    Get Values From JSON    ${If Init JSON}    $..enabled
    &{States}=    Create Dict From Lists    ${If Names}    ${If Enabled}
    FOR    ${Name}    ${Enabled}    IN    &{States}
        ${Sys Oper Path}=    Set Variable    /sys/class/net/${Name}/operstate
        File Should Exist    ${Sys Oper Path}
        ${Sys Enabled}=    Get File    ${Sys Oper Path}
        ${Enabled Norm}=    Set Variable If    "${Sys Enabled.strip()}" == "up"
        ...    True
        ...    False
        Should Be Equal As Strings    ${Enabled}    ${Enabled Norm}
    END

Test Interface Lo Enable
    [Documentation]    Attempt to enable loopback interface
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_enabled.xml   xml
    ${Lo Enabled}=    Sysrepo Get Json Fields    /ietf-interfaces:interfaces/interface[name="lo"]/enabled    enabled
    Should Be True    ${Lo Enabled}
    Interface Operstate Should Be Up    lo

Test Interface Lo Description
    [Documentation]    Attempt to change loopback interface description
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_description.xml   xml
    ${Lo Description}=    Sysrepo Get Json Fields    /ietf-interfaces:interfaces/interface[name="lo"]/description    description
    Should Be Equal As Strings    "${Lo Description}[0]"    "test"
    
Test Interface Lo Rename
    [Documentation]    Make sure that attempts to rename the loopback interface fail
    TRY
        Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_rename.xml   xml
    EXCEPT    AS    ${Err}
        Should Not Be Empty   ${Err} 
    END
    ${Lo Name}=    Sysrepo Get Json Fields    /ietf-interfaces:interfaces/interface[name="lo"]/name    name
    Should Be Equal As Strings    "${Lo Name}[0]"    "lo"

Test Interface Lo Change Type
    [Documentation]    Make sure that attempts to change the loopback interface type fail
    TRY
        Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/loopback_change_type.xml   xml
    EXCEPT    AS    ${Err}
        Should Not Be Empty   ${Err} 
    END
    ${Lo Type}=    Sysrepo Get Json Fields    /ietf-interfaces:interfaces/interface[name="lo"]/type    type
    Should Be Equal As Strings    "${Lo Type}[0]"    "iana-if-type:ethernetCsmacd"

Test Interface Dummy
    [Documentation]    Attempt to add a dummy interface
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/dummy.xml   xml
    ${Dummy Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    /ietf-interfaces:interfaces/interface[name="dummy"]
    ...    xml
    Should Be Equal As Strings    ${Dummy Str}    ${Expected Dummy}
    File Should Exist    /sys/class/net/dummy
    Interface Operstate Should Be Up    dummy
