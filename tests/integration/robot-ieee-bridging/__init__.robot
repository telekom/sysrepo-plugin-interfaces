*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process
Resource            BridgingInit.resource

Test Setup          Connect Bridging
Test Teardown       Cleanup Bridging
