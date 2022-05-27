*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process

*** Variables ***
${Xpath Interfaces}     /ietf-interfaces:interfaces

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
