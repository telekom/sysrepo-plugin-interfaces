*** Settings ***
Library     BuiltIn


*** Keywords ***
List Elements Should Be Empty
    [Arguments]    ${List}
    FOR    ${Item}    IN    @{List}
        Should Be Equal As Strings    ${Item}    ${EMPTY}
    END
