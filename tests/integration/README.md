# ietf-system plugin integration tests

This directory contains the integration tests and required data for the ietf-interfaces plugin.

The tests are located in `ietf-interfaces.py` or in the directories starting with `robot-*` in case of Robot framework tests, while the test data is in XML files in `data`.

# Dependencies

## Python 3
As the integration tests are writen in python, several dependencies are required
in order to run the tests.

The easiest way to set up the dependencies and use the tests is by creating a python
virtual environment and installing the dependencies from the `requirements.txt` file.

```
python3 -m venv test-venv

source test-venv

pip3 install -r requirements.txt
```

## Robot framework

Make sure to have Sysrepo and the accompanying plugins installed under `devel`.

Create a virtual environment, activate it and make sure `pip`, `setuptools` and `wheel` are up to date.
Finally install the packages.

```
$ python3 -m venv sysrepolibrary-venv
$ source sysrepolibrary-venv/bin/activate
$ python3 -m pip install --upgrade pip setuptools wheel
$ python3 -m pip install rpaframework SysrepoLibrary robotframework-tidy
```

To autoformat the robot code:
```
$ robotidy robot-ietf-interfaces/
```


# Running the tests

## Python 3
After setting up the dependencies, to run the tests either execute the `.py` file or
run it in a python interpreter.

The tests will require an environment variables to be set.
The variable is `SYSREPO_GENERAL_PLUGIN_PATH`, the path to the plugin built in standalone mode.

As the tests are written by using the python unit-test library, the output looks
something like this:

```
 SYSREPO_GENERAL_PLUGIN_PATH=/home/user/src/sysrepo-plugin-general/build/sysrepo-plugin-general python3 ietf-interfaces.py
...
----------------------------------------------------------------------
Ran 3 tests in 6.182s

OK
```

## Robot framework
Note the root privileges when invoking the command (datastore permission issues otherwise, item not found):

The plugin wait start time can be modified via the env var `SYSREPO_INTEGRATION_PLUGIN_TIMEOUT_SECONDS`. The default value is `0.5s`. It's set in `robot-ietf-interfaces/InterfacesInit.resource` and `robot-ietf-routing/RoutingInit.resource`.

To test the interfaces plugin (change env var path):
```
# SYSREPO_INTERFACES_PLUGIN_PATH=/path/to/interfaces/plugin/executable robot robot-ietf-interfaces
```
To test the routing plugin:
```
#  SYSREPO_ROUTING_PLUGIN_PATH=/path/to/routing/plugin/executable robot robot-ietf-routing`
```
