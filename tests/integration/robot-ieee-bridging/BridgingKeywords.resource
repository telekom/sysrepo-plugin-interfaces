*** Settings ***
Library     BuiltIn
Library     OperatingSystem


*** Keywords ***
List Elements Should Be Empty
    [Arguments]    ${List}
    FOR    ${Item}    IN    @{List}
        Should Be Equal As Strings    ${Item}    ${EMPTY}
    END

Bridging Is Bridge
    [Arguments]    ${Br Name}
    Should Exist    /sys/class/net/${Br Name}/bridge

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
