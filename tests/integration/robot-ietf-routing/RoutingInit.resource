*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process

*** Variables ***
${Xpath Routing}            /ietf-routing:routing

*** Keywords ***
Restore Initial Running Datastore
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${Routing Init Running Str}   json
    Check If Datastore Is Restored

Check If Datastore Is Restored
    [Documentation]     Make sure the initial datastore is restored
    ${Restored}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Routing}
    ...    json
    Should Be Equal As Strings    ${Restored}    ${Routing Init Running Str}    msg="failed to restore inital running datastore"
