# ietf-system plugin integration tests

This directory contains the integration tests and required data for the ietf-interfaces plugin.

The tests are located in `ietf-interfaces.py` while the test data is in XML files in `data/`.

# Dependencies

As the integration tests are writen in python, several dependencies are required
in order to run the tests.

The easiest way to set up the dependencies and use the tests is by creating a python
virtual environment and installing the dependencies from the `requirements.txt` file.

```
python3 -m venv test-venv

source test-venv

pip3 install -r requirements.txt
```

# Running the tests

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
