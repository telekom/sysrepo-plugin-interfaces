*** Settings ***
Library         Collections
Library         OperatingSystem
Library         String
Library         SysrepoLibrary
Library         RPA.JSON
Library         Utils.py
Variables       InterfaceVariables.py
Resource        InterfaceKeywords.resource


*** Test Cases ***
Test interfaces name get
    [Documentation]    Check if datastore names match system network interface names
    @{If Names}=    Get Values From JSON    ${If JSON}    $..name
    @{Sys Names}=    List Directory    /sys/class/net
    Lists Should Be Equal    ${If Names}    ${Sys names}

Test interfaces description
    [Documentation]    Check if datastore descriptions are empty
    @{If Descriptions}=    Get Values From JSON    ${If JSON}    $..description
    List Elements Should Be Empty    ${If Descriptions}

Test interfaces types
    [Documentation]    Check if datastore contains only supported types
    @{If Types}=    Get Values From JSON    ${If JSON}    $..type
    List Should Contain Sub List    ${Supported If Types}    ${If Types}

Test interfaces network interface status
    [Documentation]    Check if datastore interface state matches system state
    @{If Names}=    Get Values From JSON    ${If JSON}    $..name
    @{If Enabled}=    Get Values From JSON    ${If JSON}    $..enabled
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
