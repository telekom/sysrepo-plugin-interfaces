*** Settings ***
Library         Collections
Library         OperatingSystem
Library		    BuiltIn
Library         SysrepoLibrary
Library         RPA.JSON
Variables       RoutingVariables.py
Resource        RoutingKeywords.resource

*** Test Cases ***
Test Routing Routes
    [Documentation]    Check datastore routes count nonzero
    @{Routes}=    Get Values From JSON    ${Routing Init Operational JSON}    $..route
    Should Not Be Empty    ${Routes}
