*** Settings ***
Library         Collections
Library         OperatingSystem
Library		    BuiltIn
Library         SysrepoLibrary
Library         RPA.JSON
Variables       RoutingVariables.py
Resource        RoutingKeywords.resource

*** Test Cases ***
Test Routing Name Get
    [Documentation]    Check if datastore names match system network interface names
    ${If Names Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Operational}
    ...    /ietf-routing:routing/interfaces
    ...    json
    &{If Names JSON}=    Convert String To JSON    ${If Names Str}
    @{If Names}=    Get Values From JSON    ${If Names JSON}    $..interface
    @{Sys Names}=    List Directory    /sys/class/net
    Lists Should Be Equal    ${If Names}[0]    ${Sys names}    ignore_order=True
