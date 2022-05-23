*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process

*** Keywords ***
Restore Initial Running Datastore
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${Routing Init Running Str}   json
