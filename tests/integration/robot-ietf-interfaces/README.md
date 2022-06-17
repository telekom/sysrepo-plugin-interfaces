# Robot IETF Interfaces
Robot framework tests via the Sysrepo robot library.

## `__init__.robot`
Initializes a default Sysrepo connection as well as a running  datastore session at the start of each test.
At the end of a test, the initial datastore data is restored as well as all connections and sessions being closed. 

## `InterfaceTests.robot`, `IpTests.robot`
These files contain integration test cases

## `InterfaceKeywords.resource`
Contains robot specific composite keywords.

## `InterfaceVariables.py`
Creates robot variables through the `getVariable` function.
Key, value pairs of the returned dictionary are mapped to robot Variable names and values respectively.

## `Utils.py`
Extended robot custom keywords written in Python 3.
