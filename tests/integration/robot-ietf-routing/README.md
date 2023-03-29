# Robot IETF Routing
Robot framework tests via the Sysrepo robot library.

## `__init__.robot`
Starts the plugin, initializes a default Sysrepo connection as well as running and operational datastore sessions at the start of each test.
At the end of a test, the initial datastore data is restored and checked for success as well as all connections and sessions being closed. 

## `RoutingInit.resource`
Contains all the keywords used for test setup/teardown in `__init__.robot`

## `InterfaceTests.robot`, `RoutesTests.robot`, `StaticRoutesTests.robot`
These files contain integration test cases

## `RoutingKeywords.resource`
Contains robot specific composite keywords.

## `RoutingVariables.py`
Creates robot variables through the `getVariable` function.
Key, value pairs of the returned dictionary are mapped to robot Variable names and values respectively.
