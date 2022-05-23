*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process
Resource            RoutingInit.resource

Suite Setup         Setup IETF Routing
Suite Teardown      Cleanup IETF Routing

Test Teardown       Restore Initial Running Datastore

*** Variables ***
${Xpath Routing}            /ietf-routing:routing
${Operational Datastore}    operational
${Running Datastore}        running 


*** Keywords ***
Setup IETF Routing
    [Documentation]    Create a default connection and running and operational sessions
    Start Plugin
    ${Connection Default}=    Open Sysrepo Connection
    Set Global Variable    ${Connection Default}
    Init Running Session
    Init Operational Session

Start Plugin
    ${Plugin}=    Start Process    %{SYSREPO_ROUTING_PLUGIN_PATH}
    Set Suite Variable    ${Plugin}
    Wait For Process    ${Plugin}    timeout=2s    on_timeout=continue

Init Running Session
    ${Session Running}=    Open Datastore Session    ${Connection Default}    ${Running Datastore}
    Set Global Variable    ${Session Running}
    ${Routing Init Running Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session RUnning}
    ...    ${Xpath Routing}
    ...    json
    Set Global Variable    ${Routing Init Running Str}
    &{Routing Init Running JSON}=    Convert String To JSON    ${Routing Init Running Str}
    Set Global Variable    ${Routing Init Running JSON}

Init Operational Session
    ${Session Operational}=    Open Datastore Session    ${Connection Default}    ${Operational Datastore}
    Set Global Variable    ${Session Operational}
    ${Routing Init Operational Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Operational}
    ...    ${Xpath Routing}
    ...    json
    Set Global Variable    ${Routing Init Operational Str}
    &{Routing Init Operational JSON}=    Convert String To JSON    ${Routing Init Operational Str}
    Set Global Variable    ${Routing Init Operational JSON}

Cleanup IETF Routing
    [Documentation]    Restore initial running datastore
    Terminate Process    ${Plugin}
    Close All Sysrepo Connections And Sessions

