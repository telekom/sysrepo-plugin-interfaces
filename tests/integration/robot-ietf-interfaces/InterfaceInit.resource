*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process

*** Keywords ***
Restore Initial Running Datastore
    Edit Datastore Config    ${Connection Default}    ${Session Running}    ${If Init Str}   json
