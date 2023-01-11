*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process
Resource            RoutingInit.resource


Test Setup	        Connect Routing
Test Teardown       Cleanup Routing

