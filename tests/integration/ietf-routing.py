import unittest
import sysrepo
import os
import subprocess
import signal
import time
import json
import operator


class RoutingTestCase(unittest.TestCase):
    def setUp(self):
        plugin_path = os.environ.get('SYSREPO_ROUTING_PLUGIN_PATH')
        if plugin_path is None:
            self.fail(
                "SYSREPO_ROUTING_PLUGIN_PATH has to point to interfaces plugin executable")

        self.plugin = subprocess.Popen(
            [plugin_path],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        self.conn = sysrepo.SysrepoConnection()
        self.session = self.conn.start_session("running")
        self.operational_session = self.conn.start_session('operational')
        time.sleep(2)

    def tearDown(self):
        self.session.stop()
        self.conn.disconnect()
        self.plugin.send_signal(signal.SIGINT)
        self.plugin.wait()

    def load_initial_data(self, path):
        ctx = self.conn.get_ly_ctx()

        self.session.replace_config_ly(None, "ietf-routing")
        with open(path, "r") as f:
            data = f.read()
            data = ctx.parse_data_mem(data, "xml", config=True, strict=True)
            self.session.replace_config_ly(data, "ietf-routing")

    def edit_config(self, path):
        ctx = self.conn.get_ly_ctx()

        with open(path, "r") as f:
            data = f.read()
            data = ctx.parse_data_mem(data, "xml", config=True, strict=True)
            self.session.edit_batch_ly(data)
            data.free()

        self.session.apply_changes()


class InterfacesTestCase(RoutingTestCase):
    def test_interface_name_get(self):

        data = self.operational_session.get_data(
            '/ietf-routing:routing/interfaces')

        # interfaces
        interfaces = set([
            interface for interface in data['routing']['interfaces']['interface']])

        # system interfaces
        real_interfaces = set(os.listdir('/sys/class/net'))

        self.assertEqual(interfaces, real_interfaces,
                         "interfaces are different")


if __name__ == '__main__':
    unittest.main()
