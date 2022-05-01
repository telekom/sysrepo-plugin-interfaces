*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON

Suite Setup         Setup IETF Interfaces
Suite Teardown      Cleanup IETF Interfaces


*** Variables ***
${Xpath Interfaces}     /ietf-interfaces:interfaces
${Format JSON}          json
${Running Datastore}    running


*** Keywords ***
Setup IETF Interfaces
    ${Connection Default}=    Open Sysrepo Connection
    ${Session Running}=    Open Datastore Session    ${Connection Default}    ${Running Datastore}
    ${If Init Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Interfaces}
    ...    ${Format JSON}
    &{If Init JSON}=    Convert String To JSON    ${If Init Str}
    Set Global Variable    ${Connection Default}
    Set Global Variable    ${Session Running}
    Set Global Variable    ${If Init Str}
    Set Global Variable    ${If Init JSON}

Cleanup IETF Interfaces
    # Restore initial data
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${If Init Str}   json
    Close All Sysrepo Connections And Sessions
