*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process

*** Variables ***
${Xpath Bridging}                    /ieee802-dot1q-bridge:bridges
${Xpath Interfaces}                  /ietf-interfaces:interfaces
${Xpath Vlan Operational}            /ieee802-dot1q-bridge:bridges/bridge/component/bridge-vlan
${Running Datastore}                 running
${Operational Datastore}             operational
${Plugin Timeout Seconds Default}    0.5s
${Plugin Timeout Seconds}            %{SYSREPO_INTEGRATION_PLUGIN_TIMEOUT_SECONDS=${Plugin Timeout Seconds Default}}

*** Keywords ***
Restore Initial Running Datastore
    IF    len(${Bridge Init Str}) > 0
        Edit Datastore Config    ${Connection Default}    ${Session Running}    ${Bridge Init Str}   json
        Check If Datastore Is Restored
    END

Check If Datastore Is Restored
    [Documentation]     Make sure the initial datastore is restored
    ${Restored}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Bridging}
    ...    json
    Should Be Equal As Strings    ${Restored}    ${Bridge Init Str}    msg="failed to restore inital running datastore"

Init Running Session
    ${Session Running}=    Open Datastore Session    ${Connection Default}    ${Running Datastore}
    Set Global Variable    ${Session Running}
    ${Bridge Init Str}=    Get Datastore Data
    ...    ${Connection Default}
    ...    ${Session Running}
    ...    ${Xpath Bridging}
    ...    json
    Set Global Variable    ${Bridge Init Str}
    &{Bridge Init JSON}=    Convert String To JSON    ${Bridge Init Str}
    Set Global Variable    ${Bridge Init JSON}

Connect Bridging
    Start Plugin
    ${Connection Default}=    Open Sysrepo Connection
    Set Global Variable    ${Connection Default}
    Init Running Session

Cleanup Bridging
    #Restore Initial Running Datastore
    Stop Plugin

Start Plugin
    ${Plugin}=    Start Process    %{SYSREPO_BRIDGING_PLUGIN_PATH}
    Set Global Variable    ${Plugin}
    Wait For Process    ${Plugin}    timeout=${Plugin Timeout Seconds}    on_timeout=continue

Stop Plugin
    Send Signal To Process    SIGINT    ${Plugin}
