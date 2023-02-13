*** Settings ***
Library             SysrepoLibrary
Library             RPA.JSON
Library             Process
Resource            InterfaceInit.resource


Test Setup	    Connect Interfaces
Test Teardown       Cleanup Interfaces

