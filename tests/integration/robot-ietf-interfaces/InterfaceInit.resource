*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process

*** Variables ***
${Xpath Interfaces}     /ietf-interfaces:interfaces
${Running Datastore}    running

*** Keywords ***
Restore Initial Running Datastore
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${If Init Str}   json
    Check If Datastore Is Restored

Check If Datastore Is Restored
    [Documentation]     Make sure the initial datastore is restored
    ${Restored}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Interfaces}
    ...    json
    Should Be Equal As Strings    ${Restored}    ${If Init Str}    msg="failed to restore inital running datastore"

Init Running Session
    ${Session Running}=    Open Datastore Session    ${Connection Default}    ${Running Datastore}
    Set Global Variable    ${Session Running}
    ${If Init Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Interfaces}
    ...    json
    Set Global Variable    ${If Init Str}
    &{If Init JSON}=    Convert String To JSON    ${If Init Str}
    Set Global Variable    ${If Init JSON}

Connect Interfaces
    Start Plugin
    ${Connection Default}=    Open Sysrepo Connection
    Set Global Variable    ${Connection Default}
    Init Running Session

Cleanup Interfaces
    Restore Initial Running Datastore
    Cleanup IETF Interfaces

Start Plugin
    ${Plugin}=    Start Process    %{SYSREPO_INTERFACES_PLUGIN_PATH}
    Set Global Variable    ${Plugin}
    Wait For Process    ${Plugin}    timeout=0.1s    on_timeout=continue

Cleanup IETF Interfaces
    Terminate Process    ${Plugin}
