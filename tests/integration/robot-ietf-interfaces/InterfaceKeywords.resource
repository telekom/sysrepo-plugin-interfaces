*** Settings ***
Library     BuiltIn
Library     OperatingSystem


*** Keywords ***
List Elements Should Be Empty
    [Arguments]    ${List}
    FOR    ${Item}    IN    @{List}
        Should Be Equal As Strings    ${Item}    ${EMPTY}
    END

Interface Operstate Should Be Up
    [Arguments]    ${If Name}
    ${Sys Enabled}=    Get File    /sys/class/net/${If Name}/operstate
    Should Be Equal As Strings    "${Sys Enabled.strip()}"    "up"

Sysrepo Get Json Fields
    [Arguments]    ${Path}    ${Field}
    ${Path Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Path}
    ...    json
    &{Path JSON}=    Convert String To JSON    ${Path Str}
    ${Fields}=    Get Values From Json    ${Path Json}    $..${Field}
    [Return]     ${Fields}
    
