import unittest
import sysrepo
import os
import subprocess
import signal
import time
import json
from pyroute2 import IPRoute


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


class StaticRoutesTestCase(RoutingTestCase):
    def test_static_routes(self):
        self.edit_config('data/routing/static-route.xml')

        data = self.session.get_data_ly(
            '/ietf-routing:routing/control-plane-protocols/control-plane-protocol[type="static"][name="static"]/static-routes')

        jobj = json.loads(data.print_mem('json'))

        routes = jobj['ietf-routing:routing']['control-plane-protocols']['control-plane-protocol'][0]['static-routes']['ietf-ipv4-unicast-routing:ipv4']['route']
        new_route = routes[0]

        # assert static-routes container
        self.assertEqual(new_route['description'], 'Test Route')
        self.assertEqual(new_route['destination-prefix'], '192.168.100.0/24')
        self.assertEqual(new_route['next-hop']['next-hop-address'], '10.0.2.1')

        # assert operational data working
        op_data = self.operational_session.get_data_ly(
            '/ietf-routing:routing/ribs/rib[name="ipv4-main"]')
        op_jobj = json.loads(op_data.print_mem('json'))
        op_routes = op_jobj['ietf-routing:routing']['ribs']['rib'][0]['routes']['route']

        in_operational = False

        for route in op_routes:
            if 'ietf-ipv4-unicast-routing:destination-prefix' in route and 'ietf-ipv4-unicast-routing:next-hop-address' in route['next-hop']:
                prefix = route['ietf-ipv4-unicast-routing:destination-prefix']
                nexthop = route['next-hop']['ietf-ipv4-unicast-routing:next-hop-address']

                if prefix != None and nexthop != None:
                    if prefix == '192.168.100.0/24':
                        self.assertEqual(nexthop, '10.0.2.1')
                        in_operational = True

        self.assertEqual(in_operational, True)

        # assert system applied change
        with IPRoute() as ipr:
            attrs = ipr.route('get', dst='192.168.100.1')[0]['attrs']
            gateway = list(
                filter(lambda tpl: tpl[0] == 'RTA_GATEWAY', attrs))[0][1]
            self.assertEqual(gateway, '10.0.2.1')

        op_data.free()
        data.free()

        pass


class RoutesTestCase(RoutingTestCase):
    def test_routes(self):
        data = self.operational_session.get_data_ly(
            '/ietf-routing:routing/ribs/rib[name="ipv4-main"]')

        obj = json.loads(data.print_mem('json'))

        routes_list = obj['ietf-routing:routing']['ribs']['rib'][0]['routes']['route']

        self.assertGreater(len(routes_list), 0)

        data.free()


if __name__ == '__main__':
    unittest.main()
