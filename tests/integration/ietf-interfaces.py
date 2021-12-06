#
# telekom / sysrepo-plugin-system
#
# This program is made available under the terms of the
# BSD 3-Clause license which is available at
# https://opensource.org/licenses/BSD-3-Clause
#
# SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
# SPDX-FileContributor: Sartura Ltd.
#
# SPDX-License-Identifier: BSD-3-Clause
#

import unittest
import sysrepo
import os
import subprocess
import signal
import time
import json
import operator

class InterfacesTestCase(unittest.TestCase):
    def setUp(self):
        plugin_path = os.environ.get('SYSREPO_INTERFACES_PLUGIN_PATH')
        if plugin_path is None:
            self.fail(
                "SYSREPO_INTERFACES_PLUGIN_PATH has to point to interfaces plugin executable")

        self.plugin = subprocess.Popen(
            [plugin_path],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        self.conn = sysrepo.SysrepoConnection()
        self.session = self.conn.start_session("running")
        time.sleep(2)

        self.initial_data = self.session.get_data_ly('/ietf-system:system')

    def tearDown(self):
        self.session.stop()
        self.conn.disconnect()
        self.plugin.send_signal(signal.SIGINT)
        self.plugin.wait()

    def load_initial_data(self, path):
        ctx = self.conn.get_ly_ctx()

        self.session.replace_config_ly(None, "ietf-interfaces")
        with open(path, "r") as f:
            data = f.read()
            data = ctx.parse_data_mem(data, "xml", config=True, strict=True)
            self.session.replace_config_ly(data, "ietf-interfaces")

    def edit_config(self, path):
        ctx = self.conn.get_ly_ctx()

        with open(path, "r") as f:
            data = f.read()
            data = ctx.parse_data_mem(data, "xml", config=True, strict=True)
            self.session.edit_batch_ly(data)
            data.free()

        self.session.apply_changes()

class InterfaceTestCase(InterfacesTestCase):
    def test_interface_name_get(self):
        data = self.session.get_data_ly('/ietf-interfaces:interfaces')
        interfaces = set(map(operator.itemgetter('name'), json.loads(data.print_mem("json"))['ietf-interfaces:interfaces']['interface']))

        real_interfaces = set(os.listdir('/sys/class/net'))

        self.assertEqual(real_interfaces, interfaces, "plugin and system interface list differ")

        data.free()

    def test_interface_description(self):
        data = self.session.get_data_ly('/ietf-interfaces:interfaces')
        interfaces = list(map(operator.itemgetter('description'), json.loads(data.print_mem("json"))['ietf-interfaces:interfaces']['interface']))

        for i in interfaces:
            self.assertEqual(i, "", "non empty interface description at startup")

        data.free()

    def test_interface_supported_types(self):
        """ Check if only supported types are present """

        data = self.session.get_data_ly('/ietf-interfaces:interfaces')

        interface = json.loads(data.print_mem('json'))['ietf-interfaces:interfaces']['interface']

        types = map(operator.itemgetter('type'), interface)
        types = set(typ.split(':')[-1] for typ in types)

        supported_types = {
            'softwareLoopback',
            'ethernetCsmacd',
            'l2vlan',
            'other',
        }
    
        self.assertTrue(types.issubset(supported_types), 'one or more types not based on the supported iana-if-type model')

        data.free()

    def test_interface_status(self):
        """ Check interface status """

        data = self.session.get_data_ly('/ietf-interfaces:interfaces')

        interface = json.loads(data.print_mem('json'))['ietf-interfaces:interfaces']['interface']

        names = map(operator.itemgetter('name'), interface)
        enabled = map(operator.itemgetter('enabled'), interface)

        states = dict(zip(names, enabled))

        for name, enabled in states.items():
            with open(f'/sys/class/net/{name}/operstate') as f:
                real_enabled = True if f.read().strip() == 'up' else False
                self.assertEqual(real_enabled, enabled, 'plugin and system interface state differ')

        data.free()

    def test_interface_lo_enable(self):
        """ Attempt to enable loopback interface """

        self.edit_config('data/loopback_enabled.xml')

        expected_lo_enabled = \
        '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>' \
        '<name>lo</name><enabled>true</enabled>' \
        '</interface></interfaces>' 

        data = self.session.get_data_ly('/ietf-interfaces:interfaces/interface[name="lo"]/enabled')

        lo_enabled = data.print_mem('xml')

        self.assertEqual(expected_lo_enabled, lo_enabled, 'loopback interface status not enabled (up)')

        with open('/sys/class/net/lo/operstate') as f:
            state = f.read().strip()
            self.assertEqual(state, 'up', 'system interface state unchanged')

        data.free()
        self.session.replace_config_ly(self.initial_data, 'ietf-system')

    def test_interface_lo_description(self):
        """ Attempt to change loopback description """

        self.edit_config('data/loopback_description.xml')

        expected_lo_description = \
        '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>' \
        '<name>lo</name><description>test</description>' \
        '</interface></interfaces>' 

        data = self.session.get_data_ly('/ietf-interfaces:interfaces/interface[name="lo"]/description')

        lo_description = data.print_mem('xml')

        self.assertEqual(expected_lo_description, lo_description, 'loopback interface description unchanged')

        # Check if description is saved
        desc_fname = 'interface_description'

        env_path =  os.environ.get('IF_PLUGIN_DATA_DIR')
            
        desc_path = os.path.join('/usr/local/lib/sysrepo-interfaces-plugin', desc_fname) \
                    if env_path is None \
                    else os.path.join(env_path, desc_fname)

        # TODO: add check when interface_description starts working
        with open(desc_path) as f:
            pass

        data.free()
        self.session.replace_config_ly(self.initial_data, 'ietf-system')


if __name__ == '__main__':
    unittest.main()

