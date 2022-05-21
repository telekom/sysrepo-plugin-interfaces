*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON

Suite Setup         Setup IETF Interfaces
Suite Teardown      Cleanup IETF Interfaces


*** Variables ***
${Xpath Interfaces}     /ietf-interfaces:interfaces
${Running Datastore}    running


*** Keywords ***
Setup IETF Interfaces
    [Documentation]    Create a default connection and running session
    ${Connection Default}=    Open Sysrepo Connection
    Set Global Variable    ${Connection Default}
    Init Running Session

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
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${If Init Str}   json
    Close All Sysrepo Connections And Sessions
