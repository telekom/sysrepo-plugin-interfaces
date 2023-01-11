*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process

*** Variables ***
${Xpath Routing}            /ietf-routing:routing
${Running Datastore}        running
${Plugin Timeout Seconds}   0.5s

*** Keywords ***
Restore Initial Running Datastore
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${Route Init Str}   json
    Check If Datastore Is Restored

Check If Datastore Is Restored
    [Documentation]     Make sure the initial datastore is restored
    ${Restored}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Routing}
    ...    json
    Should Be Equal As Strings    ${Restored}    ${Route Init Str}    msg="failed to restore inital running datastore"

Init Running Session
    ${Session Running}=    Open Datastore Session    ${Connection Default}    ${Running Datastore}
    Set Global Variable    ${Session Running}
    ${Route Init Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Routing}
    ...    json
    Set Global Variable    ${Route Init Str}
    &{Route Init JSON}=    Convert String To JSON    ${Route Init Str}
    Set Global Variable    ${Route Init JSON}

Connect Routing
    Start Plugin
    ${Connection Default}=    Open Sysrepo Connection
    Set Global Variable    ${Connection Default}
    Init Running Session

Cleanup Routing
    Restore Initial Running Datastore
    Stop Plugin

Start Plugin
    ${Plugin}=    Start Process    %{SYSREPO_ROUTING_PLUGIN_PATH}
    Set Global Variable    ${Plugin}
    Wait For Process    ${Plugin}    timeout=${Plugin Timeout Seconds}    on_timeout=continue

Stop Plugin
    Terminate Process    ${Plugin}
