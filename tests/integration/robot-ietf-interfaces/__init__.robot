*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON

Suite Setup         Setup IETF Interfaces
Suite Teardown      Close All Sysrepo Connections And Sessions


*** Variables ***
${Xpath Interfaces}     /ietf-interfaces:interfaces
${Format}               json
${Running Datastore}    running


*** Keywords ***
Setup IETF Interfaces
    ${Connection Default}=    Open Sysrepo Connection
    ${Session Running}=    Open Datastore Session    ${Connection Default}    ${Running Datastore}
    ${If Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Interfaces}
    ...    ${Format}
    &{If JSON}=    Convert String To JSON    ${If Str}
    Set Global Variable    ${Connection Default}
    Set Global Variable    ${Session Running}
    Set Global Variable    ${If Str}
    Set Global Variable    ${If JSON}
