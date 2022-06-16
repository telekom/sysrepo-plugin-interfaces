*** Settings ***
Library         Collections
Library         OperatingSystem
Library		    BuiltIn
Library         SysrepoLibrary
Library         RPA.JSON
Variables       RoutingVariables.py
Resource        RoutingKeywords.resource
Resource        RoutingInit.resource


*** Test Cases ***
Test Routing Static Routes
    [Documentation]    Check datastore static routes
    Edit Datastore Config By File    ${Connection Default}    ${Session Running}    data/routing/static-route.xml    xml
    @{Routes}=   Running Get Json Fields   
    ...    /ietf-routing:routing/control-plane-protocols/control-plane-protocol[type="static"][name="static"]/static-routes    
    ...    route
    Should Be Equal As Strings   Test Route    ${Routes}[0][0][description]
    Should Be Equal As Strings   192.168.100.0/24    ${Routes}[0][0][destination-prefix]
    Should Be Equal As Strings   10.0.2.1    ${Routes}[0][0][next-hop][next-hop-address]
    @{Operational Routes}=   Operational Get Json Fields   /ietf-routing:routing/ribs/rib[name="ipv4-main"]    route
    ${In Operational}=    Set Variable    False
    FOR    ${Route}    IN    @{Operational Routes}[0]
        IF    'ietf-ipv4-unicast-routing:destination-prefix' in ${Route} and 'ietf-ipv4-unicast-routing:next-hop-address' in ${Route}[next-hop]
            IF    "${Route}[ietf-ipv4-unicast-routing:destination-prefix]" == "192.168.100.0/24"
                Should Be Equal As Strings   ${Route}[next-hop][ietf-ipv4-unicast-routing:next-hop-address] 
                ...                          10.0.2.1
                ${In Operational}=    Set Variable    True
            END
        END
    END
    Should Be True    ${In Operational}
