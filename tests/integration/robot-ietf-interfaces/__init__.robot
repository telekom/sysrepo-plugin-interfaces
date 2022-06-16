*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process
Resource            InterfaceInit.resource

Suite Setup         Setup IETF Interfaces
Suite Teardown      Cleanup IETF Interfaces

Test Teardown       Restore Initial Running Datastore

*** Variables ***
${Xpath Interfaces}     /ietf-interfaces:interfaces
${Running Datastore}    running


*** Keywords ***
Setup IETF Interfaces
    [Documentation]    Create a default connection and running session
    Start Plugin
    ${Connection Default}=    Open Sysrepo Connection
    Set Global Variable    ${Connection Default}
    Init Running Session

Start Plugin
    ${Plugin}=    Start Process    %{SYSREPO_INTERFACES_PLUGIN_PATH}
    Set Suite Variable    ${Plugin}
    Wait For Process    ${Plugin}    timeout=2s    on_timeout=continue

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

Cleanup IETF Interfaces
    # Restore initial data
    Terminate Process    ${Plugin}
    Close All Sysrepo Connections And Sessions
