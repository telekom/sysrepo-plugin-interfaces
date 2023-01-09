*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process
Resource            InterfaceInit.resource


Suite Teardown      Cleanup IETF Interfaces

Test Setup	        Connect Interfaces
Test Teardown       Cleanup Interfaces

