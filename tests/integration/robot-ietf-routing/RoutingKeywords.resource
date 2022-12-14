*** Settings ***
Library     BuiltIn


*** Keywords ***
Running Get Json Fields
    [Arguments]    ${Path}    ${Field}
    ${Path Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Path}
    ...    json
    &{Path JSON}=    Convert String To JSON    ${Path Str}
    ${Fields}=    Get Values From Json    ${Path Json}    $..${Field}
    [Return]     ${Fields}

Operational Get Json Fields
    [Arguments]    ${Path}    ${Field}
    ${Path Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Operational}
    ...    ${Path}
    ...    json
    &{Path JSON}=    Convert String To JSON    ${Path Str}
    ${Fields}=    Get Values From Json    ${Path Json}    $..${Field}
    [Return]     ${Fields}
